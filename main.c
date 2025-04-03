#include <stdio.h>

#include "wav.c"

int main() {
    WavFile *wav_file = (WavFile *) malloc(sizeof(WavFile));
    if (read_file(wav_file, "sm8.wav") != 0) {
        printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
        return 1;
    }

    // printf("%.4s\n", wav_file->riff_chunk->chunk_id);
    // printf("%.4s\n", wav_file->riff_chunk->chunk_format);
    // printf("%.4s\n", wav_file->fmt_sub_chunk->sub_chunk_id);
    // printf("%d\n", wav_file->fmt_sub_chunk->bits_per_sample);
    // printf("TERMINATED");
}