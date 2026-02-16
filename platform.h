/**
 * @file platform.h
 * @brief Platform abstraction for 64-bit file seek (and later: mkdir, getopt on Windows).
 * macOS: fseeko; Linux: fseeko64. Windows: _fseeki64.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Seek to a 64-bit offset in a file stream (Windows).
 * @param stream  File stream
 * @param offset  Offset in bytes
 * @param whence  SEEK_SET, SEEK_CUR, or SEEK_END
 * @return 0 on success, non-zero on error
 */
int xbt_fseek64(FILE *stream, int64_t offset, int whence);
#ifdef __cplusplus
}
#endif
#else
#include <sys/types.h>
/**
 * @brief Seek to a 64-bit offset in a file stream (Linux/macOS).
 * @param stream  File stream
 * @param offset  Offset in bytes
 * @param whence  SEEK_SET, SEEK_CUR, or SEEK_END
 * @return 0 on success, non-zero on error
 */
#ifdef __APPLE__
static inline int xbt_fseek64(FILE *stream, int64_t offset, int whence)
{
    return fseeko(stream, (off_t)offset, whence);
}
#else
static inline int xbt_fseek64(FILE *stream, int64_t offset, int whence)
{
    return fseeko64(stream, (off_t)offset, whence);
}
#endif
#endif

#endif
