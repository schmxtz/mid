#ifndef _WAV_HEADER
#define _WAV_HEADER

#define DATACHUNKID "data"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char            chunk_id[4];        // "RIFF"
    int32_t         chunk_size;         // total file size (in bytes) - 8
    char            chunk_format[4];    // "WAVE"
} RIFFChunk;

typedef struct {
    char            sub_chunk_id[4];     // "fmt "
    int32_t         sub_chunk_size;      
    int16_t         audio_format;       // 1 for PCM
    int16_t         num_channel;        
    int32_t         sample_rate;
    int32_t         byte_rate;
    int16_t         block_align;
    int16_t         bits_per_sample;
} FMTSubChunk;

typedef struct {
    char            sub_chunk_id[4];     // "data"
    int32_t         sub_chunk_size;
    int32_t         num_sample;         // (subchunk_size / block_align)
    float           *data;
} DataSubChunk;

typedef struct {
    RIFFChunk       *riff_chunk;
    FMTSubChunk     *fmt_sub_chunk;
    DataSubChunk    *data_sub_chunk;
} WavFile;

typedef struct {
    char            sub_chunk_id[4];
    int32_t         sub_chunk_size;
} SubChunkBuffer;

int read_file(WavFile *wav_file, char *file_name);
int read_data(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer);
int read_data_one_byte(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer);
int read_data_two_byte(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer);
int read_data_four_byte(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer);

#endif
