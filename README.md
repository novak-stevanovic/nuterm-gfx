# Nuterm-GFX - Work in progress

__Nuterm-GFX__ is a retained-mode terminal GUI library written in C. It implements a __multi-pass layout process__ (inspired by GTK) that uses an arena allocator, providing __great performance__. Core pieces are the the Object (base GUI component type), the Scene (drives the layout), the Scene Focuser (tracks focus and routes input/events to the focused objects), the Stage (combines per-object drawings into a single buffer, ready for rendering), the Renderer(outputs the resulting buffer onto the terminal screen), and the Taskmaster (allows for executing asynchronous tasks).

The library is built from the ground up with strong __modularity__: Scene, Stage, Focuser, Renderer, Object, etc. are all implemented as abstract types, giving the user the ability to provide their own implementation and/or mix up different implementations. The codebase was intentionally written in an OOP-inspired style, but without excessive use of OOP patterns, as heavy OOP abstractions are often inconvenient in C. This combination makes the process of adding new widgets intuitive and straightforward.

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
