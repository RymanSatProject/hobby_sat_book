#include "rf.hpp"

#include <string.h>

static uint8_t trx_pkt_[RF_RAWDATA_SIZE];
static uint8_t raw_up_[RF_RAWDATA_SIZE];
static uint8_t raw_down_[RF_RAWDATA_SIZE];

static COBS1Transceiver trx_;
static Buzzer           buzzer_;
static CW               cw_;

RF::RF(bridge_t *bridge) {
    this->bridge = bridge;

#ifdef RF_UART
    serial = new HardwareSerial(0);
#else
    serial = new BluetoothSerial;
#endif

    baud = BAUD_RF;
    prot = RFPROT_BT;

    raw_down = raw_down_;
    raw_down_size = 0;

    buzzer_.construct(PIN_BUZZER);

    cw = &cw_;
    cw->construct(&buzzer_);

    trx = &trx_;
    trx->construct(raw_up_, ARRAY_SIZE(raw_up_), trx_pkt_,
                   ARRAY_SIZE(trx_pkt_));
}

RF::~RF() {
    delete serial;
}

void RF::init(void) {
#ifdef RF_UART
    serial->begin(baud);
#else
    serial->begin(RF_NAME);
#endif
    logger_debug("RF start\n", __func__);
}

void RF::tick(void) {
    receive_uplink();
    downlink();
    cw->tick();
}

void RF::receive_uplink(void) {
    if (!serial->available()) {
        return;
    }

    while (serial->available()) {
        trx->receive((uint8_t)serial->read());
    }

    cw->reset();

    trx->parse();

    if (trx->has_packet()) {
        bridge->up.lock->lock();
        trx->pop(bridge->up.buf, &bridge->up.size, 256);
        bridge->up.lock->unlock();
    }
}

void RF::downlink(void) {
    if (bridge->down.size == 0) {
        return;
    }

    logger_debug("rf.%s start\n", __func__);

    bridge->down.lock->lock();
    int err = trx->encode(bridge->down.buf, bridge->down.size, raw_down,
                          &raw_down_size);

    prot = bridge->down.prot;

    bridge->down.size = 0;
    bridge->down.lock->unlock();

    if (err) {
        return;
    }

    if (prot == RFPROT_CW) {
        cw->start(raw_down, raw_down_size);
    } else {
        cw->reset();
        serial->write(raw_down, raw_down_size);
    }

    logger_debug("rf.%s: Downlinked %u bytes: ", __func__, raw_down_size);
    logger_hexdump(VERB_DEBUG, raw_down, raw_down_size);
}
