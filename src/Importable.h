// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_IMPORTABLE_H_
#define VIGILANTE_IMPORTABLE_H_

#include <string>

namespace vigilante {

class Importable {
 public:
  virtual ~Importable() = default;
  virtual void import(const std::string& jsonFileName) = 0;
};

}  // namespace vigilante

#endif  // VIGILANTE_IMPORTABLE_H_
