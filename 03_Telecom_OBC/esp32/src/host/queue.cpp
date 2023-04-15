#include "queue.hpp"

OpQueue::OpQueue(uint32_t size) {
    this->size = size;
    this->queue = new comm_op_t[size];
    this->head = 0;
    this->tail = size - 1;
    this->num = 0;

    for (uint32_t i = 0; i < size; i++) {
        this->queue[i].data.buf = new uint8_t[COMM_BUF_SIZE];
    }
}

OpQueue::~OpQueue() {
    for (uint32_t i = 0; i < size; i++) {
        delete[] queue[i].data.buf;
    }

    delete[] queue;
}

int OpQueue::push(data_t& bdata) {
    if (num >= size) {
        return -1;
    }

    tail = (tail + 1) % size;
    memcpy(queue[tail].data.buf, bdata.buf, bdata.size);
    queue[tail].data.size = bdata.size;
    bdata.size = 0;

    num++;

    logger_debug("queue.%s: tail = %u\n", __func__, tail);

    return 0;
}

uint16_t OpQueue::next_size(void) {
    return (num == 0) ? 0 : queue[head].data.size;
}

int OpQueue::pop(comm_op_t& op) {
    if (num == 0) {
        logger_error("Queue empty\n");
        return -1;
    }

    memcpy(op.data.buf, queue[head].data.buf, queue[head].data.size);
    op.data.size = queue[head].data.size;
    head = (head + 1) % size;
    num--;

    logger_trace("queue.%s: head = %u\n", __func__, head);

    return 0;
}
