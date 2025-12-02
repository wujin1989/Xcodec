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

_Pragma("once")

#include <stdint.h>
#include <stdbool.h>

typedef struct xcodec_alaw_encoder_s        xcodec_alaw_encoder_t;
typedef struct xcode_alaw_enc_params_s      xcode_alaw_enc_params_t;
typedef enum xcode_alaw_sample_rates_e      xcode_alaw_sample_rates_t;
typedef enum xcode_alaw_channel_layouts_e   xcode_alaw_channel_layouts_t;
typedef struct xcodec_alaw_encoder_module_s xcodec_alaw_encoder_module_t;

enum xcode_alaw_sample_rates_e {
    ALAW_SAMPLE_RATE_8000  = 8000,
    ALAW_SAMPLE_RATE_16000 = 16000,
    ALAW_SAMPLE_RATE_44100 = 44100,
    ALAW_SAMPLE_RATE_48000 = 48000,
};

enum xcode_alaw_channel_layouts_e {
    ALAW_CHANNEL_LAYOUT_MONO   = 1,
    ALAW_CHANNEL_LAYOUT_STEREO = 2,
};

struct xcode_alaw_enc_params_s {
    xcode_alaw_sample_rates_t sample_rate;
    xcode_alaw_channel_layouts_t channel_layout;
};

struct xcodec_alaw_encoder_s {
    void* opaque;
};

struct xcodec_alaw_encoder_module_s {
    const char* restrict name;

    xcodec_alaw_encoder_t* (*create)(xcode_alaw_enc_params_t* params);
    bool (*encode)(
        xcodec_alaw_encoder_t* encoder,
        int16_t*               src,
        int                    slen,
        uint8_t*               dst,
        int                    dlen);
    void (*destroy)(xcodec_alaw_encoder_t* encoder);
};

extern xcodec_alaw_encoder_module_t xcodec_alaw_encoder;
