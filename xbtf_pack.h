/**
 * @file xbtf_pack.h
 * @brief Pack: build .xbt from a directory of PNG/JPEG (and optionally GIF) images.
 * Kodi 17+ / Omega 21: A8R8G8B8/RGB8, LZO-compressed (or stored uncompressed if not smaller).
 * Part of kodixbttool (C) 2026 Sunny C.
 */
#ifndef XBTF_PACK_H
#define XBTF_PACK_H

/**
 * @brief Pack a directory of images into a Kodi .xbt file (XBT v2, A8R8G8B8/RGB8).
 * @param input_dir    Directory containing PNG/JPEG (and optionally GIF) images
 * @param output_path  Path of the output .xbt file
 * @param dupecheck    Non-zero to store identical image data only once
 * @param remove_input Non-zero to delete input_dir after a successful pack (default behaviour)
 * @return 0 on success, non-zero on error
 */
int xbtf_pack(const char *input_dir, const char *output_path, int dupecheck, int remove_input);

#endif
