/**
 * @file
 * @brief
 */

#include "protocol.hpp"
#include "net.hpp"
#include "common/common.hpp"

Telemetry::Telemetry(uint8_t *buf, size_t size) {
    data = new Array<uint8_t>(buf, size);
}

Telemetry::~Telemetry() {
    delete data;
}

Telecommand::Telecommand(size_t size) {
    data = new Array<uint8_t>(size);
}

Telecommand::Telecommand(uint8_t cmd, uint16_t id, size_t size) {
    this->cmd = cmd;
    this->id = id;
    data = new Array<uint8_t>(size);
}

Telecommand::~Telecommand() {
    delete data;
}

SpaceSocket::SpaceSocket(Commdev *dev) {
    this->dev = dev;
}

SpaceSocket::~SpaceSocket() {
}

int SpaceSocket::send(Telecommand &cmd, Telemetry &tlm, bool last, bool first,
                      uint16_t seq) {
    packet_t pkt = Packet::create(cmd.cmd, last, first, seq, tlm.data->size,
                                  cmd.id, tlm.data->buf);
    logger_debug("socket.%s: cmd=%x last=%u first=%u seq=%u size=%u id=%u\n",
                 __func__, pkt.command, pkt.last, pkt.first, pkt.seq, pkt.size,
                 pkt.id);
    dev->downlink(pkt);
    return 0;
}

int SpaceSocket::send(Telecommand &cmd, Telemetry &tlm) {
    return send(cmd, tlm, true, true, 0);
}

int SpaceSocket::receive(Telecommand &cmd) {
    packet_t pkt = Packet::create_empty();
    pkt.payload = cmd.data->buf;

    int ret = dev->receive_uplink(pkt);

    if (ret != 0) {
        return ret;
    }

    cmd.cmd = pkt.command;
    cmd.id = pkt.id;

    return 0;
}

int SpaceSocket::update(void) {
    dev->update_status();
    return 0;
}

size_t SpaceSocket::maxsize(void) {
    return dev->mtu - Packet::headlen > 0 ? dev->mtu - Packet::headlen : 0;
}
