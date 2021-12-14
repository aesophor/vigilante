// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "OptionPane.h"

#include <vector>

#include "input/InputManager.h"
#include "scene/SceneManager.h"
#include "scene/GameScene.h"

#define OPTIONS_COUNT 4

using std::vector;
using std::unique_ptr;
using cocos2d::ui::Layout;
using cocos2d::EventKeyboard;

namespace vigilante {

OptionPane::OptionPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _options(OPTIONS_COUNT),
      _optionListView(std::make_unique<OptionListView>(pauseMenu)) {

  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)

  // Place item list view.
  _optionListView->getLayout()->setPosition({5, -5});
  _layout->addChild(_optionListView->getLayout());

  auto quit = []() {
    InputManager::getInstance()->deactivate();
    SceneManager::getInstance()->popScene();
    InputManager::getInstance()->activate(SceneManager::getInstance()->getCurrentScene());
  };

  // Define available Options.
  _options = {{
    {"Save Game", []() {}},
    {"Load Game", []() {}},
    {"Options",   []() {}},
    {"Quit",      quit   },
  }};
 
  vector<Option*> options;
  for (auto& o : _options) {
    options.push_back(&o);
  }
  _optionListView->setObjects(options); 
}


void OptionPane::update() {

}

void OptionPane::handleInput() {
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _optionListView->selectUp();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _optionListView->selectDown();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
    _optionListView->confirm();
  }
}

} // namespace vigilante
