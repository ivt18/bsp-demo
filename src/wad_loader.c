#include "wad_loader.h"

#include <stdio.h>
#include <string.h>

bool load_header(const char* wad_path, struct wad_header* header)
{
    FILE *fptr;
    size_t bytes_read;
    int32_t ret = 0;

    // TODO: check that it is safe to open wad_path
    fptr = fopen(wad_path, "rb");
    if (!fptr) {
        perror("Failed to open WAD file");
        ret = 1;
        goto exit_load_header;
    }

    /* Read WAD type. */
    bytes_read = fread(header->wad_type, sizeof(char), 4, fptr);
    if (bytes_read != 4) {
        fprintf(stderr, "Could not read WAD type from WAD file.\n");
        ret = 1;
        goto exit_load_header;
    }

    /* Add null terminator to the WAD type. */
    header->wad_type[4] = '\0';

    /* Check that it is a valid WAD type. */
    bool IWAD = strncmp(header->wad_type, "IWAD", 4);
    bool PWAD = strncmp(header->wad_type, "PWAD", 4);
    if (!IWAD && !PWAD) {
        fprintf(stderr, "Invalid WAD type read.\n");
        ret = 1;
        goto exit_load_header;
    }

    /* Read number of directories. */
    bytes_read = fread(&header->num_directories, sizeof(uint32_t), 1, fptr);
    if (bytes_read != 1) {
        fprintf(stderr, "Could not read number of directories from WAD file.\n");
        ret = 1;
        goto exit_load_header;
    }

    /* Read offset of listing. */
    bytes_read = fread(&header->listing_offset, sizeof(uint32_t), 1, fptr);
    if (bytes_read != 1) {
        fprintf(stderr, "Could not read offset of listing from WAD file.\n");
        ret = 1;
        goto exit_load_header;
    }

exit_load_header:
    fclose(fptr);
    return ret;
}