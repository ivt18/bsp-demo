#include "wad.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool load_wad(const char *wad_path, WAD *wad)
{
    FILE *fptr;
    size_t bytes_read;
    long file_size;
    bool ret = 0;

    // TODO: check that it is safe to open wad_path
    fptr = fopen(wad_path, "rb");
    if (!fptr) {
        perror("Failed to open WAD file");
        ret = 1;
        goto exit_load;
    }

    /* Get the size of the file. */
    if (fseek(fptr, 0, SEEK_END)) {
        perror("Failed to go to end of file");
        ret = 1;
        goto exit_load;
    }
    file_size = ftell(fptr);
    if (file_size == -1) {
        fprintf(stderr, "Failed to get file size.\n");
        ret = 1;
        goto exit_load;
    }
    wad->sz = (size_t)file_size;
    fseek(fptr, 0, SEEK_SET);

    /* Allocate memory. */
    wad->data = (uint8_t *)malloc(wad->sz);
    if (!wad->data) {
        fprintf(stderr, "Failed to allocate memory for WAD.\n");
        ret = 1;
        goto exit_load;
    }

    /* Read the WAD. */
    bytes_read = fread(wad->data, 1, wad->sz, fptr);
    if (bytes_read != wad->sz) {
        fprintf(stderr, "Failed to read whole WAD.\n");
        ret = 1;
        goto exit_load;
    }

exit_load:
    fclose(fptr);
    return ret;
}

bool read_wad_uint8(const WAD *wad, uint8_t *dst, size_t offset)
{
    /* Check for null pointers. */
    if (!wad || !wad->data) {
        fprintf(stderr, "Cannot load data from unloaded WAD or null pointer.\n");
        return 1;
    }
    if (!dst) {
        fprintf(stderr, "Cannot load data into null pointer destination.\n");
        return 1;
    }

    /* Check that the data we want to load exists. */
    if (wad->sz < offset + 1) {
        fprintf(stderr, "Cannot load data beyond WAD size.\n");
        return 1;
    }

    /* Copy the data. */
    memcpy(dst, wad->data + offset, 1);

    return 0;
}

bool read_wad_uint16(const WAD *wad, uint16_t *dst, size_t offset)
{
    /* Check for null pointers. */
    if (!wad || !wad->data) {
        fprintf(stderr, "Cannot load data from unloaded WAD or null pointer.\n");
        return 1;
    }
    if (!dst) {
        fprintf(stderr, "Cannot load data into null pointer destination.\n");
        return 1;
    }

    /* Check that the data we want to load exists. */
    if (wad->sz < offset + 2) {
        fprintf(stderr, "Cannot load data beyond WAD size.\n");
        return 1;
    }

    /* Copy the data. */
    memcpy(dst, wad->data + offset, 2);

    return 0;
}

bool read_wad_uint32(const WAD *wad, uint32_t *dst, size_t offset)
{
    /* Check for null pointers. */
    if (!wad || !wad->data) {
        fprintf(stderr, "Cannot load data from unloaded WAD or null pointer.\n");
        return 1;
    }
    if (!dst) {
        fprintf(stderr, "Cannot load data into null pointer destination.\n");
        return 1;
    }

    /* Check that the data we want to load exists. */
    if (wad->sz < offset + 4) {
        fprintf(stderr, "Cannot load data beyond WAD size.\n");
        return 1;
    }

    /* Copy the data. */
    memcpy(dst, wad->data + offset, 4);

    return 0;
}

bool load_header(const WAD* wad, Header* header)
{
    /* Check for null pointers. */
    if (!wad || !wad->data) {
        fprintf(stderr, "Cannot load header from unloaded WAD or null pointer.\n");
        return 1;
    }
    if (!header) {
        fprintf(stderr, "Cannot load header into null pointer.\n");
        return 1;
    }

    /* Check that there is actual data. */
    if (wad->sz < 12) {
        fprintf(stderr, "WAD file given does not contain a header.\n");
        return 1;
    }

    /* Read WAD type and add null terminator. */
    for (size_t i = 0; i < 4; i++) {
        if (read_wad_uint8(wad, (uint8_t *)header->wad_type + i, i)) {
            fprintf(stderr, "Could not read WAD type.\n");
            return 1;
        }
    }
    header->wad_type[4] = '\0';

    /* Check that it is a valid WAD type. */
    bool IWAD = strncmp(header->wad_type, "IWAD", 4);
    bool PWAD = strncmp(header->wad_type, "PWAD", 4);
    if (!IWAD && !PWAD) {
        fprintf(stderr, "Invalid WAD type read.\n");
        return 1;
    }

    /* Read number of directories. */
    if (read_wad_uint32(wad, &header->num_directories, 4)) {
        fprintf(stderr, "Could not read number of directories.\n");
        return 1;
    }

    /* Read offset of listing. */
    if (read_wad_uint32(wad, &header->listing_offset, 8)) {
        fprintf(stderr, "Could not read offset of listing.\n");
        return 1;
    }

    return 0;
}

bool load_directory(const WAD* wad, Directory* directory, size_t offset)
{
    /* Check for null pointers. */
    if (!wad || !wad->data) {
        fprintf(stderr, "Cannot load header from unloaded WAD or null pointer.\n");
        return 1;
    }
    if (!directory) {
        fprintf(stderr, "Cannot load directory into null pointer.\n");
        return 1;
    }

    /* Read offset of lump. */
    if (read_wad_uint32(wad, &directory->lump_offset, offset)) {
        fprintf(stderr, "Could not read lump offset for directory.\n");
        return 1;
    }

    /* Read size of lump. */
    if (read_wad_uint32(wad, &directory->lump_size, offset + 4)) {
        fprintf(stderr, "Could not read lump size for directory.\n");
        return 1;
    }

    /* Read name of lump. */
    for (size_t i = 0; i < 8; i++) {
        if (read_wad_uint8(wad, (uint8_t *)directory->lump_name + i, offset + 8 + i)) {
            fprintf(stderr, "Could not read lump name.\n");
            return 1;
        }
    }
    directory->lump_name[8] = '\0';

    return 0;
}