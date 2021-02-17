// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueMenu.h"

#include "std/make_unique.h"
#include "AssetManager.h"
#include "character/Npc.h"
#include "input/InputManager.h"
#include "ui/dialogue/DialogueManager.h"

// The positionX of DialogueMenu will be updated dynamically in runtime.
// See DialogueMenu::updatePosition() and ui/dialogue/Subtitles.cc
#define DIALOGUE_MENU_BG_POS {0, 63}
#define DIALOGUE_MENU_POS {0, 60}

using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::EventKeyboard;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using vigilante::asset_manager::kDialogueMenuBg;

namespace vigilante {

DialogueMenu::DialogueMenu()
    : _layer(Layer::create()),
      _background(ImageView::create(kDialogueMenuBg)),
      _dialogueListView(std::make_unique<DialogueListView>(this)) {
  _background->setAnchorPoint({0, 1});  // make top-left (0, 0)
  _background->setPosition(DIALOGUE_MENU_BG_POS);
  _layer->addChild(_background, 0);

  _dialogueListView->getLayout()->setAnchorPoint({0, 0});
  _dialogueListView->getLayout()->setPosition(DIALOGUE_MENU_POS);
  _layer->addChild(_dialogueListView->getLayout());
  _layer->setVisible(false);
}


void DialogueMenu::handleInput() {
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _dialogueListView->selectUp();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _dialogueListView->selectDown();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
    _dialogueListView->confirm();
  }
}


Layer* DialogueMenu::getLayer() const {
  return _layer;
}

DialogueListView* DialogueMenu::getDialogueListView() const {
  return _dialogueListView.get();
}

}  // namespace vigilante
