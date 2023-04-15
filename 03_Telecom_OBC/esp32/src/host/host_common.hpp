#ifndef __HOST_COMMON_HPP__
#define __HOST_COMMON_HPP__

#define COMM_BUF_SIZE   512
#define COMM_TIMEOUT_MS 500

typedef enum _comm_cmd {
    COMM_CMD_GET_STATUS = 0x10,
    COMM_CMD_GET_GND_DATA = 0x20,
    COMM_CMD_DOWNLINK_BEACON = 0x30,
    COMM_CMD_DOWNLINK_DATA = 0x31,
    COMM_CMD_RESET = 0x40,
} comm_cmd_t;

typedef enum _comm_result {
    COMM_SUCCESS = 0,
    COMM_FAILURE_HANDSHAKE = 1,
    COMM_FAILURE_RECV = 3,
    COMM_FAILURE_RECV_TIMEOUT = 4,
} comm_result_t;

typedef struct comm_op_s {
    uint8_t command;
    struct {
        uint8_t *buf;
        uint16_t size;
    } data;
} comm_op_t;

#endif // __HOST_COMMON_HPP__
