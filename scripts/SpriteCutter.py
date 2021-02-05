#!/usr/bin/env python3
# -*- encoding: utf-8 -*-
#
# Description
# ===========
# This program will cut a single png into multiple png files.
# Example usage: ./SpriteCutter.py attack.png 8
#
# After running the program, there will be 8 new png files
# named 0.png, 1.png, 2.png, ..., 7.png
#
# Requirement
# ===========
# $ pip3 install --user Pillow

from PIL import Image
import os
import sys

def usage():
    return 'usage: {} <sprite.png> <frame_count>'.format(sys.argv[0])

def main():
    if len(sys.argv) < 3:
        print(usage())
        sys.exit(0)

    sprite_png_name = sys.argv[1]
    frame_count = sys.argv[2]

    img = Image.open(sprite_png_name)
    width, height = img.size
    crop_width = width // int(frame_count)

    save_dir_name = sprite_png_name.split('.')[0]
    if not os.path.isdir(save_dir_name):
        os.mkdir(save_dir_name)

    for i, x in enumerate(range(0, width, crop_width)):
        box = (x, 0, x + crop_width, height)
        img.crop(box).save('{}/{}.png'.format(save_dir_name, i))


if __name__ == '__main__':
    main()
