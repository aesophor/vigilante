// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TEXT_FIELD_H_
#define VIGILANTE_TEXT_FIELD_H_

#include <string>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UILayout.h>
#include "Controllable.h"

namespace vigilante {

class TextField : public Controllable {
 public:
  TextField();
  virtual ~TextField() = default;

  virtual void update(float delta);
  virtual void handleInput() override;
  virtual void clear();

  cocos2d::ui::Layout* getLayout() const;

 private:
  virtual void toggleCursor();

  cocos2d::ui::Layout* _layout;
  cocos2d::Label* _label;
  std::string _buffer;

  float _timer;
  bool _isRecevingInput;
  bool _isCursorVisible;
};

} // namespace vigilante

#endif // VIGILANTE_TEXT_FIELD_H_
