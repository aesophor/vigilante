// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CONSOLE_H_
#define VIGILANTE_CONSOLE_H_

#include <deque>
#include <string>

#include <cocos2d.h>
#include "Controllable.h"
#include "ui/TextField.h"
#include "ui/console/CommandParser.h"
#include "util/CircularBuffer.h"

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

  class CommandHistory : public CircularBuffer<std::string> {
   public:
    friend class Console;
    CommandHistory();
    virtual ~CommandHistory() = default;

    bool canGoBack() const;
    bool canGoForward() const;
    void goBack();
    void goForward();
    const std::string& getCurrentLine() const;

   private:
    int _current;
  };

  cocos2d::Layer* _layer;

  TextField _textField;
  CommandParser _cmdParser;
  CommandHistory _cmdHistory;
};

} // namespace vigilante

#endif // VIGILANTE_CONSOLE_H_
