// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "DialogueManager.h"

using namespace std;
USING_NS_AX;

namespace requiem {

DialogueManager::DialogueManager()
    : _layer{Layer::create()},
      _subtitles{std::make_unique<Subtitles>()},
      _dialogueMenu{std::make_unique<DialogueMenu>()} {
  _layer->addChild(_subtitles->getLayer());
  _layer->addChild(_dialogueMenu->getLayer());
}

void DialogueManager::update(const float delta) {
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

optional<string> DialogueManager::getLatestNpcDialogueTree(const string& npcJsonFilePath) {
  auto it = _latestNpcDialogueTrees.find(npcJsonFilePath);
  return it != _latestNpcDialogueTrees.end() ? optional<string>{it->second} : nullopt;
}

void DialogueManager::setLatestNpcDialogueTree(const string& npcJsonFilePath,
                                               const string& dialogueTreeJsonFilePath) {
  _latestNpcDialogueTrees[npcJsonFilePath] = dialogueTreeJsonFilePath;
}

void DialogueManager::clearLatestNpcDialogueTree(const string& npcJsonFilePath) {
  _latestNpcDialogueTrees[npcJsonFilePath] = "";
}

}  // namespace requiem
