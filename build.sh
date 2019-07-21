# Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#!/usr/bin/env sh

project_root="/home/aesophor/Code/Vigilante/"
spritesheets_list="Resources/Texture/spritesheets.txt"
quests_list="Resources/Gameplay/quests_list.txt"

# Generate Resources/Texture/spritesheets.txt
cd $project_root/Resources && find Texture -type f | grep plist > $project_root/$spritesheets_list

# Generate Resources/Gameplay/quest_list.txt
cd $project_root && find Resources -type f | grep quest | grep json > $project_root/$quests_list

cocos run -p linux
