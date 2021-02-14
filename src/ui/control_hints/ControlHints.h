// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CONTROL_HINTS_H_
#define VIGILANTE_CONTROL_HINTS_H_

#include <string>
#include <vector>
#include <unordered_map>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>
#include "ui/Colorscheme.h"

namespace vigilante {

class ControlHints {
 public:
  static ControlHints* getInstance();
  virtual ~ControlHints() = default;

  bool isShown(const cocos2d::EventKeyboard::KeyCode keyCode) const;

  void show(const cocos2d::EventKeyboard::KeyCode keyCode,
            const std::string& text,
            const cocos2d::Color4B& textColor=colorscheme::kWhite);

  void hide(const cocos2d::EventKeyboard::KeyCode keyCode);

  bool isVisible() const;
  void setVisible(bool visible);
  cocos2d::Layer* getLayer() const;

 private:
  class Hint final {
   public:
    Hint(const cocos2d::EventKeyboard::KeyCode keyCode,
         const std::string& text,
         const cocos2d::Color4B& textColor);
    ~Hint() = default;

    cocos2d::Size getContentSize() const;
    cocos2d::ui::Layout* getLayout() const;
    cocos2d::EventKeyboard::KeyCode getKeyCode() const;

   private:
    static const int _kIconLabelGap;

    cocos2d::ui::Layout* _layout;
    cocos2d::ui::ImageView* _icon;
    cocos2d::Label* _label;
    cocos2d::EventKeyboard::KeyCode _keyCode;
  };

  ControlHints();
  void normalize();

  static const int _kHintGap;

  cocos2d::Layer* _layer;
  std::vector<ControlHints::Hint> _hints;
};

}  // namespace vigilante

#endif  // VIGILANTE_CONTROL_HINTS_H_
