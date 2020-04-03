// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "OptionPane.h"

#include <vector>

#include "input/InputManager.h"

#define OPTIONS_COUNT 4

using std::vector;
using std::unique_ptr;
using cocos2d::ui::Layout;
using cocos2d::EventKeyboard;

namespace vigilante {

OptionPane::OptionPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _optionListView(new OptionListView(pauseMenu)) {

  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)

  // Place item list view.
  _optionListView->getLayout()->setPosition({5, -5});
  _layout->addChild(_optionListView->getLayout());

  // Define available Options.
  _options = {{
    unique_ptr<Option>(new Option({"Save Game", []() {}})),
    unique_ptr<Option>(new Option({"Load Game", []() {}})),
    unique_ptr<Option>(new Option({"Options",   []() {}})),
    unique_ptr<Option>(new Option({"Quit",      []() {}}))
  }};
 
  vector<Option*> options;
  for (const auto& o : _options) {
    options.push_back(o.get());
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
