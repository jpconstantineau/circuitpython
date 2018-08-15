/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/displayio/Bitmap.h"

#include <string.h>

#include "py/runtime.h"

void common_hal_displayio_bitmap_construct(displayio_bitmap_t *self, uint32_t width,
    uint32_t height, uint32_t value_size) {
    uint32_t row_width = width * value_size;
    // word align
    if (row_width % 32 != 0) {
        self->stride = (row_width / 32 + 1);
    } else {
        self->stride = row_width / 32;
    }
    self->width = width;
    self->height = height;
    self->data = m_malloc(self->stride * height * sizeof(uint32_t), false);

    self->bits_per_value = value_size;

    self->x_shift = 0;
    uint32_t power_of_two = 1;
    while (power_of_two < 32 / value_size ) {
        self->x_shift++;
        power_of_two <<= 1;
    }
    self->x_mask = (1 << self->x_shift) - 1; // Used as a modulus on the x value
    self->bitmask = (1 << value_size) - 1;
}

void common_hal_displayio_bitmap_load_row(displayio_bitmap_t *self, uint16_t y, uint8_t* data, uint16_t len) {
    if (len != self->stride * 4) {
        mp_raise_ValueError(translate("row must be packed and word aligned"));
    }
    uint32_t* row_value = self->data + (y * self->stride);
    // Do the memcpy ourselves since we may want to flip endianness.
    for (uint32_t i = 0; i < self->stride; i++) {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcast-align"
        uint32_t value = ((uint32_t *)data)[i];
        #pragma GCC diagnostic pop
        if (self->bits_per_value < 16) {
            value = ((value >> 24) & 0xff) |
                    ((value << 8) & 0xff0000) |
                    ((value >> 8) & 0xff00) |
                    ((value << 24) & 0xff000000);
        }
        *row_value = value;
        row_value++;
    }
}
uint32_t common_hal_displayio_bitmap_get_pixel(displayio_bitmap_t *self, int16_t x, int16_t y) {
    int32_t row_start = y * self->stride;
    if (self->bits_per_value < 8) {
        uint32_t word = self->data[row_start + (x >> self->x_shift)];

        return (word >> (32 - ((x & self->x_mask) + 1) * self->bits_per_value)) & self->bitmask;
    } else {
        uint32_t bytes_per_value = self->bits_per_value / 8;
        return self->data[row_start + x * bytes_per_value];
    }
}
