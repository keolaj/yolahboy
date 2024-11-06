Package: sdl2:x64-windows@2.0.8-3

**Host Environment**

- Host: x64-windows
- Compiler: MSVC 19.35.32216.1
-    vcpkg-tool version: 2024-10-18-e392d7347fe72dff56e7857f7571c22301237ae6
    vcpkg-scripts version: 7fd612ee1 2024-11-05 (11 hours ago)

**To Reproduce**

`vcpkg install `

**Failure logs**

```
CMake Warning at scripts/cmake/vcpkg_common_functions.cmake:3 (message):
  vcpkg_common_functions has been removed and all values are automatically
  provided in all portfile.cmake invocations.  Please remove
  `include(vcpkg_common_functions)`.
Call Stack (most recent call first):
  buildtrees/versioning_/versions/sdl2/07c207585512ac07014d6ddf4f3111a911046857/portfile.cmake:1 (include)
  scripts/ports.cmake:192 (include)


-- Using cached SDL-Mirror-SDL-release-2.0.8.tar.gz.
-- Extracting source D:/vcpkg/downloads/SDL-Mirror-SDL-release-2.0.8.tar.gz
-- Applying patch export-symbols-only-in-shared-build.patch
-- Applying patch enable-winrt-cmake.patch
CMake Error at scripts/cmake/z_vcpkg_apply_patches.cmake:34 (message):
  Applying patch failed: Checking patch CMakeLists.txt...

  error: while searching for:

    ${SDL2_SOURCE_DIR}/src/timer/*.c
    ${SDL2_SOURCE_DIR}/src/video/*.c)

  



  if(ASSERTIONS STREQUAL "auto")

    # Do nada - use optimization settings to determine the assertion level

  elseif(ASSERTIONS STREQUAL "disabled")



  error: patch failed: CMakeLists.txt:358

  error: CMakeLists.txt: patch does not apply

  Checking patch include/SDL_config.h.cmake...

  Hunk #1 succeeded at 312 (offset 14 lines).

  Hunk #2 succeeded at 381 (offset 15 lines).

  Hunk #3 succeeded at 404 (offset 15 lines).

Call Stack (most recent call first):
  scripts/cmake/vcpkg_extract_source_archive.cmake:147 (z_vcpkg_apply_patches)
  scripts/cmake/vcpkg_extract_source_archive_ex.cmake:8 (vcpkg_extract_source_archive)
  scripts/cmake/vcpkg_from_github.cmake:113 (vcpkg_extract_source_archive_ex)
  buildtrees/versioning_/versions/sdl2/07c207585512ac07014d6ddf4f3111a911046857/portfile.cmake:3 (vcpkg_from_github)
  scripts/ports.cmake:192 (include)



```

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "dependencies": [
    "sdl2"
  ],
  "builtin-baseline": "7fd612ee1c57192d49028ed6a922e5e8bdb257f3",
  "overrides": [
    {
      "name": "sdl2",
      "version": "2.0.8-3"
    }
  ]
}

```
</details>
