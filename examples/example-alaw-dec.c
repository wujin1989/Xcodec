/**
 * @file alaw_decode_demo.c
 * @brief Demo: Decode A-law audio to raw PCM (s16) using xcodec_alaw.
 *
 * Usage:
 *   ./alaw_decode_demo input.alaw output.pcm 8000 1
 *
 * Arguments:
 *   argv[1] = input A-law file (raw 8-bit A-law samples)
 *   argv[2] = output PCM file (s16 interleaved, native endianness)
 *   argv[3] = sample rate (e.g., 8000, 16000)
 *   argv[4] = number of channels (1=mono, 2=stereo)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xcodec.h"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(
            stderr,
            "Usage: %s <input.alaw> <output.pcm> <sample_rate> <channels>\n"
            "Example: %s speech.alaw speech.pcm 8000 1\n",
            argv[0],
            argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];
    int         sample_rate = atoi(argv[3]);
    int         channels = atoi(argv[4]);

    if (sample_rate <= 0 || channels <= 0 || channels > 8) {
        fprintf(stderr, "Invalid sample_rate or channels.\n");
        return EXIT_FAILURE;
    }
    FILE* fin = fopen(input_file, "rb");
    if (!fin) {
        perror("fopen input");
        return EXIT_FAILURE;
    }
    FILE* fout = fopen(output_file, "wb");
    if (!fout) {
        perror("fopen output");
        fclose(fin);
        return EXIT_FAILURE;
    }
    xcodec_alaw_decoder_t* decoder =
        xcodec_alaw_decoder.create(sample_rate, channels);
    if (!decoder) {
        fprintf(stderr, "Failed to create A-law decoder.\n");
        fclose(fin);
        fclose(fout);
        return EXIT_FAILURE;
    }
    uint8_t alaw_buf[640];
    uint8_t pcm_buf[1280];

    size_t total_alaw_bytes = 0;
    size_t total_pcm_bytes = 0;

    while (!feof(fin)) {
        size_t nread = fread(alaw_buf, 1, sizeof(alaw_buf), fin);
        if (nread == 0)
            break;

        int decoded_len =
            xcodec_alaw_decoder.decode(decoder, alaw_buf, (int)nread, pcm_buf);
        if (decoded_len < 0) {
            fprintf(
                stderr, "Decoding failed with error code: %d\n", decoded_len);
            break;
        }
        fwrite(pcm_buf, 1, (size_t)decoded_len, fout);
        total_alaw_bytes += nread;
        total_pcm_bytes += (size_t)decoded_len;
    }
    xcodec_alaw_decoder.destroy(decoder);
    fclose(fin);
    fclose(fout);

    printf("Decoding completed.\n");
    printf("  Input:  %zu bytes (A-law)\n", total_alaw_bytes);
    printf(
        "  Output: %zu bytes (%zu s16 samples)\n",
        total_pcm_bytes,
        total_pcm_bytes / 2);
    printf(
        "  Ratio:  %.2f:1 (PCM:A-law)\n",
        (double)total_pcm_bytes / (double)total_alaw_bytes);

    return EXIT_SUCCESS;
}