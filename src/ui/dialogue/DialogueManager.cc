// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueManager.h"

using namespace std;
USING_NS_CC;

namespace vigilante {

DialogueManager::DialogueManager()
    : _layer(Layer::create()),
      _subtitles(std::make_unique<Subtitles>()),
      _dialogueMenu(std::make_unique<DialogueMenu>()),
      _latestNpcDialogueTree() {
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

Dialogue* DialogueManager::getCurrentDialogue() const {
  return (_targetNpc) ? _targetNpc->getDialogueTree().getCurrentNode() : nullptr;
}

void DialogueManager::setCurrentDialogue(Dialogue* dialogue) const {
  if (!_targetNpc) {
    return;
  }
  _targetNpc->getDialogueTree().setCurrentNode(dialogue);
}

string DialogueManager::getLatestNpcDialogueTree(const string& npcJsonFileName) {
  auto it = _latestNpcDialogueTree.find(npcJsonFileName);
  return (it != _latestNpcDialogueTree.end()) ? it->second : "";
}

void DialogueManager::setLatestNpcDialogueTree(const string& npcJsonFileName,
                                               const string& dialogueTreeJsonFileName) {
  _latestNpcDialogueTree[npcJsonFileName] = dialogueTreeJsonFileName;
}

}  // namespace vigilante
