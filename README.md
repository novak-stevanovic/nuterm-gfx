# Nuterm-GFX - Work in progress

**Nuterm-GFX** is a retained-mode terminal GUI library written in C. It is built around a **multi-pass layout system** inspired by GTK, with arena-based memory management for **high performance**.

The library is designed with strong **modularity** and an **OOP-inspired architecture**, while avoiding heavy abstractions. This makes the codebase easier to extend and keeps the process of adding new widgets straightforward.

Nuterm-GFX also provides **UTF-32 text support**, **asynchronous task execution** through worker threads, and aims to maintain **cross-terminal compatibility**.

This project was developed entirely by me, including all of its dependencies.

# Dependencies

The library is built on top of [Nuterm](https://github.com/novak-stevanovic/nuterm), which abstracts differences between terminal emulators and handles escape sequences, input events, and output buffering. To fetch compile and link flags, pkg-config is used. This means that, when compiling the library, pkg-config must be able to locate the Nuterm's .pc file.

Additionally, this library relies on [UConv](https://github.com/novak-stevanovic/uconv) for its UTF-32 conversion needs, [SArena](https://github.com/novak-stevanovic/sarena) for arena allocation and [Gen-C](https://github.com/novak-stevanovic/genc) for generic containers. All of this is bundled internally.

## Makefile instructions:

1. `make [PC_WITH_PATH=...] [LIB_TYPE=so/ar] [OPT={0...3}]` - This will compile the source files and build the library file. If the library depends on packages discovered via pkg-config, you can specify where to search for their .pc files, in addition to `PKG_CONFIG_PATH`.
2. `make install [LIB_TYPE=so/ar] [PREFIX=...] [PC_PREFIX=...]` - This will place the public headers inside `PREFIX/include` and the built library file inside `PREFIX/lib`. This will also place the .pc file inside `PC_PREFIX`.

Default options are `PREFIX=/usr/local`, `PC_PREFIX=PREFIX/lib/pkgconfig`, `OPT=2`, `LIB_TYPE=so`.

## Usage instructions:

Compile your project with flags: `$(pkgconf --cflags nutermgfx)` and link with flags: `$(pkgconf --libs nutermgfx)`. For this to work, make sure that pkg-config searches in the directory of the .pc file generated in the installation process. See demo.c for usage examples.
