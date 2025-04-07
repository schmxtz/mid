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
    // TODO: Fix if subchunk is bigger
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

// Returns samples as list of float with values in [-1, 1]
int read_data(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer) {
    switch (fmt_sub_chunk->bits_per_sample)
    {
    case 8:
        return read_data_one_byte(fmt_sub_chunk, data_sub_chunk, buffer);
    case 16:
        return read_data_two_byte(fmt_sub_chunk, data_sub_chunk, buffer);
    case 32:
        return read_data_four_byte(fmt_sub_chunk, data_sub_chunk, buffer);
    default:
        printf("Samples with %d bit-depth are not yet supported", fmt_sub_chunk->bits_per_sample);
        return 1;
        break;
    }

    return 0;
}

int read_data_one_byte(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer) {
    int32_t sub_chunk_size = data_sub_chunk->sub_chunk_size;
    int16_t num_channel = fmt_sub_chunk->num_channel;
    int16_t block_align = fmt_sub_chunk->block_align;
    int sample_ctr = 0;
    float sample;
    for (int i = 0; i < sub_chunk_size; i += block_align) {
        sample = 0;
        for (int j = 0; j < block_align; j ++) {
            sample += buffer[i + j] + INT8_MIN; // 128 is middle point for 8bit samples (unsigned)
        }
        if (fmt_sub_chunk->num_channel > 1) sample /= fmt_sub_chunk->num_channel;
        data_sub_chunk->data[sample_ctr++] = sample / -INT8_MIN;
    }
    return 0;
}

int read_data_two_byte(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer) {
    int32_t sub_chunk_size = data_sub_chunk->sub_chunk_size;
    int16_t num_channel = fmt_sub_chunk->num_channel;
    int16_t block_align = fmt_sub_chunk->block_align;
    int sample_ctr = 0;
    float sample;
    for (int i = 0; i < sub_chunk_size; i += block_align) {
        sample = 0;
        for (int j = 0; j < block_align; j += 2) {
            sample += *((int16_t *) (buffer + i + j));
        }
        if (fmt_sub_chunk->num_channel > 1) sample /= fmt_sub_chunk->num_channel;
        data_sub_chunk->data[sample_ctr++] = sample / -INT16_MIN;
    }
    return 0;
}

int read_data_four_byte(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, uint8_t *buffer) {
    int32_t sub_chunk_size = data_sub_chunk->sub_chunk_size;
    int16_t num_channel = fmt_sub_chunk->num_channel;
    int16_t block_align = fmt_sub_chunk->block_align;
    int sample_ctr = 0;
    float sample;
    for (int i = 0; i < sub_chunk_size; i += block_align) {
        sample = 0;
        for (int j = 0; j < block_align; j += 4) {
            sample += *((int32_t *) (buffer + i + j));
        }
        if (fmt_sub_chunk->num_channel > 1) sample /= fmt_sub_chunk->num_channel;
        data_sub_chunk->data[sample_ctr++] = sample / -INT32_MIN;
    }
    return 0;
}