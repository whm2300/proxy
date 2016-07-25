#include "server.h"

#include <stdio.h>
#include <string>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"
#include "protocol/command.h"
#include "protocol/proxy.pb.h"
#include "util/log.h"

Server::Server():_evbase(NULL), _manage_bev(NULL) {
}

Server::~Server() {
}
bool Server::ConnectToPeer(const char *ip, uint16_t port) {
    int fd = ConnectBlock(ip, port);
    if (fd == -1){
        return false;
    }
    _manage_bev = bufferevent_socket_new(_evbase, fd, 
                BEV_OPT_CLOSE_ON_FREE);
    if (_manage_bev == NULL) {
        return false;
    }

    _packet.MakePacket(NULL, LOCAL_REGISTER);
    bufferevent_write(_manage_bev, _packet.get_send_data(), 
                _packet.get_send_len());

    bufferevent_setcb(_manage_bev, LocalSocketReadCallback, NULL, 
                LocalSocketEventCallback, this);
    bufferevent_enable(_manage_bev, EV_READ | EV_WRITE);

    return true;
}

int Server::ConnectBlock(const char *ip, uint16_t port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd <= 0) {
        log_error("get socket fd error", strerror(errno));
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        log_error("connect error. ip:%s, port:%d", ip, port);
        close(fd);
        return -1;
    }

    int flags;
    if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
        log_error("fcntl(%d, F_GETFL)", fd);
        close(fd);
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        log_error("fcntl(%d, F_SETFL)", fd);
        close(fd);
        return -1;
    }
    return fd;
}

bool Server::InitServer() {
    if (access(CONFIG->get_pidfile_path().c_str(), F_OK) == 0) {
        log_error("pidfile exist. path:%s", CONFIG->get_pidfile_path().c_str());
        return false;
    }

    //libevent
    //event_set_log_callback(log_for_libevent);
    //event_enable_debug_mode();
    _evbase = event_base_new();
    if (_evbase == NULL){
        return false;
    }
    log_info("use method:%s, %ld", event_base_get_method(_evbase), _evbase);

    if (!ConnectToPeer(CONFIG->get_ip().c_str(), CONFIG->get_port())) {
        log_error("connect to peer error");
        return false;
    }
    log_debug("connect ok");

    if (!InitSignalAndTimer()){
        return false;
    }

    return true;
}

bool Server::InitSignalAndTimer() {
    signal(SIGPIPE, SIG_IGN);

    struct event *sigint_event = evsignal_new(_evbase, SIGINT, 
                SignalCallback, this);
    if (sigint_event == NULL || event_add(sigint_event, NULL) < 0){
        log_error("Could not create/add SIGINT event");
        return false;
    }
    struct event *sigterm_event = evsignal_new(_evbase, SIGTERM, 
                SignalCallback, this);
    if (sigterm_event == NULL || event_add(sigterm_event, NULL) < 0 ){
        log_error("Could not create/add SIGTERM event");
        return false;
    }
    struct event *timer_event = event_new(_evbase, -1, EV_PERSIST, SendHeartBeat, this);
    {
        struct timeval tv;
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        if (timer_event == NULL || evtimer_add(timer_event, &tv) < 0){
            log_error("%s", "could not create/add a timer event");
            return false;
        }
    }
    return true;
}

void Server::SendHeartBeat(evutil_socket_t sig, short events, void *user_data) {
    log_debug("send heart beat");
    Server *server = (Server *)user_data;
    server->_packet.MakePacket(NULL, HEART_BEAT);
    bufferevent_write(server->_manage_bev, server->_packet.get_send_data(), 
                server->_packet.get_send_len());
}

void Server::SignalCallback(evutil_socket_t sig, short events, 
            void *user_data) {
    exit(0);
}

bool Server::RunServer() {
    /*
    {
        int pidfile = open(CONFIG->get_pidfile_path().c_str(), 
                    O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (pidfile == -1){
            log_error("Could not open pidfile. %s", strerror(errno));
            return false;
        }
        char buf[21] = {0};
        sprintf(buf, "%d", getpid());
        write(pidfile, buf, strlen(buf));
        ::close(pidfile);
    }
    */

    int res = event_base_dispatch(_evbase);

    log_info("main loop exit. res:%d", res);
    //remove(CONFIG->get_pidfile_path().c_str());
    return true;
}

void Server::LocalSocketReadCallback(struct bufferevent *bev, 
            void *user_data) {
    Server *server = (Server *)user_data;
    assert(server != NULL);
    struct evbuffer *input = bufferevent_get_input(bev);
    if (input == NULL) {
        return ;
    }
    BuffereventMap::iterator it_map = server->_local_to_real.find(bev);
    if (it_map != server->_local_to_real.end()) {
        //代理连接，已存在映射。
        if (it_map->second != NULL) {
            server->MoveData(bev, it_map->second);
        } else {
            server->_local_to_real.erase(bev);
            close(bufferevent_getfd(bev));
            log_info("close bufferevent_getfd");
            bufferevent_free(bev);
        }
    }
    if (!server->_packet.ParsePacket(input)) {
        return ;
    }
    server->ProcessMsg(bev);
}

void Server::ProcessMsg(struct bufferevent *bev) {
    assert(bev != NULL);
    uint16_t cmd = _packet.get_cmd();
    switch (cmd) {
        case PEER_REGISTER:
            ProcessNewProxy(bev);
            log_info("local proxy");
            break;
        default :
            log_error("error cmd:%d", cmd);
            break;
    }
}

void Server::ProcessNewProxy(struct bufferevent *bev) {
    NewConnection msg;
    msg.ParseFromArray(_packet.get_msg_data(), _packet.get_msg_len());

    //连接到服务程序
    int fd = ConnectBlock("127.0.0.1", (uint16_t)msg.to_prot());
    if (fd == -1){
        log_error("connect error. port:%d", msg.to_prot());
        return ;
    }
    struct bufferevent *real_bev = bufferevent_socket_new(_evbase, fd, 
                BEV_OPT_CLOSE_ON_FREE);
    if (real_bev == NULL) {
        return ;
    }

    //连接到peer
    fd = ConnectBlock(CONFIG->get_ip().c_str(), CONFIG->get_port());
    if (fd == -1){
        log_error("connect error. port:%d", msg.to_prot());
        return ;
    }
    struct bufferevent *local_bev = bufferevent_socket_new(_evbase, fd, 
                BEV_OPT_CLOSE_ON_FREE);
    if (local_bev == NULL) {
        return ;
    }

    bufferevent_setcb(local_bev, LocalSocketReadCallback, NULL, 
                LocalSocketEventCallback, this);
    bufferevent_enable(local_bev, EV_READ | EV_WRITE);

    //发送注册
    _packet.MakePacket(&msg, LOCAL_PROXY);
    bufferevent_write(local_bev, _packet.get_send_data(), 
                _packet.get_send_len());

    //发送代理数据
    MoveData(real_bev, local_bev);

    //建立映射关系
    _local_to_real[local_bev] = real_bev;
    _real_to_local[real_bev] = local_bev;

    //设置回调
    bufferevent_setcb(real_bev, RealSocketReadCallback, NULL, 
                RealSocketEventCallback, this);
    bufferevent_enable(real_bev, EV_READ | EV_WRITE);
    bufferevent_enable(local_bev, EV_READ | EV_WRITE);
}

void Server::LocalSocketEventCallback(struct bufferevent *bev, 
            short events, void *user_data) {
    Server *server = (Server *)user_data;
    if (bev == server->_manage_bev) {
        log_fatal("manage bev close, exit.");
        exit(0);
    }
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        log_info("close local socket");
        BuffereventMap::iterator it = server->_local_to_real.find(bev);
        if (it != server->_local_to_real.end()) {
            log_info("find");
            server->MoveData(bev, it->second);
            server->_real_to_local.erase(it->second);
            close(bufferevent_getfd(it->second));
            bufferevent_free(it->second);
        }
        server->_local_to_real.erase(it);
        close(bufferevent_getfd(bev));
        bufferevent_free(bev);
        log_debug("close local socket over");
    }
}

void Server::RealSocketReadCallback(struct bufferevent *bev, void *user_data) {
    Server *server = (Server *)user_data;
    assert(server != NULL);
    BuffereventMap::iterator it_map = server->_real_to_local.find(bev);
    if (it_map == server->_real_to_local.end()) {
        log_error("send to proxy error");
    } else {  //转发连接已存在，直接转发。
        server->MoveData(bev, it_map->second);
        log_info("peer to local");
    }
}

void Server::RealSocketEventCallback(struct bufferevent *bev, short events, 
            void *user_data) {
    Server *server = (Server *)user_data;
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        log_info("close peer socket");
        BuffereventMap::iterator it = server->_real_to_local.find(bev);
        if (it != server->_real_to_local.end()) {
            server->MoveData(bev, it->second);
            server->_local_to_real.erase(it->second);
            close(bufferevent_getfd(it->second));
            bufferevent_free(it->second);
        }
        server->_real_to_local.erase(it);
        bufferevent_free(bev);
    }
}

int Server::MoveData(struct bufferevent *src, struct bufferevent *des) {
    struct evbuffer *input = bufferevent_get_input(src);
    struct evbuffer *output = bufferevent_get_output(des);
    if (input == NULL || output == NULL) {
        log_error("read data error.");
        return -1;
    }
    size_t len = evbuffer_get_length(input);
    int ret = evbuffer_remove_buffer(input, output, len);
    log_info("proxy data. bytes:%d, %d", len, ret);
    return ret;
}
