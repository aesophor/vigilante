// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueMenu.h"

#include "AssetManager.h"
#include "character/Npc.h"
#include "input/InputManager.h"
#include "ui/dialogue/DialogueManager.h"

#define DIALOGUE_MENU_Y 38

using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;

namespace vigilante {

DialogueMenu::DialogueMenu()
    : _layer(Layer::create()), _dialogueListView(new DialogueListView(this)) {
  auto winSize = Director::getInstance()->getWinSize();
  _dialogueListView->getLayout()->setPosition({winSize.width / 2, DIALOGUE_MENU_Y});
  _dialogueListView->getLayout()->setAnchorPoint({0.5, 1});
  _layer->addChild(_dialogueListView->getLayout());
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
