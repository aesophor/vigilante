// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_IMPORTABLE_H_
#define REQUIEM_IMPORTABLE_H_

#include <filesystem>

namespace requiem {

class Importable {
 public:
  virtual ~Importable() = default;

  virtual void import(const std::filesystem::path& jsonFilePath) = 0;
};

}  // namespace requiem

#endif  // REQUIEM_IMPORTABLE_H_
