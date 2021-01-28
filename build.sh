# Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#!/usr/bin/env sh

# Add environment variable COCOS_CONSOLE_ROOT for cocos2d-x
export COCOS_CONSOLE_ROOT="$HOME/Code/vigilante/cocos2d-x-3.17.1/tools/cocos2d-console/bin"
export PATH=$COCOS_CONSOLE_ROOT:$PATH

# Add environment variable COCOS_TEMPLATES_ROOT for cocos2d-x
export COCOS_TEMPLATES_ROOT="$HOME/Code/vigilante/cocos2d-x-3.17.1/templates"
export PATH=$COCOS_TEMPLATES_ROOT:$PATH

proj_root="$HOME/Code/vigilante/"
spritesheets_list="Resources/Texture/spritesheets.txt"
quests_list="Resources/Gameplay/quests_list.txt"

# Generate Resources/Texture/spritesheets.txt
cd $proj_root/Resources && find Texture -type f | grep plist > $proj_root/$spritesheets_list

# Generate Resources/Gameplay/quest_list.txt
cd $proj_root/Resources && find . -type f | grep quest | grep json > $proj_root/$quests_list

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  cocos compile -p linux
elif [[ "$OSTYPE" == "darwin"* ]]; then
  xcodebuild -project $proj_root/proj.ios_mac/Vigilante.xcodeproj\
    -configuration Debug \
    -arch x86_64 \
    -target "Vigilante-desktop" \
    CONFIGURATION_BUILD_DIR="$proj_root/bin/debug/mac"
else
  echo 'Unsupported platform'
fi
