<div align="center">
<h3>VIGILANTE</h3>
<img src="/Documentation/images/cover.png">
</div>

## Overview
Vigilante is a 2D side-scrolling single-player RPG which offers gaming experience similar to Maplestory, Castlevania and The Elder Scrolls Series. Vigilante features an intriguing vampire storyline, allowing the player to grow from a nobody eventually to a vampiric lord. It also provides plenty of maps, enemies, magics, and equipments to discover.

Similar to The Elder Scrolls series, the player can team up with certain NPCs and go adventure together. The player can either take side with the vampire hunters and save the world from the vampires' plot, or join the vampires and bring destruction to the world. The storyline will be tailored based on the player's playstyle.

## Build and Run

#### Build requirements
* C++17
* [cocos2d-x-3.17.1](https://cocos2d-x.org/filedown/cocos2d-x-3.17.1) [[bak](https://drive.google.com/file/d/1o4RbJePm_2LGuLNNnIoiklDPvjqT3B3V/view?usp=share_link)]

#### Building and running vigilante using build script
```sh
git clone https://github.com/aesophor/vigilante --recurse-submodules -j8
cd vigilante
# `setup.sh` downloads cocos2d-x-3.17.1.zip
./setup.sh
# `build.sh` contains several paths that must be edited before you run it.
./build.sh
```

#### Building and running vigilante using Xcode
* Launch Xcode
* Open the project: `proj.ios_mac/Vigilante.xcodeproj`
* Build

## License
Copyright (c) 2018-2023 Marco Wang \<m.aesophor@gmail.com\>. All rights reserved.

* You are allowed to:
  - edit and modify the codebase for educational purposes;
* You are NOT allowed to:
  - edit and modify the codebase for commercial purposes;
  - Resell it, original or modified;
