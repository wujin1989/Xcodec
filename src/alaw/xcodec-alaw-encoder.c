/** Copyright (c) 2025, Wu Jin <wujin.developer@gmail.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include <stdlib.h>
#include "xcodec-alaw-encoder.h"

typedef struct alaw_encoder_context_s {
    int     sample_rate;
    int     channels;
    uint8_t alaw_table[16384];
} alaw_encoder_context_t;

static int _alaw_to_linear_convert(unsigned char a_val) {
    int t;
    int seg;

    a_val ^= 0x55;

    t = a_val & 0xf;
    seg = ((unsigned)a_val & 0x70) >> 4;
    if (seg)
        t = (t + t + 1 + 32) << (seg + 2);
    else
        t = (t + t + 1) << 3;

    return (a_val & 0x80) ? t : -t;
}

static void
_alaw_table_build(uint8_t* table) {
    int i, j, v, v1, v2;

    j = 1;
    table[8192] = 0xd5;
    for (i = 0; i < 127; i++) {
        v1 = _alaw_to_linear_convert(i ^ 0xd5);
        v2 = _alaw_to_linear_convert((i + 1) ^ 0xd5);
        v = (v1 + v2 + 4) >> 3;
        for (; j < v; j += 1) {
            table[8192 - j] = (i ^ (0xd5 ^ 0x80));
            table[8192 + j] = (i ^ 0xd5);
        }
    }
    for (; j < 8192; j++) {
        table[8192 - j] = (127 ^ (0xd5 ^ 0x80));
        table[8192 + j] = (127 ^ 0xd5);
    }
    table[0] = table[1];
}

xcodec_alaw_encoder_t*
xcodec_alaw_encoder_create(int samplerate, int channels) {
    xcodec_alaw_encoder_t* enc = malloc(sizeof(xcodec_alaw_encoder_t));
    if (!enc) {
        return NULL;
    }
    enc->opaque = malloc(sizeof(alaw_encoder_context_t));
    if (!enc->opaque) {
        free(enc);
        return NULL;
    }
    alaw_encoder_context_t* ctx = enc->opaque;

    ctx->sample_rate = samplerate;
    ctx->channels = channels;
    _alaw_table_build(ctx->alaw_table);
    return enc;
}

int xcodec_alaw_encoder_encode(
    xcodec_alaw_encoder_t* encoder, uint8_t* pcm, int pcmlen, uint8_t* alaw) {
    if (!encoder || !alaw || !pcm || pcmlen <= 0) {
        return -1;
    }
    alaw_encoder_context_t* ctx = encoder->opaque;

    int      v;
    int16_t* samples = (int16_t*)pcm;
    int      n_samples = pcmlen / sizeof(int16_t);
    int      alawlen = n_samples;

    for (; n_samples > 0; n_samples--) {
        v = *samples++;
        *alaw++ = ctx->alaw_table[(v + 32768) >> 2];
    }
    return alawlen;
}

void xcodec_alaw_encoder_destroy(xcodec_alaw_encoder_t* encoder) {
    if (encoder) {
        if (encoder->opaque) {
            free(encoder->opaque);
        }
        free(encoder);
    }
}