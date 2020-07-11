// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueMenu.h"

#include "std/make_unique.h"
#include "AssetManager.h"
#include "character/Npc.h"
#include "input/InputManager.h"
#include "ui/dialogue/DialogueManager.h"

#define DIALOGUE_MENU_Y 45

using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::EventKeyboard;

namespace vigilante {

DialogueMenu::DialogueMenu()
    : _layer(Layer::create()),
      _dialogueListView(std::make_unique<DialogueListView>(this)) {
  auto winSize = Director::getInstance()->getWinSize();
  _dialogueListView->getLayout()->setAnchorPoint({0.5, 1});
  _dialogueListView->getLayout()->setPosition({250, DIALOGUE_MENU_Y});

  _layer->addChild(_dialogueListView->getLayout());
  _layer->setVisible(false);
}


void DialogueMenu::handleInput() {
  auto inputMgr = InputManager::getInstance();

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _dialogueListView->selectUp();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _dialogueListView->selectDown();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {
    _dialogueListView->confirm();
  }
}


Layer* DialogueMenu::getLayer() const {
  return _layer;
}

DialogueListView* DialogueMenu::getDialogueListView() const {
  return _dialogueListView.get();
}

} // namespace vigilante
