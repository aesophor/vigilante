// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "MiscItem.h"

namespace fs = std::filesystem;
using namespace std;

namespace requiem {

MiscItem::MiscItem(const fs::path& jsonFilePath) : Item{jsonFilePath} {}

}  // namespace requiem
