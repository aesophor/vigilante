// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "OptionPane.h"

#include <vector>

#include "std/make_unique.h"
#include "input/InputManager.h"

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

  // Define available Options.
  _options = {{
    {"Save Game", []() {}},
    {"Load Game", []() {}},
    {"Options",   []() {}},
    {"Quit",      []() {}}
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
  auto inputMgr = InputManager::getInstance();

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _optionListView->selectUp();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _optionListView->selectDown();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {
    _optionListView->confirm();
  }
}

} // namespace vigilante
