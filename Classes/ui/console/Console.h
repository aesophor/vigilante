// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CONSOLE_H_
#define VIGILANTE_CONSOLE_H_

#include <string>
#include <memory>

#include <cocos2d.h>
#include "Controllable.h"
#include "ui/TextField.h"
#include "ui/console/CommandParser.h"

namespace vigilante {

class Console : public Controllable {
 public:
  static Console* getInstance();
  virtual ~Console() = default;

  virtual void update(float delta);
  virtual void handleInput() override;
  virtual void executeCmd(const std::string& cmd);

  cocos2d::Layer* getLayer() const;

 private:
  static Console* _instance;
  Console();

  cocos2d::Layer* _layer;
  std::unique_ptr<TextField> _textField;

  CommandParser _commandParser;
};

} // namespace vigilante

#endif // VIGILANTE_CONSOLE_H_
