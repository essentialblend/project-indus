# Project Indus

Indus is a multithreaded path-tracer. It is my first attempt at fleshing out and applying concepts I learn from various reference texts on graphics, programming, and math.
Presently, it serves as a revision playground which I aim to update immediately upon reaching a learning checkpoint.

Current Features:
- Developed from scratch using C++.
- Multithreading compatible.
- Per pixel MSAA.
- Jittered MSAA.
- Gamma Correction.
- Positionable camera with defocus blur (DOF).
- Lambertian, Metallic, and Dielectric Materials.

# Status

[Actively being re-written.](https://github.com/essentialblend/project-indus/issues) 

# Directory Structure
  - `src/` --
    Main source folder.

  - `src/headers` --
    Contains all in-use headers.

  - `src/headers/base` --
    Contains all base and utility classes.

  - `src/headers/materials` --
    Contains all derived material classes.

  - `src/headers/world_objects` --
    Contains all derived world-object classes.

Credit
--------------------
Third party libraries: 
- [Simple and Fast Multimedia Library (SFML)](https://www.sfml-dev.org/).
- [STB](https://github.com/nothings/stb).
