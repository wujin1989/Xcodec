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
#include "xcodec-alaw-decoder.h"

typedef struct alaw_decoder_context_s {
    int     sample_rate;
    int     channels;
    int16_t pcm_table[256];
} alaw_decoder_context_t;

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

static void _pcm_table_build(int16_t* table) {
    for (int i = 0; i < 256; i++) {
        table[i] = _alaw_to_linear_convert(i);
    }
}

xcodec_alaw_decoder_t*
xcodec_alaw_decoder_create(int samplerate, int channels) {
    xcodec_alaw_decoder_t* dec = malloc(sizeof(xcodec_alaw_decoder_t));
    if (!dec) {
        return NULL;
    }
    dec->opaque = malloc(sizeof(alaw_decoder_context_t));
    if (!dec->opaque) {
        free(dec);
        return NULL;
    }
    alaw_decoder_context_t* ctx = dec->opaque;

    ctx->sample_rate = samplerate;
    ctx->channels = channels;
    _pcm_table_build(ctx->pcm_table);
    return dec;
}

int xcodec_alaw_decoder_decode(
    xcodec_alaw_decoder_t* decoder, uint8_t* alaw, int alawlen, uint8_t* pcm) {
    if (!decoder || !alaw || !pcm || alawlen <= 0) {
        return -1;
    }
    alaw_decoder_context_t* ctx = decoder->opaque;

    const int16_t* const lut = ctx->pcm_table;
    int16_t* restrict samples = (int16_t*)pcm;
    int n = 0;
    int pcmlen = 0;

    n = ctx->channels;
    if (n && alawlen % n) {
        if (alawlen >= n) {
            alawlen -= alawlen % n;
        }
    }
    n = alawlen;
    pcmlen = n * 2;

    for (; n > 0; n--) {
        *samples++ = lut[*alaw++];
    }
    return pcmlen;
}

void xcodec_alaw_decoder_destroy(xcodec_alaw_decoder_t* decoder) {
    if (decoder) {
        if (decoder->opaque) {
            free(decoder->opaque);
        }
        free(decoder);
    }
}