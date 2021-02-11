// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_MAIN_MENU_SCENE_H_
#define VIGILANTE_MAIN_MENU_SCENE_H_

#include <array>
#include <vector>
#include <string>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UIImageView.h>
#include "Controllable.h"
#include "input/InputManager.h"

namespace vigilante {

class MainMenuScene : public cocos2d::Scene, public Controllable {
 public:
  CREATE_FUNC(MainMenuScene);
  virtual ~MainMenuScene() = default;

  virtual bool init() override; // cocos2d::Scene
  virtual void update(float delta) override; // cocos2d::Scene
  virtual void handleInput() override; // Controllable

 private:
  enum Option {
    NEW_GAME,
    LOAD_GAME,
    OPTIONS,
    EXIT,
    SIZE
  };
  static const std::array<std::string, MainMenuScene::Option::SIZE> _kOptionStr;
  static const std::string _kCopyrightStr;
  static const std::string _kVersionStr;

  static const int _kMenuOptionGap;
  static const int _kFooterLabelPadding;

  cocos2d::ui::ImageView* _background;
  std::vector<cocos2d::Label*> _labels;
  int _current;
};

}  // namespace vigilante

#endif  // VIGILANTE_MAIN_MENU_SCENE_H_
