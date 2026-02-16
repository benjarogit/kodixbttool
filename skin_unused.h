/**
 * @file skin_unused.h
 * @brief Unused texture detection and removal for skin XBT pack workflow (cross-platform).
 * Part of kodixbttool (C) 2026 Sunny C.
 */
#ifndef SKIN_UNUSED_H
#define SKIN_UNUSED_H

#include <stddef.h>

#ifdef __cplusplus

#include <string>
#include <vector>

/** Default skin root from KODI_ADDONS or HOME/.kodi/addons/skin.dokukanal. */
std::string skin_unused_default_skin_root();

/**
 * Collect image paths (relative) under input_dir. Cross-platform.
 */
void skin_unused_collect_image_paths(const char *input_dir, std::vector<std::string> &out);

/**
 * Collect image references from all .xml files under skin_root (regex). Cross-platform.
 */
void skin_unused_collect_refs(const char *skin_root, std::vector<std::string> &refs_out);

/**
 * From texture_paths, compute those not in refs (by full path or basename).
 * If exclude_default true, paths starting with "default" are not marked unused.
 */
void skin_unused_compute_unused(const std::vector<std::string> &texture_paths,
                                const std::vector<std::string> &refs,
                                int exclude_default,
                                std::vector<std::string> &unused_out);

/** Delete files in input_dir with relative paths in unused_paths. Returns number removed. */
int skin_unused_remove_from_dir(const char *input_dir, const std::vector<std::string> &unused_paths);

/**
 * Prompt: "N unused textures. Remove before pack? (y/N) ". Returns true for yes.
 * If not a TTY, returns false. Cross-platform.
 */
bool skin_unused_prompt_remove(size_t unused_count);

#endif /* __cplusplus */

#endif /* SKIN_UNUSED_H */
