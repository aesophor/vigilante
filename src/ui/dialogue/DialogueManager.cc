// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueManager.h"

#include "std/make_unique.h"

using std::unique_ptr;
using cocos2d::Layer;

namespace vigilante {

DialogueManager* DialogueManager::getInstance() {
  static DialogueManager instance;
  return &instance;
}

DialogueManager::DialogueManager()
    : _layer(Layer::create()),
      _subtitles(std::make_unique<Subtitles>()),
      _dialogueMenu(std::make_unique<DialogueMenu>()) {
  _layer->addChild(_subtitles->getLayer());
  _layer->addChild(_dialogueMenu->getLayer());
}


void DialogueManager::update(float delta) {
  _subtitles->update(delta);
}

void DialogueManager::handleInput() {
  if (_dialogueMenu->getLayer()->isVisible()) {
    _dialogueMenu->handleInput();
  } else if (_subtitles->getLayer()->isVisible()) {
    _subtitles->handleInput();
  }
}


Layer* DialogueManager::getLayer() const {
  return _layer;
}

Npc* DialogueManager::getTargetNpc() const {
  return _targetNpc;
}

Subtitles* DialogueManager::getSubtitles() const {
  return _subtitles.get();
}

DialogueMenu* DialogueManager::getDialogueMenu() const {
  return _dialogueMenu.get();
}

Dialogue* DialogueManager::getCurrentDialogue() const {
  return (_targetNpc) ? _targetNpc->getDialogueTree().getCurrentNode() : nullptr;
}


void DialogueManager::setTargetNpc(Npc* npc) {
  _targetNpc = npc;
}

void DialogueManager::setCurrentDialogue(Dialogue* dialogue) const {
  if (!_targetNpc) {
    return;
  }
  _targetNpc->getDialogueTree().setCurrentNode(dialogue);
}

}  // namespace vigilante
