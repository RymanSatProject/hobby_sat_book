#ifndef __QUEUE_HPP__
#define __QUEUE_HPP__

#include <Arduino.h>
#include <stdint.h>
#include "../common/common.hpp"
#include "host_common.hpp"

class OpQueue {
public:
    OpQueue(uint32_t size);
    ~OpQueue();

    int push(data_t& bdata);
    int pop(comm_op_t& op);

    uint16_t next_size(void);

    uint32_t head, tail, num;
    uint32_t size;

private:
    comm_op_t* queue;
};

#endif // __QUEUE_HPP__
