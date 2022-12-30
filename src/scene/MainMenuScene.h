// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_MAIN_MENU_SCENE_H_
#define VIGILANTE_MAIN_MENU_SCENE_H_

#include <array>
#include <string>
#include <string_view>
#include <vector>

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

  static inline constexpr std::array<std::string_view, MainMenuScene::Option::SIZE> _kOptionStr = {{
    "New Game",
    "Load Game",
    "Options",
    "Exit",
  }};
  static inline constexpr const char* _kCopyrightStr = "© 2018-2023 Aesophor Softworks";
  static inline constexpr const char* _kVersionStr = "0.1.0";
  static inline constexpr int _kMenuOptionGap = 20;
  static inline constexpr int _kFooterLabelPadding = 10;

  cocos2d::ui::ImageView* _background;
  std::vector<cocos2d::Label*> _labels;
  int _current;
};

}  // namespace vigilante

#endif  // VIGILANTE_MAIN_MENU_SCENE_H_
