# include "wav.h"

int read_file(WavFile *wav_file, char *file_name) {
    wav_file->riff_chunk = (RIFFChunk *) malloc(sizeof(RIFFChunk));
    if (wav_file->riff_chunk == NULL) return 1;
    wav_file->fmt_sub_chunk = (FMTSubChunk *) malloc(sizeof(FMTSubChunk));
    if (wav_file->fmt_sub_chunk == NULL) return 1; 
    wav_file->data_sub_chunk = (DataSubChunk *) malloc(sizeof(DataSubChunk));
    if (wav_file->data_sub_chunk == NULL) return 1;


    FILE *file = fopen(file_name, "r");
    if (file == NULL) return 1;
    if (fread(wav_file->riff_chunk, sizeof(RIFFChunk), 1, file) != 1) return 1;
    if (fread(wav_file->fmt_sub_chunk, sizeof(FMTSubChunk), 1, file) != 1) return 1;
    
    // TODO: Implement other subchunk types, for now skip until data subchunk 
    SubChunkBuffer *subchunk_buffer = (SubChunkBuffer *) malloc(sizeof(SubChunkBuffer));
    if (subchunk_buffer == NULL) return 1;
    if (fread(subchunk_buffer, sizeof(SubChunkBuffer), 1, file) != 1) return 1;
    while (strncmp(subchunk_buffer->sub_chunk_id, DATACHUNKID, 4) != 0) {
        if (fseek(file, subchunk_buffer->sub_chunk_size, SEEK_CUR) != 0) return 1;
        if (fread(subchunk_buffer, sizeof(SubChunkBuffer), 1, file) != 1) return 1;
    }
    memcpy(wav_file->data_sub_chunk->sub_chunk_id, subchunk_buffer->sub_chunk_id, 4);
    wav_file->data_sub_chunk->sub_chunk_size = subchunk_buffer->sub_chunk_size;
    free(subchunk_buffer);
    wav_file->data_sub_chunk->num_sample = wav_file->data_sub_chunk->sub_chunk_size / wav_file->fmt_sub_chunk->block_align;
    wav_file->data_sub_chunk->data = (float *) malloc(sizeof(float) * wav_file->data_sub_chunk->num_sample);
    if (wav_file->data_sub_chunk->data == NULL) return 1;

    /* TOOD: Check if reading into smaller buffers is more efficient, this might be 
        very naive, especially considering the file size of some WAVE files */
    uint8_t *buffer = (uint8_t *) malloc(sizeof(uint8_t) * wav_file->data_sub_chunk->sub_chunk_size);
    if (buffer == NULL) return 1;
    if (fread(buffer, sizeof(uint8_t) * wav_file->data_sub_chunk->sub_chunk_size, 1, file) != 1) return 1;
    fclose(file);

    return read_data(wav_file->fmt_sub_chunk, wav_file->data_sub_chunk, buffer);
}

int read_data(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer) {
    switch(fmt_sub_chunk->bits_per_sample) {
        case 8:
            return read_data_one_byte(fmt_sub_chunk, data_sub_chunk, buffer);
        case 16:
            return read_data_two_byte(fmt_sub_chunk, data_sub_chunk, buffer);
        case 24:
            break;
        case 32:
            break;
        default:
            return 1;
    }
    

    return 0;
}

int read_data_one_byte(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer) {
    int32_t num_sample = data_sub_chunk->num_sample;
    int16_t num_channel = fmt_sub_chunk->num_channel;
    int16_t block_align = fmt_sub_chunk->block_align;
    int sample = 0;
    float signal;
    for (int i = 0; i < num_sample; i += block_align) {
        signal = 0;
        for (int j = 0; j < num_channel; j++) {
            signal += (buffer[i + j] - 128);
        }
        data_sub_chunk->data[sample++] = signal / num_channel;
        printf("%.4f %f\n", (float) i / fmt_sub_chunk->sample_rate, data_sub_chunk->data[sample-1] / INT8_MAX);
    }
    return 0;
}

int read_data_two_byte(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer) {
    int32_t num_sample = data_sub_chunk->num_sample;
    int16_t num_channel = fmt_sub_chunk->num_channel;
    int16_t block_align = fmt_sub_chunk->block_align;
    int16_t byte_per_sample = fmt_sub_chunk->bits_per_sample / 8;
    int sample = 0;
    float signal;
    for (int i = 0; i < num_sample; i += block_align) {
        signal = 0;
        for (int j = 0; j < num_channel; j += byte_per_sample) {
            signal += (int16_t) (buffer[i + j] << 8) + (buffer[i + j + 1]);
        }
        data_sub_chunk->data[sample++] = (signal / num_channel) ;
        printf("%.4f %f\n", (float) (sample-1) / fmt_sub_chunk->sample_rate ,data_sub_chunk->data[sample-1]);
    }
    return 0;
}