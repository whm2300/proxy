#ifndef PACKET_H_
#define PACKET_H_

#include "util/define.h"

#include <google/protobuf/message.h>
#include <event2/buffer.h>

//包格式说明。包头8字节，前两个字节固定为0xFEFF，中间两字节为命令。
//后四字节为包体长度。

const uint8_t PACKET_HEAD_LEN = 8;

class Packet {
    public:
        Packet();
        ~Packet();
        DISALLOW_COPY_AND_ASSIGN(Packet);

    public:
        static Packet *GetInstance();
        bool MakePacket(::google::protobuf::Message *msg, uint16_t cmd);
        bool ParsePacket(struct evbuffer *input);

        //返回发送的包数据
        FORCEINLINE uint8_t *get_send_data() { return _data; }
        FORCEINLINE uint32_t get_send_len() { return _data_len; }

        //返回解包的包体数据
        FORCEINLINE uint8_t *get_msg_data() { return _data + PACKET_HEAD_LEN; }
        FORCEINLINE uint32_t get_msg_len() {return _data_len - PACKET_HEAD_LEN;}
        FORCEINLINE uint16_t get_cmd() {
            if (_data_len >= 4) {
                return ntohs(*((uint16_t*)(_data + 2)));
            }
            return 0;
        }

    protected:
        bool CheckDataMem(uint32_t len);
        void ReleaseDataMen();

    private:
        uint8_t *_data;
        uint32_t _alloc_len;
        uint32_t _data_len;
};

#endif // PACKET_H_
