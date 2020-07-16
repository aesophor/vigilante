# Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#!/usr/bin/env sh

FILENAME="cocos2d-x-3.17.1.zip"

# Download cocos2d-x-3.17.1
if [ ! -f $FILENAME ]; then
  wget -O $FILENAME https://digitalocean.cocos2d-x.org/Cocos2D-X/$FILENAME
fi

unzip $FILENAME
