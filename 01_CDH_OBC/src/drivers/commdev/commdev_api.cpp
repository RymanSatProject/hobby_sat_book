/**
 * @file
 * @brief
 */

#include "commdev.hpp"
#include "common/common.hpp"

void logger_debug_payload(const char func[], packet_t &packet) {
    if (logger_verbosity() >= VERB_DEBUG) {
        logger_debug("[comm.%s] func = %s, command = 0x%X size = %d\n",
                     __func__, func, packet.command, packet.size);
        logger_debug("[comm.%s] payload:\n", __func__);

        logger_hexdump(VERB_DEBUG, packet.payload, packet.size);
    }
}

void Commdev::update_status(void) {
    comm_op_t op = {
        .command = COMM_CMD_GET_STATUS,
        .data = {.buf = buf, .size_out = 0, .size_in = COMM_BUF_SIZE}};

    if (transact(op) != COMM_SUCCESS) {
        recvnum = 0;
        return;
    }

    recvnum = to_u16(buf[0], buf[1]);
    temp = static_cast<int>(to_u32(buf[2], buf[3], buf[4], buf[5]));
}

int Commdev::receive_uplink(packet_t &packet_in) {
    comm_op_t op = {
        .command = COMM_CMD_GET_GND_DATA,
        .data = {.buf = buf, .size_out = 0, .size_in = COMM_BUF_SIZE}};

    if (transact(op) != COMM_SUCCESS) {
        return -ETIMEDOUT;
    }

    return Packet::decode(packet_in, buf, COMM_BUF_SIZE);
}

int Commdev::downlink(packet_t &packet_out) {
    logger_debug_payload(__func__, packet_out);

    uint16_t size;

    int ret = Packet::encode(packet_out, buf, size);
    if (ret != 0) {
        return ret;
    }

    comm_op_t op = {COMM_CMD_DOWNLINK_DATA, {buf, size, 0}};

    transact(op);
    wait_us(1000 * 100); // to not make TOBC too busy

    return 0;
}

void Commdev::beacon(packet_t &packet_out) {
    logger_debug_payload(__func__, packet_out);

    comm_op_t op = {.command = COMM_CMD_DOWNLINK_BEACON,
                    .data = {.buf = packet_out.payload,
                             .size_out = packet_out.size,
                             .size_in = 0}};
    transact(op);
}

uint8_t Commdev::reset() {
    logger_debug("[comm.%s] Reset\n", __func__);

    comm_op_t op = {.command = COMM_CMD_RESET,
                    .data = {.buf = buf, .size_out = 0, .size_in = 0}};
    return transact(op);
}
