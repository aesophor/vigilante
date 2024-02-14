// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "OptionPane.h"

#include <vector>

#include "gameplay/GameState.h"
#include "input/InputManager.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr int kMaxOptionCount = 4;

}  // namespace

OptionPane::OptionPane(PauseMenu* pauseMenu)
    : AbstractPane{pauseMenu},
      _options{kMaxOptionCount},
      _optionListView{std::make_unique<OptionListView>(pauseMenu)} {
  _layout->setLayoutType(ui::Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)

  // Place item list view.
  _optionListView->getLayout()->setPosition({5, -5});
  _layout->addChild(_optionListView->getLayout());

  // Define available Options.
  _options = {{
    {"Save Game", []() { GameState("quicksave.vgs").save(); }},
    {"Load Game", []() { GameState("quicksave.vgs").load(); }},
    {"Options",   []() {}},
    {"Quit",      []() { SceneManager::the().getCurrentScene<GameScene>()->setRunning(false); }},
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

}  // namespace vigilante
