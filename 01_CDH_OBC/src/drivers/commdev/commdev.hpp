/**
 * @file
 * @brief
 */

#ifndef __COMMDEV_HPP__
#define __COMMDEV_HPP__

#include "mbed.h"
#include "drivers/device/device.hpp"
#include "net/protocol.hpp"

constexpr uint16_t COMM_BUF_SIZE = 512;
constexpr uint16_t COMM_TIMEOUT_MS = 1000;

typedef enum _comm_result {
    COMM_SUCCESS = 0,
    COMM_FAILURE_HANDSHAKE = 1,
    COMM_FAILURE_RECV = 3,
    COMM_FAILURE_RECV_TIMEOUT = 4,
} comm_result_t;

typedef enum _comm_cmd {
    COMM_CMD_TEST = 0x01,
    COMM_CMD_GET_STATUS = 0x10,
    COMM_CMD_GET_GND_DATA = 0x20,
    COMM_CMD_DOWNLINK_BEACON = 0x30,
    COMM_CMD_DOWNLINK_DATA = 0x31,
    COMM_CMD_RESET = 0x40,
} comm_cmd_t;

typedef struct _comm_op {
    uint8_t command;
    struct {
        uint8_t *buf;
        uint16_t size_out;
        uint16_t size_in;
    } data;
} comm_op_t;

class Commdev: public Device {
public:
    uint8_t  rssi;    /** Reception intensity */
    uint16_t recvnum; /** Number of bytes received */
    int      temp;

    Commdev(RawSerial *_serial, PinName _pinname);
    ~Commdev();

    int init(void);
    void update_status(void);
    uint8_t reset(void);

    int receive_uplink(packet_t &packet_in);
    int downlink(packet_t &packet_out);

    void beacon(packet_t &packet_out);

    static const uint16_t mtu = 254;

private:
    Timer *     timer;
    RawSerial * serial;
    DigitalOut *pin;

    uint8_t *buf;

    comm_result_t transact(comm_op_t &op);
    comm_result_t handshake(void);
    void          send(comm_op_t &op);
    comm_result_t receive(comm_op_t &op);

    void flush(void);
};

#endif // __COMMDEV_HPP__
