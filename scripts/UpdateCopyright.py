#!/usr/bin/env python3
# -*- encoding: utf-8 -*-

from asyncio.queues import Queue
import sys
import os

copyright_line = '// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.\n'

def maybe_rewrite_copyright_line(path):
    with open(path, 'r') as f:
        lines = f.readlines()
        lines[0] = copyright_line if lines[0].startswith('// Copyright (c)') else lines[0]

    with open(path, 'w') as f:
        f.writelines(lines)

def usage():
    return 'usage: {} <src_directory>'.format(sys.argv[0])

def main():
    if len(sys.argv) < 2 or not os.path.isdir(sys.argv[1]):
        print(usage())
        sys.exit(0)

    q = Queue()
    q.put_nowait(sys.argv[1])

    while not q.empty():
        dir = q.get_nowait()
        for dentry in os.listdir(dir):
            path = os.path.join(dir, dentry)

            if os.path.isdir(path):
                q.put_nowait(path)
            else:
                maybe_rewrite_copyright_line(path)


if __name__ == '__main__':
    main()
