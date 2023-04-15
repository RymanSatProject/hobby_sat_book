/**
 * @file
 * @brief
 */

#include "commdev.hpp"
#include "common/common.hpp"

uint8_t _buf[COMM_BUF_SIZE];

void logger_debug_op(comm_op_t &op, char io) {
    const char *inout = (io == 'i') ? "in" : "out";
    uint16_t    size = (io == 'i') ? op.data.size_in : op.data.size_out;

    logger_debug("%s: op_%s: command = 0x%X size_%s = %d\n", __func__, inout,
                 op.command, inout, size);

    logger_debug("%s: op_%s: buf:\n", __func__, inout);

    logger_hexdump(VERB_DEBUG, op.data.buf, size);
}

Commdev::Commdev(RawSerial *_serial, PinName _pinname) {
    timer = new Timer;
    pin = new DigitalOut(_pinname);
    serial = _serial;

    recvnum = 0;
    buf = _buf;

    init();
}

Commdev::~Commdev() {
    delete timer;
    delete pin;
    delete serial;
}

int Commdev::init() {
    *pin = 0;
    return 0;
}

void Commdev::flush(void) {
    uint16_t cnt = 0;

    while (serial->readable() && cnt < 1000) {
        serial->getc();
        cnt++;
    }
}

comm_result_t Commdev::transact(comm_op_t &op) {
    flush();

    *pin = 1;

    comm_result_t ret = handshake();
    if (ret != COMM_SUCCESS) {
        *pin = 0;
        goto exit;
    }

    *pin = 0;

    send(op);
    ret = receive(op);

exit:
    if (ret != COMM_SUCCESS) {
        logger_error("%s: failed: cmd=0x%02X ret=%u\n", __func__, op.command,
                     ret);
    }

    return ret;
}

comm_result_t Commdev::handshake(void) {
    comm_result_t ret = COMM_SUCCESS;
    timer->start();

    uint8_t ack[2] = {0, 0};
    uint8_t i = 0;

    while (true) {
        if (serial->readable()) {
            ack[i] = serial->getc();
            i++;
        }

        if (i == 2) {
            break;
        }

        if (timer->read_ms() > COMM_TIMEOUT_MS) {
            logger_error("comm.%s: timeout\n", __func__);
            ret = COMM_FAILURE_HANDSHAKE;
            goto exit;
        }
    }

    if (!(ack[0] == 'o' && ack[1] == 'k')) {
        logger_error("comm.%s: Invalid ack: 0x%02X%02X\n", __func__, ack[0],
                     ack[1]);
        ret = COMM_FAILURE_HANDSHAKE;
    }

exit:
    timer->stop();
    timer->reset();
    return ret;
}

void Commdev::send(comm_op_t &op) {
    serial->putc(op.command);
    serial->putc((op.data.size_out >> 8) & 0xFF);
    serial->putc(op.data.size_out & 0xFF);

    for (int16_t i = 0; i < op.data.size_out; i++) {
        serial->putc(op.data.buf[i]);
    }
}

comm_result_t Commdev::receive(comm_op_t &op) {
    uint16_t i = 0;
    uint8_t  headbuf[3];
    uint8_t  cmd_recv;
    uint16_t size_in;

    timer->start();

    while (i < 3) {
        if (serial->readable()) {
            headbuf[i] = serial->getc();
            i++;
        }

        if (timer->read_ms() > COMM_TIMEOUT_MS) {
            goto exit_timeout;
        }
    }

    timer->stop();
    timer->reset();

    cmd_recv = headbuf[0];

    if (cmd_recv != op.command) {
        logger_error(
            "comm.%s: Command inconsistent: got: 0x%02X, exp: 0x%02X\n",
            __func__, cmd_recv, op.command);
        return COMM_FAILURE_RECV;
    }

    size_in = to_u16(headbuf[1], headbuf[2]);

    if (size_in > op.data.size_in) {
        logger_warning("comm.%s: size_in too large: %u\n", __func__, size_in);
        size_in = op.data.size_in;
    }

    i = 0;

    while (i < size_in) {
        if (serial->readable()) {
            op.data.buf[i++] = serial->getc();
        }

        if (timer->read_ms() > COMM_TIMEOUT_MS) {
            goto exit_timeout;
        }
    }

    op.data.size_in = size_in;

    timer->stop();
    timer->reset();

    return COMM_SUCCESS;

exit_timeout:
    logger_error("comm.%s: TIMEOUT\n", __func__);
    timer->stop();
    timer->reset();

    return COMM_FAILURE_RECV_TIMEOUT;
}
