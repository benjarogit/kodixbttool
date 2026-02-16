/**
 * @file platform_win.c
 * @brief Windows implementation of 64-bit file seek for platform.h.
 * Part of kodixbttool; compile only on Windows builds.
 */
#include "platform.h"
#ifdef _WIN32
#include <io.h>

int xbt_fseek64(FILE *stream, int64_t offset, int whence)
{
    return _fseeki64(stream, (__int64)offset, whence);
}
#endif
