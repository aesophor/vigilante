// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CONSOLE_H_
#define VIGILANTE_CONSOLE_H_

namespace vigilante {

class Console {
 public:
  static Console* getInstance();
  virtual ~Console() = default;

 private:
  static Console* _instance;
  Console();
};

} // namespace vigilante

#endif // VIGILANTE_CONSOLE_H_
