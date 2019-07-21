// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_MENU_H_
#define VIGILANTE_DIALOGUE_MENU_H_

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UIImageView.h>
#include "Controllable.h"

namespace vigilante {

class DialogueMenu : public Controllable {
 public:
  static DialogueMenu* getInstance();
  virtual ~DialogueMenu() = default;

  virtual void handleInput() override; // Controllable

  bool isVisible() const;
  void setVisible(bool visible);

  cocos2d::Layer* getLayer() const;

 private:
  static DialogueMenu* _instance;
  DialogueMenu();

  cocos2d::Layer* _layer;
};

} // namespace vigilante

#endif // VIGILANTE_DIALOGUE_MENU_H_
