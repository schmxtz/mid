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
    fread(wav_file->riff_chunk, sizeof(RIFFChunk), 1, file);
    fread(wav_file->fmt_sub_chunk, sizeof(FMTSubChunk), 1, file);

    // TODO: Implement other subchunk types, for now skip until data subchunk 
    SubChunkBuffer *subchunk_buffer = (SubChunkBuffer *) malloc(sizeof(SubChunkBuffer));
    if (subchunk_buffer == NULL) return 1;
    fread(subchunk_buffer, sizeof(SubChunkBuffer), 1, file);
    while (strcmp(subchunk_buffer->sub_chunk_id, DATACHUNKID) != 0) {
        if (fseek(file, subchunk_buffer->sub_chunk_size, SEEK_CUR) != 0) return 1;
        fread(subchunk_buffer, sizeof(SubChunkBuffer), 1, file);
    }

    strcpy(wav_file->data_sub_chunk->sub_chunk_id, subchunk_buffer->sub_chunk_id);
    wav_file->data_sub_chunk->sub_chunk_size = subchunk_buffer->sub_chunk_size;
    free(subchunk_buffer);
    wav_file->data_sub_chunk->num_sample = wav_file->data_sub_chunk->sub_chunk_size / wav_file->fmt_sub_chunk->block_align;
    wav_file->data_sub_chunk->data = (float *) malloc(sizeof(float) * wav_file->data_sub_chunk->num_sample);
    if (wav_file->data_sub_chunk->data == NULL) return 1;

    int result = read_data(wav_file->fmt_sub_chunk, wav_file->data_sub_chunk, file);
    fclose(file);
    return result;
}

int read_data(FMTSubChunk *fmt_sub_chunk, DataSubChunk *data_sub_chunk, FILE *file) {
    printf("%d\n", fmt_sub_chunk->bits_per_sample); 

    return 0;
}