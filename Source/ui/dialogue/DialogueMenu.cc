// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "DialogueMenu.h"

#include "Assets.h"
#include "character/Npc.h"
#include "gameplay/DialogueManager.h"
#include "input/InputManager.h"

using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

namespace {

// The positionX of DialogueMenu will be updated dynamically in runtime.
// See DialogueMenu::updatePosition() and ui/dialogue/Subtitles.cc
const Vec2 kDialogueMenuBgPos{0, 63};
const Vec2 kDialogueMenuPos{0, 60};

}  // namespace

DialogueMenu::DialogueMenu()
    : _layer{Layer::create()},
      _background{ui::ImageView::create(string{kDialogueMenuBg})},
      _dialogueListView{make_unique<DialogueListView>(this)} {
  _background->setAnchorPoint({0, 1});  // make top-left (0, 0)
  _background->setPosition(kDialogueMenuBgPos);
  _layer->addChild(_background, 0);

  _dialogueListView->getLayout()->setAnchorPoint({0, 0});
  _dialogueListView->getLayout()->setPosition(kDialogueMenuPos);
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

}  // namespace requiem
