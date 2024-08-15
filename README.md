# Red 11 Engine

Personal 2D/3D game engine I'm creating for development of my indie projects. Primary renderer uses DirectX 9.0c.

### Features

- PBR rendering
- Lights (Omni, Directional and Spot)
- Shadows (Directional and Spot only)
- Simple Physics (Capsule, AABB, Mesh, Convex, Sphere)
- UI system
- Support for variety of retro hardware
- Works on Windows 7 and later, Windows XP currently unsupported (Work in progress, problems with multithreading)
- No additional libraries required to build aside of standard C++ ones

# Compilation

1. Clone the repo, install make tools and clang++
2. Run make in root directory

# Usage

### Windows

Link rtengine.lib with your program and put rtengine.dll into your bin folder.
You can just reproduce the same building process as the examples.

### How to learn

The easiest and currently the only way is to go through the examples. 
They have comments to explain how to do most of the things the engine capable of and to understand it's architecture

### How engine is being developed

I have a backlog of features I want to implement. I select them mostly by my current needs of the projects I'm working on.
