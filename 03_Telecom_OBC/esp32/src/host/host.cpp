#include "host.hpp"

#include <string.h>

Host::Host(bridge_t *bridge) {
    this->serial = new HardwareSerial(2);
    this->bridge = bridge;

    op_in.command = 0;
    op_in.data.size = 0;
    op_in.data.buf = new uint8_t[COMM_BUF_SIZE];

    op_out.command = 0;
    op_out.data.size = 0;
    op_out.data.buf = new uint8_t[COMM_BUF_SIZE];

    queue = new OpQueue(QUEUE_DEPTH);
}

Host::~Host() {
    delete queue;
    delete[] op_in.data.buf;
    delete[] op_out.data.buf;
}

volatile static bool is_transaction_requested = false;

static void irq_host_start() {
    is_transaction_requested = true;
}

void Host::init(void) {
    logger_debug("host start\n");

    pinMode(PIN_HOST_IRQ, INPUT);

    serial->begin(BAUD_HOST);
    attachInterrupt(PIN_HOST_IRQ, irq_host_start, RISING);
}

void Host::tick(void) {
    if (is_transaction_requested) {
        transact();
    }

    if (bridge->up.size != 0) {
        bridge->up.lock->lock();
        queue->push(bridge->up);
        bridge->up.lock->unlock();
    }
}

void Host::run(void) {
    init();
    while (true) {
        tick();
    }
}

void Host::transact(void) {
    flush_serial();

    handshake();

    if (receive(op_in) != COMM_SUCCESS) {
        is_transaction_requested = false;
        return;
    }

    process_command(op_in, op_out);
    is_transaction_requested = false;
    send(op_out);
}

void Host::flush_serial(void) {
    uint16_t cnt = 0;

    while (serial->available() && cnt < 1000) {
        serial->read();
        cnt++;
    }
}
