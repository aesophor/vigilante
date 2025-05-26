// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_SCENE_MAIN_MENU_SCENE_H_
#define REQUIEM_SCENE_MAIN_MENU_SCENE_H_

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UIImageView.h>

#include "Controllable.h"
#include "input/InputManager.h"

namespace requiem {

class MainMenuScene final : public ax::Scene, public Controllable {
 public:
  CREATE_FUNC(MainMenuScene);
  virtual ~MainMenuScene() = default;

  virtual bool init() override; // ax::Scene
  virtual void update(const float delta) override; // ax::Scene
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
  static inline constexpr const char* _kCopyrightStr = "© 2018-2025 Aesophor Softworks";
  static inline constexpr const char* _kVersionStr = "0.3.0";
  static inline constexpr int _kMenuOptionGap = 20;
  static inline constexpr int _kFooterLabelPadding = 10;

  ax::ui::ImageView* _background;
  std::vector<ax::Label*> _labels;
  int _current;
};

}  // namespace requiem

#endif  // REQUIEM_SCENE_MAIN_MENU_SCENE_H_
