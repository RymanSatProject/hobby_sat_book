#if !defined(__COBS_HPP__)
#define __COBS_HPP__

/**
 * @file cobs.hpp
 * @brief Consistent Overhead Byte Stuffing (COBS) implementation
 */

#include <cstdlib>
#include <cstdint>

/**
 * @brief Accumulates COBS1 data, searches valid frame, and restores data
 */
class COBS1Transceiver {
public:
    COBS1Transceiver();
    ~COBS1Transceiver();

    void construct(uint8_t *rx, uint16_t rx_maxlen, uint8_t *pkt,
                   uint16_t pkt_maxlen);

    void receive(uint8_t value);
    void parse(void);
    int  pop(uint8_t *buf, uint16_t *buflen, uint16_t buf_maxlen);
    bool has_packet(void);

    static int encode(uint8_t in[], uint16_t inlen, uint8_t out[],
                       uint16_t *outlen);
    static int decode(uint8_t in[], uint16_t inlen, uint8_t out[],
                       uint16_t *outlen);

private:
    enum class State {
        GROUND,     // no valid data obtained
        LEADER,     // got 0x00 on the head
        PAYLOAD,    // getting payload bytes
        RECOGNIZED, // got 0x00 on the tail i.e. valid frame recognized
    };

    void transition(uint8_t value);
    void reset(void);
    void transition_to(State next_state);

    void try_decode(void);

    State state;

    uint8_t *rx;         // raw data
    uint16_t rxpos;      // byte position to parse next
    uint16_t rxlen;      // number of bytes received
    uint16_t rx_maxlen;  // = sizeof(rx)
    uint8_t *pkt;        // decoded data
    uint16_t pktlen;     // size of decoded data
    uint16_t pkt_maxlen; // = sizeof(pkt)

    bool has_packet_;
};

#endif // __COBS_HPP__
