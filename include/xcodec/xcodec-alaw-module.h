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

#include <stdbool.h>
#include <stdint.h>

typedef struct xcodec_alaw_encoder_s        xcodec_alaw_encoder_t;
typedef struct xcodec_alaw_encoder_module_s xcodec_alaw_encoder_module_t;

typedef struct xcodec_alaw_encoder_s        xcodec_alaw_decoder_t;
typedef struct xcodec_alaw_decoder_module_s xcodec_alaw_decoder_module_t;

struct xcodec_alaw_encoder_s {
    void* opaque;
};

struct xcodec_alaw_encoder_module_s {
    const char* restrict name;

    xcodec_alaw_encoder_t* (*create)(int samplerate, int channels);

    /**
     * @brief Encode raw PCM audio data into A-law compressed data.
     *
     * This function encodes raw PCM audio data directly into a provided output
     * buffer as A-law compressed data. It is designed for maximum performance
     * when the input PCM data conforms to the specific format requirements
     * detailed below.
     *
     * **CRITICAL INPUT FORMAT REQUIREMENTS (for `pcm` data):**
     *   - **Sample Format:** 16-bit signed integer (s16).
     *   - **Layout:** Interleaved (Packed). Planar layout is not supported.
     *   - **Endianness:** Host native byte order. The function does NOT perform
     *     byte-swapping. Data must be pre-converted if read from a foreign
     *     endianness source (e.g., network stream, big-endian file).
     *
     * **OUTPUT BUFFER MANAGEMENT (Caller's Responsibility):**
     *   - The `alaw` parameter must point to a caller-allocated output buffer.
     *   - The buffer must have sufficient capacity to hold the encoded A-law
     *     data. A **safe and minimum** allocation size is `(pcmlen / 2)` bytes, as
     *     each 16-bit PCM sample is encoded into exactly 1 byte of A-law data.
     *   - The caller is responsible for managing the lifecycle of this buffer
     *     (allocation and deallocation).
     *
     * @param encoder Pointer to the encoder instance, obtained from create().
     * @param pcm     Pointer to the buffer containing the raw PCM audio data to encode.
     * @param pcmlen  Length of the input PCM data in bytes.
     * @param alaw    Pointer to a caller-allocated output buffer.
     *                On successful return, this buffer will contain the encoded
     *                A-law data.
     *
     * @return        On success: The length (in bytes) of the encoded A-law
     *                data written to the `alaw` buffer. This will be exactly `(pcmlen / 2)`.
     *                On failure: A negative integer error code.
     *                Typical error codes may indicate invalid parameters or
     *                unsupported PCM data format.
     */
    int (*encode)(xcodec_alaw_encoder_t* encoder, uint8_t* pcm, int pcmlen, uint8_t* alaw);
    void (*destroy)(xcodec_alaw_encoder_t* encoder);
};

struct xcodec_alaw_decoder_module_s {
    const char* restrict name;

    xcodec_alaw_decoder_t* (*create)(int samplerate, int channels);
    int (*decode)(xcodec_alaw_decoder_t* decoder, uint8_t* alaw, int alawlen, uint8_t* pcm);
    void (*destroy)(xcodec_alaw_decoder_t* decoder);
};

extern xcodec_alaw_encoder_module_t xcodec_alaw_encoder;
extern xcodec_alaw_decoder_module_t xcodec_alaw_decoder;
