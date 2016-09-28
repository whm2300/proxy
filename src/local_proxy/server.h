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
        bool ConnectToPeer(const char *ip, uint16_t port);
        int ConnectBlock(const char *ip, uint16_t port);
        bool InitSignalAndTimer();
        void ProcessMsg(struct bufferevent *bev);
        void ProcessNewProxy(struct bufferevent *bev);
        FORCEINLINE int MoveData(struct bufferevent *src, struct bufferevent *des);

        static void SignalCallback(evutil_socket_t sig, short events, 
                    void *user_data);
        static void ManageSocketReadCallback(struct bufferevent *bev, 
                    void *user_data);
        static void ManageSocketEventCallback(struct bufferevent *bev, 
                    short events, void *user_data);
        static void RealSocketReadCallback(struct bufferevent *bev, 
                    void *user_data);
        static void RealSocketEventCallback(struct bufferevent *bev, 
                    short events, void *user_data);
        static void SendHeartBeat(evutil_socket_t sig, short events, 
                    void *user_data);

    private:
        struct event_base   *_evbase;
        struct bufferevent  *_manage_bev;
        Packet              _packet;
        BuffereventMap      _real_to_local;  //mysql->local
        BuffereventMap      _local_to_real;
};

#endif  // SERVER_H_
