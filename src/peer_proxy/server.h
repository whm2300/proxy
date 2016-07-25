#ifndef SERVER_H_
#define SERVER_H_

#include <map>

#include <event2/event.h>
#include <event2/bufferevent.h>

#include "protocol/packet.h"

typedef std::map<bufferevent *, bufferevent *> BuffereventMap;
typedef std::map<uint32_t, bufferevent *> IntToBufferevent;

class Server {
    public:
        Server();
        ~Server();

        bool InitServer();
        bool RunServer();

    protected:
        bool StartListen(const char *ip, uint16_t port);
        bool InitSignalAndTimer();
        bool SendNewConnectionCmd(bufferevent *bev, uint32_t token);
        FORCEINLINE int GetToken() { return ++_token; }
        void ProcessMsg(struct bufferevent *bev);
        void ProcessLocalRegister(struct bufferevent *bev);
        void ProcessLocalProxy(struct bufferevent *bev);
        FORCEINLINE int MoveData(struct bufferevent *src, struct bufferevent *des);

        static void AcceptCallback(evutil_socket_t fd, short events, 
                    void *user_data);
        static void SignalCallback(evutil_socket_t sig, short events, 
                    void *user_data);
        static void ManageSocketReadCallback(struct bufferevent *bev, 
                    void *user_data);
        static void ManageSocketEventCallback(struct bufferevent *bev, 
                    short events, void *user_data);
        static void PeerSocketReadCallback(struct bufferevent *bev, 
                    void *user_data);
        static void PeerSocketEventCallback(struct bufferevent *bev, 
                    short events, void *user_data);

    private:
        struct event_base   *_evbase;
        struct bufferevent  *_manage_bev;
        uint32_t            _token;
        Packet              _packet;
        BuffereventMap      _peer_to_local;  //peer->local
        BuffereventMap      _local_to_peer;
        IntToBufferevent    _token_to_bufferevent;

};

#endif  // SERVER_H_
