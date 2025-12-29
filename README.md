# Nuterm-GFX - Work in progress

__Nuterm-GFX__ is a retained-mode terminal GUI library written in C. It features a __multi-pass layout process__ (inspired by GTK) that relies on arena allocation, ensuring __high performance__. The library is built from the ground up with strong __modularity__ - Scene, Stage, Focuser, Renderer, Object, etc. are all implemented as abstract types, giving the user the ability to provide their own implementation and/or mix up different implementations. 
The codebase was intentionally written in an __OOP-inspired style__, but without heavy abstractions. This combination makes the process of adding new widgets both intuitive and straightforward.

It also provides native __UTF-32 support__ for correct handling of Unicode text, __asynchronous task execution__ on worker threads while maintaining __cross-terminal compatibility__.

## Dependencies

The library uses several small header-only libraries that are bundled internally, so the user does not need to manage them manually ([SArena](https://github.com/novak-stevanovic/sarena), [UConv](https://github.com/novak-stevanovic/uconv), [uthash](https://github.com/troydhanson/uthash)). 

In addition, it depends on [Nuterm](https://github.com/novak-stevanovic/nuterm), a terminal backend written in C. It is a core part of the system, acting as an abstraction layer for cross-terminal compatibility and providing the main loop used by the library. Build integration expects pkg-config to be available and uses it to obtain the required compiler and linker flags for nuterm via its .pc file.

## Makefile instructions:

1. `make [LIB_TYPE=so/ar] [OPT={0...3}]` - This will compile the source files(and thirdparty dependencies, if they exist) and build the library file.
2. `make install [LIB_TYPE=so/ar] [PREFIX={prefix}] [PC_PREFIX={pc_prefix}]` - This will place the public headers inside _{prefix}/include_ and the built library file inside _{prefix}/lib_.
This will also place the .pc file inside _{pc_prefix}_.

Default options are `PREFIX=/usr/local`, `OPT=2`, `LIB_TYPE=so`, `PC_PREFIX=/usr/local/lib/pkgconfig`.

## Usage instructions:

To use the library in your project, you must first install it. This can be done on your system - globally, or locally, inside a project that is using this library.
1. Install with desired `PREFIX` and `PC_PREFIX`.
2. Compile your project with cflags: `pkgconf --cflags nuterm-gfx` and link with flags: `pkgconf --libs nuterm-gfx`. For this to work, make sure that pkgconf seaches in the directory `PC_PREFIX` when using pkgconfig.
