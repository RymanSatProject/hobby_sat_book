#include "host.hpp"
#include "../lib/bytes.hpp"

#include <string.h>

void logger_debug_op(comm_op_t &op, char io) {
#if VERBOSITY >= VERB_DEBUG
    const char *inout = (io == 'i') ? "in" : "out";
    uint16_t    size = op.data.size;

    logger_debug("[%s] %s: cmd: 0x%X size_%s: %d buf: ", __func__, inout,
                 op.command, inout, size);

    logger_hexdump(VERB_DEBUG, op.data.buf, size);
#endif
}

void Host::handshake(void) {
    logger_trace("host.%s\n", __func__);
    serial->write('o');
    serial->write('k');
}

/**
 * @brief Receive command from main OBC
 *
 * @param op: command received from main OBC
 * @return comm_result_t
 */
comm_result_t Host::receive(comm_op_t &op) {
    uint16_t i = 0;
    uint8_t  headbuf[3] = {};

    uint32_t start_ms = millis();

    while (i < 3) {
        if (serial->available()) {
            headbuf[i] = serial->read();
            i++;
        }

        if (millis() - start_ms > COMM_TIMEOUT_MS) {
            logger_error("host.%s: early timeout: cmd=0x%X i=%u "
                         "headbuf={0x%X 0x%X 0x%X}\n",
                         __func__, op.command, i, headbuf[0], headbuf[1],
                         headbuf[2]);
            goto exit_timeout;
        }
    }

    op.command = headbuf[0];
    op.data.size = to_u16(headbuf[1], headbuf[2]);

    if (op.data.size > COMM_BUF_SIZE) {
        logger_warning("comm.%s: size too large: %u\n", __func__, op.data.size);
        op.data.size = COMM_BUF_SIZE;
    }

    i = 0;

    while (i < op.data.size) {
        if (serial->available()) {
            op.data.buf[i++] = serial->read();
        }

        if (millis() - start_ms > COMM_TIMEOUT_MS) {
            logger_error("host.%s: TIMEOUT: cmd=0x%X i=%u exp=%u\n", __func__,
                         op.command, i, op.data.size);
            goto exit_timeout;
        }
    }

    logger_debug_op(op, 'i');

    return COMM_SUCCESS;

exit_timeout:
    return COMM_FAILURE_RECV_TIMEOUT;
}

/**
 * @brief Send response back to main OBC
 *
 * @param op: command & payload to send to main OBC
 */
void Host::send(comm_op_t &op) {
    serial->write(op.command);                 // headbuf[0]
    serial->write((op.data.size >> 8) & 0xFF); // headbuf[1]
    serial->write(op.data.size & 0xFF);        // headbuf[2]

    for (int16_t i = 0; i < op.data.size; i++) {
        serial->write(op.data.buf[i]);
    }

    logger_debug_op(op, 'o');
}
