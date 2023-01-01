// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CONSOLE_H_
#define VIGILANTE_CONSOLE_H_

#include <deque>
#include <string>

#include <cocos2d.h>

#include "ui/TextField.h"
#include "ui/console/CommandHandler.h"
#include "util/ds/CircularBuffer.h"

namespace vigilante {

class Console {
 public:
  static Console* getInstance();
  virtual ~Console() = default;

  virtual void update(float delta);
  virtual void executeCmd(const std::string& cmd,
                          bool showNotification=false,
                          bool saveInHistory=false);

  bool isVisible() const;
  void setVisible(bool visible);

  inline cocos2d::Layer* getLayer() const { return _layer; }

 private:
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
  CommandHandler _cmdHandler;
  CommandHistory _cmdHistory;
};

} // namespace vigilante

#endif // VIGILANTE_CONSOLE_H_
