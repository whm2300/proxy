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

Server::Server():_evbase(NULL), _manage_bev(NULL), _token(0) {
}

Server::~Server() {
}
bool Server::StartListen(const char *ip, uint16_t port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listen_fd == -1){
        log_error("get listen socket error. %s", strerror(errno));
        return false;
    }
    int optval = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
        log_error("set socket opt fail. %s", strerror(errno));
        return false;
    }
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0){
        log_error("bind addr error. %s", strerror(errno));
        return false;
    }
    if (listen(listen_fd, 5) != 0){
        log_error("listen error. %s", strerror(errno));
        return false;
    }

    struct event *listen_event = event_new(_evbase, listen_fd, EV_READ | EV_PERSIST, AcceptCallback, this);
    event_add(listen_event, NULL);
    log_info("start listen, listen at:%s:%d", ip, port);
    return true;
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
    if (!InitSignalAndTimer()){
        return false;
    }

    if (!StartListen(CONFIG->get_ip().c_str(), CONFIG->get_port())) {
        log_error("listen manage port error");
        return false;
    }

    for (size_t i = 0; i < CONFIG->get_port_pair().size(); ++i) {
        if (!StartListen(CONFIG->get_ip().c_str(), CONFIG->get_port_pair()[i].peer_port)) {
            log_error("listen error. port:%d", 
                        CONFIG->get_port_pair()[i].peer_port);
            return false;
        }
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
    /*
    struct timer_event = event_new(_evbase, -1, EV_PERSIST, del_expire_client, this);
    {
        struct timeval tv;
        tv.tv_sec = _config_info.heartbeat_time*2;
        tv.tv_usec = 0;
        if (_timer_event == NULL || evtimer_add(_timer_event, &tv) < 0){
            log_error("%s", "could not create/add a timer event");
            return false;
        }
    }
    */
    return true;
}

void Server::SignalCallback(evutil_socket_t sig, short events, 
                            void *user_data) {
    exit(0);
}

void Server::AcceptCallback(evutil_socket_t fd, short events, void *user_data) {
    int conn_fd = accept4(fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
    log_debug("new fd:%d", conn_fd);
    if (conn_fd < 0) {
        log_error("accept4 error. %s", strerror(errno));
        return ;
    }

    Server *server = (Server *)user_data;
    struct bufferevent *bev = bufferevent_socket_new(server->_evbase, 
                conn_fd, BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL) {
        log_error("bufferevent_socket_new error. %s", strerror(errno));
        return ;
    }

    struct sockaddr addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    if (getsockname(fd, &addr, &len) != 0) {
        log_error("getsockname error. fd:%d", fd);
        return ;
    }
    struct sockaddr_in *sin_addr = (struct sockaddr_in*)&addr;
    uint16_t listen_port = ntohs(sin_addr->sin_port);
    log_debug("manage port:%d, config:%d", listen_port, CONFIG->get_port());
    if (listen_port == CONFIG->get_port()) {  //管理连接和代理连接
        bufferevent_setcb(bev, ManageSocketReadCallback, NULL, 
                    ManageSocketEventCallback, user_data);
    } else {  //外部新连接
        bufferevent_setcb(bev, PeerSocketReadCallback, NULL, 
                    PeerSocketEventCallback, user_data);

        //建立代理
        uint32_t token = server->GetToken();
        if (server->_token_to_bufferevent.find(token) != 
                    server->_token_to_bufferevent.end()) {
            log_fatal("token exist. %d", token);
            abort();
        }
        server->_token_to_bufferevent.insert(std::pair<int, bufferevent*>(token, bev));
        bufferevent_disable(bev, EV_READ);
        if (!server->SendNewConnectionCmd(bev, token)) {
            bufferevent_free(bev);
            return ;
        }
        log_debug("send new connection. token:%d", token);
    }
    bufferevent_enable(bev, EV_READ | EV_WRITE);
    return ;
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

void Server::ManageSocketReadCallback(struct bufferevent *bev, 
            void *user_data) {
    Server *server = (Server *)user_data;
    assert(server != NULL);

    struct evbuffer *input = bufferevent_get_input(bev);
    if (input == NULL) {
        return ;
    }
    do {
        BuffereventMap::iterator map_it = server->_local_to_peer.find(bev);
        if (map_it != server->_local_to_peer.end()) {
            //内网代理连接已经配对，直接转发
            server->MoveData(map_it->first, map_it->second);
            server->MoveData(map_it->second, map_it->first);
            break ;
        }

        if (!server->_packet.ParsePacket(input)) {
            break ;
        }
        server->ProcessMsg(bev);
    } while (evbuffer_get_length(input) > 0);
}

void Server::ProcessMsg(struct bufferevent *bev) {
    assert(bev != NULL);
    uint16_t cmd = _packet.get_cmd();
    switch (cmd) {
        case HEART_BEAT:
            log_debug("heart beat");
            break;
        case LOCAL_REGISTER:
            ProcessLocalRegister(bev);
            log_info("local register");
            break;
        case LOCAL_PROXY:
            ProcessLocalProxy(bev);
            log_info("local proxy");
            break;
        default :
            log_error("error cmd:0x%x", cmd);
            break;
    }
}

void Server::ProcessLocalRegister(struct bufferevent *bev) {
    assert(bev != NULL);
    if (bev == _manage_bev) {
        log_error("already regiter:%p", bev);
        return ;
    }

    if (_manage_bev != NULL) {
        bufferevent_free(bev);
        log_error("_manage_bev already register");
    }
    _manage_bev = bev;
    log_info("local register:%p", _manage_bev);
}

void Server::ProcessLocalProxy(struct bufferevent *bev) {
    NewConnection msg;
    msg.ParseFromArray(_packet.get_msg_data(), _packet.get_msg_len());
    IntToBufferevent::iterator it = _token_to_bufferevent.find(msg.token());
    if (it == _token_to_bufferevent.end()) {
        bufferevent_free(bev);
        log_error("can not find token:%d", msg.token());
        return ;
    }
    _local_to_peer[bev] = it->second;
    _peer_to_local[it->second] = bev;
    _token_to_bufferevent.erase(it);

    MoveData(it->second, bev);
    MoveData(bev, it->second);

    bufferevent_enable(bev, EV_READ | EV_WRITE);
    bufferevent_enable(it->second, EV_READ | EV_WRITE);
}

void Server::ManageSocketEventCallback(struct bufferevent *bev, 
            short events, void *user_data) {
    Server *server = (Server *)user_data;
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        log_info("close local socket, close:%p, manage:%p", bev, server->_manage_bev);
        if (bev == server->_manage_bev) {
            server->_manage_bev = NULL;
        }
        BuffereventMap::iterator map_it = server->_local_to_peer.find(bev);
        if (map_it != server->_local_to_peer.end()) {
            server->MoveData(bev, map_it->second);
            bufferevent_free(map_it->second);
            server->_local_to_peer.erase(map_it);
            log_info("release proxy");
        }
        bufferevent_free(bev);
    }
}

void Server::PeerSocketReadCallback(struct bufferevent *bev, void *user_data) {
    Server *server = (Server *)user_data;
    assert(server != NULL);
    BuffereventMap::iterator it_map = server->_peer_to_local.find(bev);
    if (it_map == server->_peer_to_local.end()) {  //还没有对应的转发连接
        /*
           uint32_t token = server->GetToken();
           if (server->_token_to_bufferevent.find(token) != 
           server->_token_to_bufferevent.end()) {
           log_fatal("token exist. %d", token);
           abort();
           }
           server->_token_to_bufferevent.insert(std::pair<int, bufferevent*>(token, bev));
           bufferevent_disable(bev, EV_READ);
           if (!server->SendNewConnectionCmd(bev, token)) {
           close(bufferevent_getfd(bev));
           bufferevent_free(bev);
           return ;
           }
           log_debug("send new connection. token:%d", token);
           */
        log_info("get peer data, but not establish connect");
    } else {  //转发连接已存在，直接转发。
        server->MoveData(bev, it_map->second);
    }
}

void Server::PeerSocketEventCallback(struct bufferevent *bev, short events, 
            void *user_data) {
    Server *server = (Server *)user_data;
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        log_info("close peer socket, 0x%x", events);
        BuffereventMap::iterator it = server->_peer_to_local.find(bev);
        if (it != server->_peer_to_local.end()) {
            log_debug("close lcoal:%p, _manage:%p", it->second, server->_manage_bev);
            server->MoveData(bev, it->second);
            server->_local_to_peer.erase(it->second);
            bufferevent_free(it->second);
            server->_peer_to_local.erase(it);
        }
        bufferevent_free(bev);
    }
}

bool Server::SendNewConnectionCmd(bufferevent *bev, uint32_t token) {
    log_debug("Send new connecion");
    int fd = bufferevent_getfd(bev);
    struct sockaddr addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));
    if (getsockname(fd, &addr, &len) != 0) {
        log_error("getsockname error. fd:%d", fd);
        return false;
    }
    struct sockaddr_in *sin_addr = (struct sockaddr_in*)&addr;
    uint16_t from_port = ntohs(sin_addr->sin_port);
    uint16_t to_port = 0;
    const VecPortPair &port_pair = CONFIG->get_port_pair();
    VecPortPair::const_iterator it = port_pair.begin();
    for (; it != port_pair.end(); ++it) {
        if (it->peer_port == from_port) {
            to_port = it->local_port;
            break;
        }
    }
    if (to_port == 0) {
        log_error("not find socket pair. peer port:%d", from_port);
        return false;
    }
    log_info("from port:%d, to port:%d", from_port, to_port);

    NewConnection msg;
    msg.set_token(token);
    msg.set_from_port(from_port);
    msg.set_to_prot(to_port);

    _packet.MakePacket(&msg, PEER_REGISTER);
    if (_manage_bev == NULL) {
        log_error("manage is null");
        return false;
    }
    bufferevent_write(_manage_bev, _packet.get_send_data(), 
                _packet.get_send_len());
    return true;
}

int Server::MoveData(struct bufferevent *src, struct bufferevent *des) {
    struct evbuffer *input = bufferevent_get_input(src);
    struct evbuffer *output = bufferevent_get_output(des);
    if (input == NULL || output == NULL) {
        log_error("read data error.");
        return -1;
    }
    size_t len = evbuffer_get_length(input);
    int ret = 0;
    if (len > 0) {
        ret = evbuffer_remove_buffer(input, output, len);
        //log_info("proxy data. bytes:%d, %d", len, ret);
    }
    return ret;
}
