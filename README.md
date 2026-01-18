# Nuterm-GFX - Work in progress

__Nuterm-GFX__ is a retained-mode terminal GUI library written in C. It features a __multi-pass layout process__ (inspired by GTK) that relies on arena allocation for __high performance__. The library is built from the ground up with strong __modularity__ - Scene, Stage, Object, etc. are all implemented as abstract types, giving the user the ability to provide their own implementation. The codebase was intentionally written in an __OOP-inspired style__, but without heavy abstractions. This combination makes the process of adding new widgets straightforward.

It also provides __UTF-32 support__ for correct handling of Unicode text and __asynchronous task execution__ on worker threads while maintaining __cross-terminal compatibility__.

# Dependencies

The library is built on top of [Nuterm](https://github.com/novak-stevanovic/nuterm), which abstracts differences between terminal emulators and handles escape sequences, input events, and output buffering. To fetch compile and link flags, pkg-config is used. This means that, when compiling the library, pkg-config must be able to locate the Nuterm's .pc file.

Additionally, this library relies on [UConv](https://github.com/novak-stevanovic/uconv) for its UTF-32 conversion needs, [SArena](https://github.com/novak-stevanovic/sarena) for arena allocation, [Gen-C](https://github.com/novak-stevanovic/genc) for generic containers and [uthash](https://github.com/troydhanson/uthash) for a hashmap implementation. All of this is bundled internally.

## Makefile instructions:

1. `make [PC_WITH_PATH=...] [LIB_TYPE=so/ar] [OPT={0...3}]` - This will compile the source files and build the library file. If the library depends on packages discovered via pkg-config, you can specify where to search for their .pc files, in addition to `PKG_CONFIG_PATH`.
2. `make install [LIB_TYPE=so/ar] [PREFIX=...] [PC_PREFIX=...]` - This will place the public headers inside `PREFIX/include` and the built library file inside `PREFIX/lib`. This will also place the .pc file inside `PC_PREFIX`.

Default options are `PREFIX=/usr/local`, `PC_PREFIX=PREFIX/lib/pkgconfig`, `OPT=2`, `LIB_TYPE=so`.

## Usage instructions:

Compile your project with flags: `$(pkgconf --cflags nutermgfx)` and link with flags: `$(pkgconf --libs nutermgfx)`. For this to work, make sure that pkg-config searches in the directory of the .pc file generated in the installation process.
