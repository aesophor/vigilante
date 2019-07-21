// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_MENU_H_
#define VIGILANTE_DIALOGUE_MENU_H_

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UIImageView.h>

namespace vigilante {

class DialogueMenu {
 public:
  DialogueMenu();
  virtual ~DialogueMenu() = default;

 private:
  cocos2d::ui::ImageView* _background;
};

} // namespace vigilante

#endif // VIGILANTE_DIALOGUE_MENU_H_
