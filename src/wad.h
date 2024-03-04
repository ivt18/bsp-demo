#ifndef WAD_LOADER_H
#define WAD_LOADER_H

#include <stdint.h>
#include <stdbool.h>

/* Data. */
typedef struct {
    uint8_t *data;
    size_t sz;
} WAD;

/* Header. */
typedef struct {
    char wad_type[5];
    uint32_t num_directories;
    uint32_t listing_offset;
} Header;

/* Directory. */
typedef struct {
    uint32_t lump_offset;
    uint32_t lump_size;
    char lump_name[9];
} Directory;

/**
 * @brief Load the WAD into memory, given the path to the WAD file.
 * 
 * @param path The path to the WAD file.
 * @param wad Pointer where to store the loaded WAD.
 * @returns 0 on success, 1 on failure.
 */
bool load_wad(const char *path, WAD *wad);

/**
 * @brief Read one byte from the WAD into another variable.
 * 
 * @param wad Pointer to loaded WAD.
 * @param dst Pointer to destination to store byte.
 * @param offset Offset of byte to load.
 * @returns 0 on success, 1 on failure.
 */
bool read_wad_uint8(const WAD *wad, uint8_t *dst, size_t offset);

/**
 * @brief Read two bytes from the WAD into another variable.
 * 
 * @param wad Pointer to loaded WAD.
 * @param dst Pointer to destination to store bytes.
 * @param offset Offset of bytes to load.
 * @returns 0 on success, 1 on failure.
 */
bool read_wad_uint16(const WAD *wad, uint16_t *dst, size_t offset);

/**
 * @brief Read four bytes from the WAD into another variable.
 * 
 * @param wad Pointer to loaded WAD.
 * @param dst Pointer to destination to store bytes.
 * @param offset Offset of bytes to load.
 * @returns 0 on success, 1 on failure.
 */
bool read_wad_uint32(const WAD *wad, uint32_t *dst, size_t offset);

/**
 * @brief Load the header of a WAD.
 * 
 * @param wad Pointer to loaded WAD.
 * @param header Pointer where to store the loaded header.
 * @returns 0 on success, 1 on failure.
 */
bool load_header(const WAD* wad, Header* header);

/**
 * @brief Load a WAD directory.
 * 
 * @param wad Pointer to loaded WAD.
 * @param directory Pointer where to store the loaded directory.
 * @returns 0 on success, 1 on failure.
 */
bool load_directory(const WAD* wad, Directory* directory, size_t offset);

#endif // WAD_LOADER_H
