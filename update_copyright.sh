#!/usr/bin/env sh

# Update copyright
year_begin=2018
year_current=`date +"%Y"`
copyright="\/\/ Copyright (c) ${year_begin}-${year_current} Marco Wang <m.aesophor@gmail.com>.\
 All rights reserved."

find src -type f | xargs sed -i "1s/.*/${copyright}/"
