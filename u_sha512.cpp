#include <string.h>
#include <stdint.h>

#include "u_sha512.h"
#include "u_algorithm.h"
#include "u_misc.h"

namespace u {

const uint64_t sha512::K[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
    0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
    0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
    0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
    0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
    0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
    0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
    0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
    0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
    0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
    0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
    0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

void sha512::init() {
    m_length = 0;
    m_currentLength = 0;

    m_state[0] = 0x6a09e667f3bcc908ULL;
    m_state[1] = 0xbb67ae8584caa73bULL;
    m_state[2] = 0x3c6ef372fe94f82bULL;
    m_state[3] = 0xa54ff53a5f1d36f1ULL;
    m_state[4] = 0x510e527fade682d1ULL;
    m_state[5] = 0x9b05688c2b3e6c1fULL;
    m_state[6] = 0x1f83d9abfb41bd6bULL;
    m_state[7] = 0x5be0cd19137e2179ULL;
}

sha512::sha512() {
    init();
}

sha512::sha512(const unsigned char *buf, size_t length) {
    init();
    process(buf, length);
    done();
}

void sha512::compress(const unsigned char *buf) {
    uint64_t S[8];
    uint64_t W[80];
    uint64_t t0;
    uint64_t t1;

    // Copy state size_to S
    for (size_t i = 0; i < 8; i++)
        S[i] = m_state[i];
    // Copy the state size_to 1024-bits size_to W[0..15]
    for (size_t i = 0; i < 16; i++)
        W[i] = load64(buf + (8*i));
    // Fill W[16..79]
    for (size_t i = 16; i < 80; i++)
        W[i] = gamma1(W[i - 2]) + W[i - 7] + gamma0(W[i - 15]) + W[i - 16];
    // Compress
    auto round = [&](uint64_t a, uint64_t b, uint64_t c, uint64_t& d,
            uint64_t e, uint64_t f, uint64_t g, uint64_t& h, uint64_t i) {
        t0 = h + sigma1(e) + ch(e, f, g) + K[i] + W[i];
        t1 = sigma0(a) + maj(a, b, c);
        d += t0;
        h = t0 + t1;
    };

    for (size_t i = 0; i < 80; i += 8) {
        round(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i+0);
        round(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], i+1);
        round(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], i+2);
        round(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], i+3);
        round(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], i+4);
        round(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], i+5);
        round(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], i+6);
        round(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], i+7);
    }

    // Feedback
    for (size_t i = 0; i < 8; i++)
        m_state[i] = m_state[i] + S[i];
}

void sha512::process(const unsigned char *in, size_t length) {
    const size_t block_size = sizeof(m_buffer);
    while (length > 0) {
        if (m_currentLength == 0 && length >= block_size) {
            compress(in);
            m_length += block_size * 8;
            in += block_size;
            length -= block_size;
        } else {
            size_t n = u::min(length, (block_size - m_currentLength));
            memcpy(m_buffer + m_currentLength, in, n);
            m_currentLength += n;
            in += n;
            length -= n;
            if (m_currentLength == block_size) {
                compress(m_buffer);
                m_length += 8 * block_size;
                m_currentLength = 0;
            }
        }
    }
}

void sha512::done() {
    m_length += m_currentLength * 8ULL;
    m_buffer[m_currentLength++] = 0x80;

    if (m_currentLength > 112) {
        while (m_currentLength < 128)
            m_buffer[m_currentLength++] = 0;
        compress(m_buffer);
        m_currentLength = 0;
    }

    // Pad upto 120 bytes of zeroes
    // Note: Bytes 112 to 120 is the 64 MSBs of the length.
    while (m_currentLength < 120)
        m_buffer[m_currentLength++] = 0;

    store64(m_length, m_buffer + 120);
    compress(m_buffer);

    for (size_t i = 0; i < 8; i++)
        store64(m_state[i], m_out + (8*i));
}

u::string sha512::hex() {
    u::string thing;
    for (size_t i = 0; i != 512 / 8; ++i)
        thing += u::format("%02x", m_out[i]);
    return thing;
}

}
