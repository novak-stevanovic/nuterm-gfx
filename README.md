# Nuterm-GFX - Work in progress

__Nuterm-GFX__ is a retained-mode terminal GUI library written in C. It features a __multi-pass layout process__ (inspired by GTK) that relies on arena allocation, ensuring __high performance__. The library is built from the ground up with strong __modularity__ - Scene, Stage, Object, etc. are all implemented as abstract types, giving the user the ability to provide their own implementation and/or mix up different implementations. 
The codebase was intentionally written in an __OOP-inspired style__, but without heavy abstractions. This combination makes the process of adding new widgets straightforward.

It also provides __UTF-32 support__ for correct handling of Unicode text and __asynchronous task execution__ on worker threads while maintaining __cross-terminal compatibility__.

# Dependencies

The library is built on top of [Nuterm](https://github.com/novak-stevanovic/nuterm), which abstracts differences between terminal emulators and handles escape sequences, input events, and output buffering. Additionally, this library relies on [UConv](https://github.com/novak-stevanovic/uconv) for its UTF-32 conversion needs and [SArena](https://github.com/novak-stevanovic/sarena) for arena allocation. To fetch compile and link flags, pkg-config is used. This means that, when compiling the library, pkg-config must be able to locate corresponding .pc files.

The library also uses [uthash](https://github.com/troydhanson/uthash). That is bundled internally.

## Makefile instructions:

1. `make [PC_WITH_PATH=...] [LIB_TYPE=so/ar] [OPT={0...3}]` - This will compile the source files and build the library file. If the library depends on packages discovered via pkg-config, you can specify where to search for their .pc files, in addition to `PKG_CONFIG_PATH`.
2. `make install [LIB_TYPE=so/ar] [PREFIX=...] [PC_PREFIX=...]` - This will place the public headers inside `PREFIX/include` and the built library file inside `PREFIX/lib`. This will also place the .pc file inside `PC_PREFIX`.

Default options are `PREFIX=/usr/local`, `PC_PREFIX=PREFIX/lib/pkgconfig`, `OPT=2`, `LIB_TYPE=so`.

## Usage instructions:

This library can be used in your project via pkg-config. First, you need to install it on your system - either globally or locally in your project (so that the .pc file is available).

1. Install with desired `PREFIX` and `PC_PREFIX`.

2. Compile your project with flags: `$(pkgconf --cflags nutermgfx)` and link with flags: `$(pkgconf --libs nutermgfx)`. Make sure pkg-config can also find the .pc files of the libraries that are used via pkg-config.
