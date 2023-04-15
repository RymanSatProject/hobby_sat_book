/**
 * @file Network APIs
 * @brief
 */

#ifndef __NET_HPP__
#define __NET_HPP__

#include "mbed.h"
#include "maindev/maindev.hpp"

class Telemetry {
public:
    Telemetry(uint8_t *buf, size_t size);
    ~Telemetry();

    Array<uint8_t> *data;
};

class Telecommand {
public:
    Telecommand(size_t size);
    Telecommand(uint8_t cmd, uint16_t id, size_t size);
    ~Telecommand();

    uint8_t         cmd;
    uint16_t        id;
    Array<uint8_t> *data;
};

class SpaceSocket {
public:
    SpaceSocket(Commdev *dev);
    ~SpaceSocket();

    /**
     * @brief Downlinks a single packet with user-defined SPP parameters
     */
    int send(Telecommand &cmd, Telemetry &tlm, bool last, bool first,
             uint16_t seq);

    /**
     * @brief Downlinks a single packet
     */
    int send(Telecommand &cmd, Telemetry &tlm);
    int send(Telecommand &cmd, packet_t &pkt);

    /**
     * @brief Receives a Telecommand
     */
    int receive(Telecommand &cmd);

    /**
     * @brief Update status of the network device
     */
    int update(void);

    /**
     * @brief Maximum size of data buffer per segment
     */
    size_t maxsize(void);

private:
    Commdev *dev;
};

#endif // __NET_HPP__
