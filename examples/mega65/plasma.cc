// Copyright 2023 LLVM-MOS Project
// Licensed under the Apache License, Version 2.0 with LLVM Exceptions.
// See https://github.com/llvm-mos/llvm-mos-sdk/blob/main/LICENSE for license
// information.

/*
MIT License

Copyright (c) 2023 Mikael Lund aka Wombat

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 * Simplistic character-mode plasma effect
 *
 * Inspired by cc65/samples/cbm
 * - 2001 by groepaz
 * - Cleanup and porting by Ullrich von Bassewitz
 * - 2023 Mega65/LLVM-MOS C++ adaptation (Wombat)
 */

#include <array>
#include <cstdint>
#include <mega65.h>

/*
 * Simple pseudo-random number generator
 * See https://en.wikipedia.org/wiki/Xorshift
 */
class RandomXORS {
private:
  uint32_t state = 7;

public:
  inline uint8_t rand8() { return static_cast<uint8_t>(rand32() & 0xff); }
  inline uint32_t rand32() {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
  }
};

/*
 * Sets MEGA65 speed to 3.5 Mhz
 */
void speed_mode3() {
  VICIV.ctrlb |= VIC3_FAST_MASK;
  VICIV.ctrlc &= ~VIC4_VFAST_MASK;
}

/*
 * Cyclic sine lookup table
 */
constexpr uint8_t sine_table[UINT8_MAX + 1] = {
    0x80, 0x7d, 0x7a, 0x77, 0x74, 0x70, 0x6d, 0x6a, 0x67, 0x64, 0x61, 0x5e,
    0x5b, 0x58, 0x55, 0x52, 0x4f, 0x4d, 0x4a, 0x47, 0x44, 0x41, 0x3f, 0x3c,
    0x39, 0x37, 0x34, 0x32, 0x2f, 0x2d, 0x2b, 0x28, 0x26, 0x24, 0x22, 0x20,
    0x1e, 0x1c, 0x1a, 0x18, 0x16, 0x15, 0x13, 0x11, 0x10, 0x0f, 0x0d, 0x0c,
    0x0b, 0x0a, 0x08, 0x07, 0x06, 0x06, 0x05, 0x04, 0x03, 0x03, 0x02, 0x02,
    0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03,
    0x03, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x0a, 0x0b, 0x0c, 0x0d, 0x0f,
    0x10, 0x11, 0x13, 0x15, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24,
    0x26, 0x28, 0x2b, 0x2d, 0x2f, 0x32, 0x34, 0x37, 0x39, 0x3c, 0x3f, 0x41,
    0x44, 0x47, 0x4a, 0x4d, 0x4f, 0x52, 0x55, 0x58, 0x5b, 0x5e, 0x61, 0x64,
    0x67, 0x6a, 0x6d, 0x70, 0x74, 0x77, 0x7a, 0x7d, 0x80, 0x83, 0x86, 0x89,
    0x8c, 0x90, 0x93, 0x96, 0x99, 0x9c, 0x9f, 0xa2, 0xa5, 0xa8, 0xab, 0xae,
    0xb1, 0xb3, 0xb6, 0xb9, 0xbc, 0xbf, 0xc1, 0xc4, 0xc7, 0xc9, 0xcc, 0xce,
    0xd1, 0xd3, 0xd5, 0xd8, 0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8,
    0xea, 0xeb, 0xed, 0xef, 0xf0, 0xf1, 0xf3, 0xf4, 0xf5, 0xf6, 0xf8, 0xf9,
    0xfa, 0xfa, 0xfb, 0xfc, 0xfd, 0xfd, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfd, 0xfd, 0xfc, 0xfb, 0xfa,
    0xfa, 0xf9, 0xf8, 0xf6, 0xf5, 0xf4, 0xf3, 0xf1, 0xf0, 0xef, 0xed, 0xeb,
    0xea, 0xe8, 0xe6, 0xe4, 0xe2, 0xe0, 0xde, 0xdc, 0xda, 0xd8, 0xd5, 0xd3,
    0xd1, 0xce, 0xcc, 0xc9, 0xc7, 0xc4, 0xc1, 0xbf, 0xbc, 0xb9, 0xb6, 0xb3,
    0xb1, 0xae, 0xab, 0xa8, 0xa5, 0xa2, 0x9f, 0x9c, 0x99, 0x96, 0x93, 0x90,
    0x8c, 0x89, 0x86, 0x83};

/*
 * Generate charset with 8 * 256 characters at given address
 */
void make_charset(uint16_t charset_address, RandomXORS &rng) {
  /*
   * Lambda function to generate a single 8x8 pixels pattern
   */
  auto make_char = [&](const uint8_t sine) {
    uint8_t pattern = 0;
    constexpr uint8_t bits[8] = {1, 2, 4, 8, 16, 32, 64, 128};
    for (const auto bit : bits) {
      if (rng.rand8() > sine) {
        pattern |= bit;
      }
    }
    return pattern;
  };

  auto charset = reinterpret_cast<volatile uint8_t *>(charset_address);
  for (const auto sine : sine_table) {
    for (int _i = 0; _i < 8; ++_i) {
      *(charset++) = make_char(sine);
    }
  }
}

/*
 * Plasma class
 */
template <size_t COLS, size_t ROWS> class Plasma {
private:
  std::array<uint8_t, ROWS> ydata;
  std::array<uint8_t, COLS> xdata;
  uint8_t x_cnt1 = 0;
  uint8_t x_cnt2 = 0;
  uint8_t y_cnt1 = 0;
  uint8_t y_cnt2 = 0;

public:
  /*
   * Generate and activate charset at given address
   */
  Plasma(const uint16_t charset_address, RandomXORS &rng) {
    make_charset(charset_address, rng);
    VICIV.charptr = charset_address;
  }

  /*
   * Draw frame
   */
  inline void update() {
    auto i = y_cnt1;
    auto j = y_cnt2;
    for (auto &y : ydata) {
      y = sine_table[i] + sine_table[j];
      i += 4;
      j += 9;
    }
    i = x_cnt1;
    j = x_cnt2;
    for (auto &x : xdata) {
      x = sine_table[i] + sine_table[j];
      i += 3;
      j += 7;
    }
    x_cnt1 += 2;
    x_cnt2 -= 3;
    y_cnt1 += 3;
    y_cnt2 -= 5;

    write_to_screen();
  }

  /*
   * Write summed buffers to screen memory
   */
  inline void write_to_screen() const {
    auto screen_ptr = reinterpret_cast<volatile uint8_t *>(&DEFAULT_SCREEN);
    for (const auto y : ydata) {
#pragma unroll
      for (const auto x : xdata) {
        *(screen_ptr++) = y + x;
      }
    }
  }
};

int main() {
  constexpr size_t COLS = 80;
  constexpr size_t ROWS = 25;
  constexpr uint16_t CHARSET_ADDRESS = 0x3000;
  RandomXORS rng;
  Plasma<COLS, ROWS> plasma(CHARSET_ADDRESS, rng);
  speed_mode3();
  while (true) {
    plasma.update();
  }
}
