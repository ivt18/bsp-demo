#ifndef WAD_LOADER_H
#define WAD_LOADER_H

#include <stdint.h>
#include <stdbool.h>

/* Data. */
struct wad_data {
    uint8_t *data;
    size_t sz;
};

/* Header */
struct wad_header {
    char wad_type[5];
    uint32_t num_directories;
    uint32_t listing_offset;
};

/* Directories */
struct wad_directory {
    uint32_t lump_offset;
    uint32_t lump_size;
    char lump_name[8];
};

/**
 * @brief Load the WAD into memory, given the path to the WAD file.
 * 
 * @param wad_path The path to the WAD file.
 * @param data Pointer where to store the loaded WAD.
 * @returns 0 on success, 1 on failure.
 */
bool load_wad(const char *wad_path, struct wad_data *data);

/**
 * @brief Load the header of a WAD given the path to the WAD file.
 * 
 * @param wad_path The path to the WAD file.
 * @param header Pointer where to store the loaded header.
 * @returns 0 on success, 1 on failure.
 */
bool load_header(const char* wad_path, struct wad_header* header);

#endif // WAD_LOADER_H
