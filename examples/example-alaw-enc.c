/**
 * @file example_alaw_enc.c
 * @brief Demo: Encode raw PCM (s16 interleaved) to A-law using xcodec_alaw.
 *
 * Usage:
 *   ./example_alaw_enc input.pcm output.alaw 48000 2
 *
 * Arguments:
 *   argv[1] = input PCM file (raw s16 interleaved, native endianness)
 *   argv[2] = output A-law file
 *   argv[3] = sample rate (e.g., 8000, 44100, 48000)
 *   argv[4] = number of channels (1=mono, 2=stereo, etc.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xcodec.h"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(
            stderr,
            "Usage: %s <input.pcm> <output.alaw> <sample_rate> <channels>\n"
            "Example: %s speech.pcm speech.alaw 8000 1\n",
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
    xcodec_alaw_encoder_t* encoder =
        xcodec_alaw_encoder.create(sample_rate, channels);
    if (!encoder) {
        fprintf(stderr, "Failed to create A-law encoder.\n");
        fclose(fin);
        fclose(fout);
        return EXIT_FAILURE;
    }
    // Process in chunks (e.g., 20ms @ 16kHz mono = 320 * 2 * 1 = 640 bytes)
    uint8_t pcm_buf[640];
    uint8_t alaw_buf[640];

    size_t total_pcm_bytes = 0;
    size_t total_alaw_bytes = 0;

    while (!feof(fin)) {
        size_t nread = fread(pcm_buf, 1, sizeof(pcm_buf), fin);
        if (nread == 0)
            break;
        // Ensure even byte count (s16 samples)
        if (nread % 2 != 0) {
            fprintf(
                stderr,
                "Warning: Truncated PCM sample at end (odd byte count). "
                "Truncating.\n");
            nread--;
            if (nread == 0)
                break;
        }
        int encoded_len =
            xcodec_alaw_encoder.encode(encoder, pcm_buf, (int)nread, alaw_buf);
        if (encoded_len < 0) {
            fprintf(
                stderr, "Encoding failed with error code: %d\n", encoded_len);
            break;
        }
        fwrite(alaw_buf, 1, (size_t)encoded_len, fout);
        total_pcm_bytes += nread;
        total_alaw_bytes += (size_t)encoded_len;
    }
    xcodec_alaw_encoder.destroy(encoder);
    fclose(fin);
    fclose(fout);

    printf("Encoding completed.\n");
    printf(
        "  Input:  %zu bytes (%zu s16 samples)\n",
        total_pcm_bytes,
        total_pcm_bytes / 2);
    printf("  Output: %zu bytes (A-law)\n", total_alaw_bytes);
    printf(
        "  Ratio:  %.2f:1\n",
        (double)total_pcm_bytes / (double)total_alaw_bytes);

    return EXIT_SUCCESS;
}