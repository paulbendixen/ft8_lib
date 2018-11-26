#include "encode.h"

// Define the LDPC sizes
constexpr int N = 174;          // Number of bits in the encoded message
constexpr int K = 87;           // Number of payload bits
constexpr int M = N - K;        // Number of checksum bits

constexpr uint16_t  POLYNOMIAL = 0xC06;  // CRC-12 polynomial without the leading (MSB) 1

constexpr int K_BYTES = (K + 7) / 8;    // Number of whole bytes needed to store K bits

// Parity generator matrix for (174,87) LDPC code, stored in bitpacked format (MSB first)
const uint8_t kGenerator[M][K_BYTES] = {
    { 0x23, 0xbb, 0xa8, 0x30, 0xe2, 0x3b, 0x6b, 0x6f, 0x50, 0x98, 0x2e },
    { 0x1f, 0x8e, 0x55, 0xda, 0x21, 0x8c, 0x5d, 0xf3, 0x30, 0x90, 0x52 },
    { 0xca, 0x7b, 0x32, 0x17, 0xcd, 0x92, 0xbd, 0x59, 0xa5, 0xae, 0x20 },
    { 0x56, 0xf7, 0x83, 0x13, 0x53, 0x7d, 0x0f, 0x43, 0x82, 0x96, 0x4e },
    { 0x29, 0xc2, 0x9d, 0xba, 0x9c, 0x54, 0x5e, 0x26, 0x77, 0x62, 0xfe },
    { 0x6b, 0xe3, 0x96, 0xb5, 0xe2, 0xe8, 0x19, 0xe3, 0x73, 0x34, 0x0c },
    { 0x29, 0x35, 0x48, 0xa1, 0x38, 0x85, 0x83, 0x28, 0xaf, 0x42, 0x10 },
    { 0xcb, 0x6c, 0x6a, 0xfc, 0xdc, 0x28, 0xbb, 0x3f, 0x7c, 0x6e, 0x86 },
    { 0x3f, 0x2a, 0x86, 0xf5, 0xc5, 0xbd, 0x22, 0x5c, 0x96, 0x11, 0x50 },
    { 0x84, 0x9d, 0xd2, 0xd6, 0x36, 0x73, 0x48, 0x18, 0x60, 0xf6, 0x2c },
    { 0x56, 0xcd, 0xae, 0xc6, 0xe7, 0xae, 0x14, 0xb4, 0x3f, 0xee, 0xee },
    { 0x04, 0xef, 0x5c, 0xfa, 0x37, 0x66, 0xba, 0x77, 0x8f, 0x45, 0xa4 },
    { 0xc5, 0x25, 0xae, 0x4b, 0xd4, 0xf6, 0x27, 0x32, 0x0a, 0x39, 0x74 },
    { 0xfe, 0x37, 0x80, 0x29, 0x41, 0xd6, 0x6d, 0xde, 0x02, 0xb9, 0x9c },
    { 0x41, 0xfd, 0x95, 0x20, 0xb2, 0xe4, 0xab, 0xeb, 0x2f, 0x98, 0x9c },
    { 0x40, 0x90, 0x7b, 0x01, 0x28, 0x0f, 0x03, 0xc0, 0x32, 0x39, 0x46 },
    { 0x7f, 0xb3, 0x6c, 0x24, 0x08, 0x5a, 0x34, 0xd8, 0xc1, 0xdb, 0xc4 },
    { 0x40, 0xfc, 0x3e, 0x44, 0xbb, 0x7d, 0x2b, 0xb2, 0x75, 0x6e, 0x44 },
    { 0xd3, 0x8a, 0xb0, 0xa1, 0xd2, 0xe5, 0x2a, 0x8e, 0xc3, 0xbc, 0x76 },
    { 0x3d, 0x0f, 0x92, 0x9e, 0xf3, 0x94, 0x9b, 0xd8, 0x4d, 0x47, 0x34 },
    { 0x45, 0xd3, 0x81, 0x4f, 0x50, 0x40, 0x64, 0xf8, 0x05, 0x49, 0xae },
    { 0xf1, 0x4d, 0xbf, 0x26, 0x38, 0x25, 0xd0, 0xbd, 0x04, 0xb0, 0x5e },
    { 0xf0, 0x8a, 0x91, 0xfb, 0x2e, 0x1f, 0x78, 0x29, 0x06, 0x19, 0xa8 },
    { 0x7a, 0x8d, 0xec, 0x79, 0xa5, 0x1e, 0x8a, 0xc5, 0x38, 0x80, 0x22 },
    { 0xca, 0x41, 0x86, 0xdd, 0x44, 0xc3, 0x12, 0x15, 0x65, 0xcf, 0x5c },
    { 0xdb, 0x71, 0x4f, 0x8f, 0x64, 0xe8, 0xac, 0x7a, 0xf1, 0xa7, 0x6e },
    { 0x8d, 0x02, 0x74, 0xde, 0x71, 0xe7, 0xc1, 0xa8, 0x05, 0x5e, 0xb0 },
    { 0x51, 0xf8, 0x15, 0x73, 0xdd, 0x40, 0x49, 0xb0, 0x82, 0xde, 0x14 },
    { 0xd0, 0x37, 0xdb, 0x82, 0x51, 0x75, 0xd8, 0x51, 0xf3, 0xaf, 0x00 },
    { 0xd8, 0xf9, 0x37, 0xf3, 0x18, 0x22, 0xe5, 0x7c, 0x56, 0x23, 0x70 },
    { 0x1b, 0xf1, 0x49, 0x06, 0x07, 0xc5, 0x40, 0x32, 0x66, 0x0e, 0xde },
    { 0x16, 0x16, 0xd7, 0x80, 0x18, 0xd0, 0xb4, 0x74, 0x5c, 0xa0, 0xf2 },
    { 0xa9, 0xfa, 0x8e, 0x50, 0xbc, 0xb0, 0x32, 0xc8, 0x5e, 0x33, 0x04 },
    { 0x83, 0xf6, 0x40, 0xf1, 0xa4, 0x8a, 0x8e, 0xbc, 0x04, 0x43, 0xea },
    { 0xec, 0xa9, 0xaf, 0xa0, 0xf6, 0xb0, 0x1d, 0x92, 0x30, 0x5e, 0xdc },
    { 0x37, 0x76, 0xaf, 0x54, 0xcc, 0xfb, 0xae, 0x91, 0x6a, 0xfd, 0xe6 },
    { 0x6a, 0xbb, 0x21, 0x2d, 0x97, 0x39, 0xdf, 0xc0, 0x25, 0x80, 0xf2 },
    { 0x05, 0x20, 0x9a, 0x0a, 0xbb, 0x53, 0x0b, 0x9e, 0x7e, 0x34, 0xb0 },
    { 0x61, 0x2f, 0x63, 0xac, 0xc0, 0x25, 0xb6, 0xab, 0x47, 0x6f, 0x7c },
    { 0x0a, 0xf7, 0x72, 0x31, 0x61, 0xec, 0x22, 0x30, 0x80, 0xbe, 0x86 },
    { 0xa8, 0xfc, 0x90, 0x69, 0x76, 0xc3, 0x56, 0x69, 0xe7, 0x9c, 0xe0 },
    { 0x45, 0xb7, 0xab, 0x62, 0x42, 0xb7, 0x74, 0x74, 0xd9, 0xf1, 0x1a },
    { 0xb2, 0x74, 0xdb, 0x8a, 0xbd, 0x3c, 0x6f, 0x39, 0x6e, 0xa3, 0x56 },
    { 0x90, 0x59, 0xdf, 0xa2, 0xbb, 0x20, 0xef, 0x7e, 0xf7, 0x3a, 0xd4 },
    { 0x3d, 0x18, 0x8e, 0xa4, 0x77, 0xf6, 0xfa, 0x41, 0x31, 0x7a, 0x4e },
    { 0x8d, 0x90, 0x71, 0xb7, 0xe7, 0xa6, 0xa2, 0xee, 0xd6, 0x96, 0x5e },
    { 0xa3, 0x77, 0x25, 0x37, 0x73, 0xea, 0x67, 0x83, 0x67, 0xc3, 0xf6 },
    { 0xec, 0xbd, 0x7c, 0x73, 0xb9, 0xcd, 0x34, 0xc3, 0x72, 0x0c, 0x8a },
    { 0xb6, 0x53, 0x7f, 0x41, 0x7e, 0x61, 0xd1, 0xa7, 0x08, 0x53, 0x36 },
    { 0x6c, 0x28, 0x0d, 0x2a, 0x05, 0x23, 0xd9, 0xc4, 0xbc, 0x59, 0x46 },
    { 0xd3, 0x6d, 0x66, 0x2a, 0x69, 0xae, 0x24, 0xb7, 0x4d, 0xcb, 0xd8 },
    { 0xd7, 0x47, 0xbf, 0xc5, 0xfd, 0x65, 0xef, 0x70, 0xfb, 0xd9, 0xbc },
    { 0xa9, 0xfa, 0x2e, 0xef, 0xa6, 0xf8, 0x79, 0x6a, 0x35, 0x57, 0x72 },
    { 0xcc, 0x9d, 0xa5, 0x5f, 0xe0, 0x46, 0xd0, 0xcb, 0x3a, 0x77, 0x0c },
    { 0xf6, 0xad, 0x48, 0x24, 0xb8, 0x7c, 0x80, 0xeb, 0xfc, 0xe4, 0x66 },
    { 0xcc, 0x6d, 0xe5, 0x97, 0x55, 0x42, 0x09, 0x25, 0xf9, 0x0e, 0xd2 },
    { 0x16, 0x4c, 0xc8, 0x61, 0xbd, 0xd8, 0x03, 0xc5, 0x47, 0xf2, 0xac },
    { 0xc0, 0xfc, 0x3e, 0xc4, 0xfb, 0x7d, 0x2b, 0xb2, 0x75, 0x66, 0x44 },
    { 0x0d, 0xbd, 0x81, 0x6f, 0xba, 0x15, 0x43, 0xf7, 0x21, 0xdc, 0x72 },
    { 0xa0, 0xc0, 0x03, 0x3a, 0x52, 0xab, 0x62, 0x99, 0x80, 0x2f, 0xd2 },
    { 0xbf, 0x4f, 0x56, 0xe0, 0x73, 0x27, 0x1f, 0x6a, 0xb4, 0xbf, 0x80 },
    { 0x57, 0xda, 0x6d, 0x13, 0xcb, 0x96, 0xa7, 0x68, 0x9b, 0x27, 0x90 },
    { 0x81, 0xcf, 0xc6, 0xf1, 0x8c, 0x35, 0xb1, 0xe1, 0xf1, 0x71, 0x14 },
    { 0x48, 0x1a, 0x2a, 0x0d, 0xf8, 0xa2, 0x35, 0x83, 0xf8, 0x2d, 0x6c },
    { 0x1a, 0xc4, 0x67, 0x2b, 0x54, 0x9c, 0xd6, 0xdb, 0xa7, 0x9b, 0xcc },
    { 0xc8, 0x7a, 0xf9, 0xa5, 0xd5, 0x20, 0x6a, 0xbc, 0xa5, 0x32, 0xa8 },
    { 0x97, 0xd4, 0x16, 0x9c, 0xb3, 0x3e, 0x74, 0x35, 0x71, 0x8d, 0x90 },
    { 0xa6, 0x57, 0x3f, 0x3d, 0xc8, 0xb1, 0x6c, 0x9d, 0x19, 0xf7, 0x46 },
    { 0x2c, 0x41, 0x42, 0xbf, 0x42, 0xb0, 0x1e, 0x71, 0x07, 0x6a, 0xcc },
    { 0x08, 0x1c, 0x29, 0xa1, 0x0d, 0x46, 0x8c, 0xcd, 0xbc, 0xec, 0xb6 },
    { 0x5b, 0x0f, 0x77, 0x42, 0xbc, 0xa8, 0x6b, 0x80, 0x12, 0x60, 0x9a },
    { 0x01, 0x2d, 0xee, 0x21, 0x98, 0xeb, 0xa8, 0x2b, 0x19, 0xa1, 0xda },
    { 0xf1, 0x62, 0x77, 0x01, 0xa2, 0xd6, 0x92, 0xfd, 0x94, 0x49, 0xe6 },
    { 0x35, 0xad, 0x3f, 0xb0, 0xfa, 0xeb, 0x5f, 0x1b, 0x0c, 0x30, 0xdc },
    { 0xb1, 0xca, 0x4e, 0xa2, 0xe3, 0xd1, 0x73, 0xba, 0xd4, 0x37, 0x9c },
    { 0x37, 0xd8, 0xe0, 0xaf, 0x92, 0x58, 0xb9, 0xe8, 0xc5, 0xf9, 0xb2 },
    { 0xcd, 0x92, 0x1f, 0xdf, 0x59, 0xe8, 0x82, 0x68, 0x37, 0x63, 0xf6 },
    { 0x61, 0x14, 0xe0, 0x84, 0x83, 0x04, 0x3f, 0xd3, 0xf3, 0x8a, 0x8a },
    { 0x2e, 0x54, 0x7d, 0xd7, 0xa0, 0x5f, 0x65, 0x97, 0xaa, 0xc5, 0x16 },
    { 0x95, 0xe4, 0x5e, 0xcd, 0x01, 0x35, 0xac, 0xa9, 0xd6, 0xe6, 0xae },
    { 0xb3, 0x3e, 0xc9, 0x7b, 0xe8, 0x3c, 0xe4, 0x13, 0xf9, 0xac, 0xc8 },
    { 0xc8, 0xb5, 0xdf, 0xfc, 0x33, 0x50, 0x95, 0xdc, 0xdc, 0xaf, 0x2a },
    { 0x3d, 0xd0, 0x1a, 0x59, 0xd8, 0x63, 0x10, 0x74, 0x3e, 0xc7, 0x52 },
    { 0x14, 0xcd, 0x0f, 0x64, 0x2f, 0xc0, 0xc5, 0xfe, 0x3a, 0x65, 0xca },
    { 0x3a, 0x0a, 0x1d, 0xfd, 0x7e, 0xee, 0x29, 0xc2, 0xe8, 0x27, 0xe0 },
    { 0x8a, 0xbd, 0xb8, 0x89, 0xef, 0xbe, 0x39, 0xa5, 0x10, 0xa1, 0x18 },
    { 0x3f, 0x23, 0x1f, 0x21, 0x20, 0x55, 0x37, 0x1c, 0xf3, 0xe2, 0xa2 }    
};

// Column order (permutation) in which the bits in codeword are stored
const uint8_t kColumn_order[174] = {
   0,  1,  2,  3, 30,  4,  5,  6,  7,  8,  9, 10, 11, 32, 12, 40, 13, 14, 15, 16,
  17, 18, 37, 45, 29, 19, 20, 21, 41, 22, 42, 31, 33, 34, 44, 35, 47, 51, 50, 43,
  36, 52, 63, 46, 25, 55, 27, 24, 23, 53, 39, 49, 59, 38, 48, 61, 60, 57, 28, 62,
  56, 58, 65, 66, 26, 70, 64, 69, 68, 67, 74, 71, 54, 76, 72, 75, 78, 77, 80, 79,
  73, 83, 84, 81, 82, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
 100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
 120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,
 140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
 160,161,162,163,164,165,166,167,168,169,170,171,172,173
};

// Costas 7x7 tone pattern
const uint8_t kCostas_map[] = { 2,5,6,0,4,1,3 };


// Returns 1 if an odd number of bits are set in x, zero otherwise
uint8_t parity8(uint8_t x) {
    x ^= x >> 4;    // a b c d ae bf cg dh
    x ^= x >> 2;    // a b ac bd cae dbf aecg bfdh
    x ^= x >> 1;    // a ab bac acbd bdcae caedbf aecgbfdh
    return (x) & 1;
}


// Encode an 87-bit message and return a 174-bit codeword. 
// The generator matrix has dimensions (87,87). 
// The code is a (174,87) regular ldpc code with column weight 3.
// The code was generated using the PEG algorithm.
// After creating the codeword, the columns are re-ordered according to 
// "kColumn_order" to make the codeword compatible with the parity-check matrix 
// Arguments:
// [IN] message   - array of 87 bits stored as 11 bytes (MSB first)
// [OUT] codeword - array of 174 bits stored as 22 bytes (MSB first)
void encode174(const uint8_t *message, uint8_t *codeword) {
    // Here we don't generate the generator bit matrix as in WSJT-X implementation
    // Instead we access the generator bits straight from the binary representation in kGenerator

    // Also we don't use the itmp temporary buffer, instead filling codeword bit by bit
    // in the reordered order as we compute the result. 

    // For reference:
    //   itmp(1:M)=pchecks
    //   itmp(M+1:N)=message(1:K)
    //   codeword(kColumn_order+1)=itmp(1:N)

    int colidx = 0; // track the current column in codeword

    // Fill the codeword with zeroes, as we will only update binary ones later
    for (int i = 0; i < (N + 7) / 8; i++) {
        codeword[i] = 0;
    }

    // Compute the first part of itmp (1:M) and store the result in codeword
    for (int i = 0; i < M; ++i) { // do i=1,M
        // Fast implementation of bitwise multiplication and parity checking
        // Normally nsum would contain the result of dot product between message and kGenerator[i], 
        // but we only compute the sum modulo 2.
        uint8_t nsum = 0;
        for (int j = 0; j < K_BYTES; ++j) {
            uint8_t bits = message[j] & kGenerator[i][j];    // bitwise AND (bitwise multiplication)
            nsum ^= parity8(bits);                  // bitwise XOR (addition modulo 2)
        }
        // Check if we need to set a bit in codeword
        if (nsum % 2) { // pchecks(i)=mod(nsum,2)
            uint8_t col = kColumn_order[colidx];     // Index of the bit to set
            codeword[col/8] |= (1 << (7 - col%8));
        }
        ++colidx;
    }

    // Compute the second part of itmp (M+1:N) and store the result in codeword
    uint8_t mask = 0x80;    // Rolling mask starting with the MSB
    for (int j = 0; j < K; ++j) {
        // Copy the j-th bit from message to codeword
        if (message[j/8] & mask) {
            uint8_t col = kColumn_order[colidx];     // Index of the bit to set
            codeword[col/8] |= (1 << (7 - col%8));
        }
        ++colidx;

        // Roll the bitmask to the right
        mask >>= 1;
        if (mask == 0) mask = 0x80;
    }
}


// Compute 12-bit CRC for a sequence of given number of bits
// [IN] message  - byte sequence (MSB first)
// [IN] num_bits - number of bits in the sequence
uint16_t ft8_crc(uint8_t *message, int num_bits) {
    // Adapted from https://barrgroup.com/Embedded-Systems/How-To/CRC-Calculation-C-Code
    constexpr int       WIDTH = 12;
    constexpr uint16_t  TOPBIT = (1 << (WIDTH - 1));

    uint16_t remainder = 0;
    int idx_byte = 0;

    // Perform modulo-2 division, a bit at a time.
    for (int idx_bit = 0; idx_bit < num_bits; ++idx_bit) {
        if (idx_bit % 8 == 0) {
            // Bring the next byte into the remainder.
            remainder ^= (message[idx_byte] << (WIDTH - 8));
            ++idx_byte;
        }

        // Try to divide the current data bit.
        if (remainder & TOPBIT) {
            remainder = (remainder << 1) ^ POLYNOMIAL;
        }
        else {
            remainder = (remainder << 1);
        }
    }
    return remainder & ((1 << WIDTH) - 1);
}


// Generate FT8 tone sequence from payload data
// [IN] payload - 9 byte array consisting of 72 bit payload (MSB first)
// [IN] i3      - 3 bits containing message type (zero?)
// [OUT] itone  - array of NN (79) bytes to store the generated tones (encoded as 0..7)
void genft8(const uint8_t *payload, uint8_t i3, uint8_t *itone) {
    uint8_t a87[11];    // Store 72 bits of payload + 3 bits i3 + 12 bits CRC

    for (int i = 0; i < 9; i++)
        a87[i] = payload[i];

    // Append 3 bits of i3 at the end of 72 bit payload
    a87[9] = ((i3 & 0x07) << 5);

    // Calculate CRC of 11 bytes = 88 bits, see WSJT-X code
    uint16_t checksum = ft8_crc(a87, 88 - 12);

    // Store the CRC at the end of 75 bit message (yes, 72 + 3)
    uint16_t tmp = (checksum << 1);
    a87[9] |= (uint8_t)(tmp >> 8);
    a87[10] = (uint8_t)tmp;

    // a87 contains 72 bits of payload + 3 bits of i3 + 12 bits of CRC
    uint8_t codeword[22];
    encode174(a87, codeword);

    // Message structure: S7 D29 S7 D29 S7
    for (int i = 0; i < 7; ++i) {
        itone[i] = kCostas_map[i];
        itone[36 + i] = kCostas_map[i];
        itone[72 + i] = kCostas_map[i];
    }

    int k = 7;          // Skip over the first set of Costas symbols
    for (int j = 0; j < ND; ++j) { // do j=1,ND
        if (j == 29) {
            k += 7;     // Skip over the second set of Costas symbols
        }
        // Extract 3 bits from codeword at i-th position
        itone[k] = 0;
        int i = 3*j;
        if (codeword[i/8] & (1 << (7 - i%8))) itone[k] |= 4;
        ++i;
        if (codeword[i/8] & (1 << (7 - i%8))) itone[k] |= 2;
        ++i;
        if (codeword[i/8] & (1 << (7 - i%8))) itone[k] |= 1;
        ++k;
    }
}
