#ifndef __HOST_HPP__
#define __HOST_HPP__

#include <Arduino.h>
#include <stdint.h>
#include "../common/common.hpp"
#include "host_common.hpp"
#include "queue.hpp"

class Host {
public:
    Host(bridge_t *bridge);
    ~Host();

    void init(void);
    void tick(void);
    void run(void);

private:
    static const uint8_t QUEUE_DEPTH = 5;

    HardwareSerial *serial;

    bridge_t *bridge;
    comm_op_t op_in, op_out;
    OpQueue * queue;

    void          process_command(comm_op_t &op_in, comm_op_t &op_out);
    void          transact(void);
    void          handshake(void);
    void          send(comm_op_t &op);
    comm_result_t receive(comm_op_t &op);

    void cmd_test(comm_op_t &op_in, comm_op_t &op_out);
    void cmd_get_gnd_data(comm_op_t &op_in, comm_op_t &op_out);
    void cmd_get_status(comm_op_t &op_in, comm_op_t &op_out);
    void cmd_downlink(comm_op_t &op_in, comm_op_t &op_out);
    void cmd_beacon(comm_op_t &op_in, comm_op_t &op_out);

    void flush_serial(void);
};

#endif // ifndef __HOST_HPP__
