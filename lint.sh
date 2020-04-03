# Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#!/usr/bin/env sh

# Run clang-format
clang-format -i Classes/**/*.{cc,h}

# Update copyright line
year_begin=2018
year_current=`date +"%Y"`
copyright="\/\/ Copyright (c) ${year_begin}-${year_current} Marco Wang <m.aesophor@gmail.com>.\
 All rights reserved."

sed -i "1s/.*/${copyright}/" Classes/*.{cc,h}
