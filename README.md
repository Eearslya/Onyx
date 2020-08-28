# Onyx
Onyx is a work-in-progress 3D game engine written in C++.

The majority of this work is inspired or taken directly from [The Cherno's Tutorial Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT),
though I have tried my best to not copy/paste blindly and think critically about each section as it's made. There are parts where the Cherno
and I differ on opinion, and I have written my own implementation in its place.

I hope one day for this to be a semi-respectable engine capable of some neat games or 3D rendering demos.

## Building

This project is currently Windows-only, and 64-bit only. Building is done via Visual Studio 2019, and the solution files are created using [premake](https://premake.github.io/).

To create the solution files:
```
vendor\premake\bin\premake5.lua vs2019
```

This will create an `Onyx.sln` file in the root directory which you can then open and build using Visual Studio.