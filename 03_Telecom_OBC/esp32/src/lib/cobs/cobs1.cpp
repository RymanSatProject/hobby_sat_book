#include "cobs.hpp"
#include <cstring>
#include "../../common/common.hpp"

COBS1Transceiver::COBS1Transceiver() {
}

COBS1Transceiver::~COBS1Transceiver() {
}

void COBS1Transceiver::construct(uint8_t *rx, uint16_t rx_maxlen, uint8_t *pkt,
                                 uint16_t pkt_maxlen) {
    this->rx = rx;
    this->rxpos = 0;
    this->rxlen = 0;
    this->rx_maxlen = rx_maxlen;
    this->pkt = pkt;
    this->pktlen = 0;
    this->pkt_maxlen = pkt_maxlen;

    this->has_packet_ = false;

    this->state = State::GROUND;
}

static constexpr uint8_t  COBS1_MARKER = 0x00;
static constexpr uint8_t  COBS1_PAD = 0xff;
static constexpr uint16_t COBS1_PAYLOAD_MAXLEN = 0xff - 1;
static constexpr uint16_t COBS1_HEADERLEN = 4; // two markers + size + next-size
static constexpr uint16_t COBS1_FRAME_MAXLEN =
    COBS1_PAYLOAD_MAXLEN + COBS1_HEADERLEN;

static inline bool is_marker(uint8_t head) {
    return head == COBS1_MARKER;
}

/**
 * @brief Convert input into COBS1 frame
 */
int COBS1Transceiver::encode(uint8_t in[], uint16_t inlen, uint8_t out[],
                             uint16_t *outlen) {
#ifdef COBS_DISABLE
    memcpy(out, in, inlen);
    *outlen = inlen;

#else
    if (inlen > COBS1_PAYLOAD_MAXLEN) {
        logger_error("COBS1: %s: inlen too long: %d\n", __func__, inlen);
        return -1;
    }

    // Strategy:
    // 1. Create output buffer containing zeros
    // 2. Replace zeros using COBS1 rule

    // 1. Create output buffer containing zeros
    out[0] = COBS1_MARKER;
    out[1] = inlen & 0xff;
    memcpy(&out[3], in, inlen);
    uint16_t outlen_ = inlen + COBS1_HEADERLEN;
    out[outlen_ - 1] = COBS1_MARKER;

    int pos_zero = 2;

    // 2. Replace zeros using COBS1 rule
    for (int i = 3; i < outlen_; i++) {
        if (out[i] == COBS1_MARKER) {
            out[pos_zero] = i - pos_zero;
            pos_zero = i;
        }
    }

    *outlen = outlen_;
#endif // COBS_DISABLE
    return 0;
}

/**
 * @brief Convert COBS1 frame into original data
 * @param in: COBS1 frame i.e. in[0] = in[inlen - 1] = COBS1_MARKER
 */
int COBS1Transceiver::decode(uint8_t in[], uint16_t inlen, uint8_t out[],
                             uint16_t *outlen) {
    if (in[0] != COBS1_MARKER || in[inlen - 1] != COBS1_MARKER) {
        logger_error("COBS1: %s: invalid frame\n", __func__);
        return -1;
    }

    if (inlen < COBS1_HEADERLEN || COBS1_FRAME_MAXLEN < inlen) {
        logger_error("COBS1: %s: invalid frame length: %u\n", __func__, inlen);
        return -2;
    }

    // Create output first without converting zeros
    uint16_t outlen_ = inlen - COBS1_HEADERLEN;

    memcpy(out, &in[3], outlen_);

    uint16_t distance_zero = in[2];

    for (int i = distance_zero - 1; i < outlen_; i += distance_zero) {
        distance_zero = out[i];
        out[i] = COBS1_MARKER;
    }

    *outlen = outlen_;

    return 0;
}

/**
 * @brief Accumulate incoming byte into RX buffer
 *
 * @param value
 */
void COBS1Transceiver::receive(uint8_t value) {
    rx[rxlen++] = value;

    if (rxlen >= rx_maxlen) {
        logger_error("COBS1: too much data\n");
        reset();
    }
}

void COBS1Transceiver::parse(void) {
#ifdef COBS_DISABLE
    // Just copy incoming data to packet
    memcpy(pkt, rx, rxlen);
    pktlen = rxlen;
    reset();
    has_packet_ = true;

#else
    uint16_t num_new = rxlen - rxpos;

    if (num_new == 0) {
        return;
    }

    logger_debug("COBS1: %s: got: %u: ", __func__, num_new);
    logger_hexdump(VERB_DEBUG, &rx[rxpos], num_new);

    while (rxpos < rxlen) {
        transition(rx[rxpos++]);

        if (state == State::RECOGNIZED) {
            logger_debug("COBS1: %s: frame[%u]\n", __func__, rxlen);
            try_decode();
            reset();
        }
    }
#endif // COBS_DISABLE
}

/**
 * @brief Attempt to decode incoming data
 */
void COBS1Transceiver::try_decode(void) {
    if (has_packet_) {
        logger_error("COBS1: %s: packet full\n", __func__);

    } else {
        int err = decode(rx, rxlen, pkt, &pktlen);

        if (err) {
            logger_error("COBS1: %s: failed: %d\n", __func__, err);
        } else {
            has_packet_ = true;
        }
    }
}

int COBS1Transceiver::pop(uint8_t *buf, uint16_t *buflen, uint16_t buf_maxlen) {
    if (!has_packet_) {
        logger_info("%s: no packet\n", __func__);
        return -1;
    }

    if (buf_maxlen < pktlen) {
        logger_error("%s: buffer too short: %u\n", __func__, buf_maxlen);
        return -1;
    }

    memcpy(buf, pkt, pktlen);
    *buflen = pktlen;

    has_packet_ = false;

    return 0;
}

/**
 * @brief Reest state & RX buffer
 */
void COBS1Transceiver::reset(void) {
    rxlen = 0;
    rxpos = 0;
    transition_to(State::GROUND);
}

bool COBS1Transceiver::has_packet(void) {
    return has_packet_;
}

void COBS1Transceiver::transition_to(State next) {
    logger_debug("COBS1: State: %d -> %d\n", state, next);
    state = next;
}

void COBS1Transceiver::transition(uint8_t value) {
    switch (state) {
        case State::GROUND:
            if (is_marker(value)) {
                transition_to(State::LEADER);
            }

            break;

        case State::LEADER:
            if (!is_marker(value)) {
                transition_to(State::PAYLOAD);
            }

            break;

        case State::PAYLOAD:
            if (is_marker(value)) {
                transition_to(State::RECOGNIZED);
            }

            break;

        case State::RECOGNIZED:
            break;
    }
}
