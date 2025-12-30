# Nuterm-GFX - Work in progress

__Nuterm-GFX__ is a retained-mode terminal GUI library written in C. It features a __multi-pass layout process__ (inspired by GTK) that relies on arena allocation, ensuring __high performance__. The library is built from the ground up with strong __modularity__ - Scene, Stage, Object, etc. are all implemented as abstract types, giving the user the ability to provide their own implementation and/or mix up different implementations. 
The codebase was intentionally written in an __OOP-inspired style__, but without heavy abstractions. This combination makes the process of adding new widgets straightforward.

It also provides __UTF-32 support__ for correct handling of Unicode text and __asynchronous task execution__ on worker threads while maintaining __cross-terminal compatibility__ through a terminal backend abstraction.
