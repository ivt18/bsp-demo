#ifndef WAD_LOADER_H
#define WAD_LOADER_H

#include <stdint.h>
#include <stdbool.h>

/* Data. */
typedef struct {
    uint8_t *data;
    size_t sz;
} wad_data;

/* Header. */
typedef struct {
    char wad_type[5];
    uint32_t num_directories;
    uint32_t listing_offset;
} wad_header;

/* Directory. */
typedef struct {
    uint32_t lump_offset;
    uint32_t lump_size;
    char lump_name[9];
} wad_directory;

/**
 * @brief Load the WAD into memory, given the path to the WAD file.
 * 
 * @param wad_path The path to the WAD file.
 * @param data Pointer where to store the loaded WAD.
 * @returns 0 on success, 1 on failure.
 */
bool load_wad(const char *wad_path, wad_data *data);

/**
 * @brief Read one byte from the WAD into another variable.
 * 
 * @param data Pointer to loaded WAD.
 * @param dst Pointer to destination to store byte.
 * @param offset Offset of byte to load.
 * @returns 0 on success, 1 on failure.
 */
bool read_wad_uint8(const wad_data *data, uint8_t *dst, size_t offset);

/**
 * @brief Read four bytes from the WAD into another variable.
 * 
 * @param data Pointer to loaded WAD.
 * @param dst Pointer to destination to store bytes.
 * @param offset Offset of bytes to load.
 * @returns 0 on success, 1 on failure.
 */
bool read_wad_uint32(const wad_data *data, uint32_t *dst, size_t offset);

/**
 * @brief Load the header of a WAD.
 * 
 * @param data Pointer to loaded WAD.
 * @param header Pointer where to store the loaded header.
 * @returns 0 on success, 1 on failure.
 */
bool load_header(const wad_data* data, wad_header* header);

/**
 * @brief Load a WAD directory.
 * 
 * @param data Pointer to loaded WAD.
 * @param directory Pointer where to store the loaded directory.
 * @returns 0 on success, 1 on failure.
 */
bool load_directory(const wad_data* data, wad_directory* directory, size_t offset);

#endif // WAD_LOADER_H
