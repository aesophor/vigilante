// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_CONSOLE_CONSOLE_H_
#define REQUIEM_UI_CONSOLE_CONSOLE_H_

#include <deque>
#include <string>

#include <axmol.h>

#include "ui/TextField.h"
#include "ui/console/CommandHandler.h"
#include "util/ds/CircularBuffer.h"

namespace requiem {

class Console final {
 public:
  Console();

  virtual void update(const float delta);
  virtual bool executeCmd(const std::string& cmd,
                          bool showNotification=false,
                          bool saveInHistory=false);

  bool isVisible() const;
  void setVisible(bool visible);

  inline ax::Layer* getLayer() const { return _layer; }

 private:
  class CommandHistory final : public CircularBuffer<std::string> {
    friend class Console;

   public:
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

  ax::Layer* _layer;

  TextField _textField;
  CommandHandler _cmdHandler;
  CommandHistory _cmdHistory;
};

} // namespace requiem

#endif // REQUIEM_UI_CONSOLE_CONSOLE_H_
