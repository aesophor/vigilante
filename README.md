<div align="center">
<h3>VIGILANTE</h3>
<img src="/Documentation/images/explore3.png">
</div>

## Overview
Vigilante is a 2D side-scrolling single-player RPG which offers gaming experience similar to Maplestory, Castlevania and The Elder Scrolls Series. Vigilante features an intriguing vampire storyline, allowing the player to grow from a nobody eventually to a vampiric lord. It also provides plenty of maps, enemies, magics, and equipments to discover.

Similar to The Elder Scrolls series, the player can team up with certain NPCs and go adventure together. The player can either take side with the vampire hunters and save the world from the vampires' plot, or join the vampires and bring destruction to the world. The storyline will be tailored based on the player's playstyle.

## Copyright Notice
This is a commercial project, so everything under the `Resources` directory is kept private, which means you'll not be able to run this game.

I've always been wondering how a large-scale video game can be made from scratch in C++ ever since I was a kid, but I've never found any good example on the Internet. Hence, I open-sourced a part of this project with the aim of helping those who are looking for such an example (although you'll not be able to run it due to lack of `Resources`).

## Build and Run

#### Build requirements
* C++20
* [axmol](https://github.com/axmolengine/axmol)

#### Building and running vigilante using Xcode
1. Generate xcode project `cmake -S . -B build -GXcode -DCMAKE_OSX_ARCHITECTURES=arm64`
2. Launch xcode, open the project and build

## License
Copyright (c) 2018-2024 Marco Wang \<m.aesophor@gmail.com\>. All rights reserved.

* You are allowed to:
  - edit and modify the codebase for educational purposes;
* You are NOT allowed to:
  - edit and modify the codebase for commercial purposes;
  - Resell it, original or modified;
