#ifndef _RF_HPP_
#define _RF_HPP_

#include "BluetoothSerial.h"
#include <Arduino.h>
#include <stdint.h>
#include "../common/common.hpp"
#include "../lib/cobs/cobs.hpp"
#include "cw.hpp"

#define RF_RAWDATA_SIZE 512

class RF {
public:
    RF(bridge_t *bridge);
    ~RF();

    void init(void);
    void tick(void);

private:
#ifdef RF_UART
    HardwareSerial *serial;
#else
    BluetoothSerial *serial;
#endif

    CW               *cw;
    COBS1Transceiver *trx;
    bridge_t         *bridge;

    uint8_t *raw_down;
    uint16_t raw_down_size;

    void receive_uplink(void);
    void downlink(void);

    int      baud;
    rfprot_t prot;
};

#endif // _RF_HPP_
