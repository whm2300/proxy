#include "packet.h"

#include <arpa/inet.h>
#include <assert.h>
#include <string.h>

#include "util/log.h"
#include "protocol/command.h"

Packet::Packet():_data(NULL), _alloc_len(0), _data_len(0) {
}

Packet::~Packet() {
    ReleaseDataMen();
}

bool Packet::MakePacket(::google::protobuf::Message *msg, uint16_t cmd) {
    int buffer_size = 0;
    if (msg != NULL) {
        buffer_size += msg->ByteSize();
    }
    if (!CheckDataMem(buffer_size)) {
        return false;
    }
    _data[0] = 0xFE;
    _data[1] = 0xFF;
    cmd = htons(cmd);
    memcpy(_data+2, &cmd, sizeof(cmd));
    _data_len = PACKET_HEAD_LEN - 4;
    if (msg != NULL) {
        uint32_t *len = (uint32_t*)(_data + 4);
        *len = htonl(msg->ByteSize());
        msg->SerializeToArray(_data + PACKET_HEAD_LEN, msg->ByteSize());
        _data_len = PACKET_HEAD_LEN + msg->ByteSize();
    }
    return true;
}

bool Packet::ParsePacket(struct evbuffer *input) {
    assert(input != NULL);
    //心跳包没有长度字段。
    uint8_t *packet_data = evbuffer_pullup(input, PACKET_HEAD_LEN - 4);
    if (packet_data == NULL) {
        return false;
    }
    if (packet_data[0] != 0xFE || packet_data[1] != 0xFF) {
        log_error("packet head error.0x%d, 0x%d", 
                    packet_data[0], packet_data[1]);
        return false;
    }
    uint32_t msg_len = 0;

    uint16_t cmd = ntohs(*((uint16_t*)(packet_data + 2)));
    log_info("cmd:0x%x", cmd);
    _data_len = PACKET_HEAD_LEN - 4;
    if (cmd != HEART_BEAT && cmd != LOCAL_REGISTER) {
        packet_data = evbuffer_pullup(input, PACKET_HEAD_LEN);
        if (packet_data == NULL) {
            log_error("parse packet error");
            return false;
        }
        msg_len = ntohl(*((int*)(packet_data + 4)));
        _data_len = PACKET_HEAD_LEN + msg_len;
    }
    if (!CheckDataMem(msg_len)) {
        return false;
    }
    packet_data = evbuffer_pullup(input, _data_len);
    memcpy(_data, packet_data, _data_len);
    evbuffer_drain(input, _data_len);
    return true;
}

bool Packet::CheckDataMem(uint32_t len) {
    uint32_t total_len = len + PACKET_HEAD_LEN;
    if (total_len < _alloc_len) {
        return true;
    }

    _alloc_len = (total_len/1024 + 1)*1024;
    log_debug("packet alloc len:%d", _alloc_len);

    if (_data != NULL) {
        delete []_data;
        _data = NULL;
    }
    _data = new(std::nothrow) uint8_t[_alloc_len];
    if (_data == NULL) {
        log_error("alloc packet data memory fail");
        return false;
    }
    return true;
}

void Packet::ReleaseDataMen() {
    if (_data != NULL) {
        delete []_data;
        _data_len = 0;
    }
}
