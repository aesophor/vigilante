// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_MANAGER_H_
#define VIGILANTE_DIALOGUE_MANAGER_H_

#include <memory>

#include <cocos2d.h>
#include "Controllable.h"
#include "character/Npc.h"
#include "ui/dialogue/Subtitles.h"
#include "ui/dialogue/DialogueMenu.h"

namespace vigilante {

class DialogueManager : public Controllable {
 public:
  DialogueManager();
  virtual ~DialogueManager() override = default;

  virtual void handleInput() override;  // Controllable

  void update(float delta);

  inline cocos2d::Layer* getLayer() const { return _layer; }
  inline Npc* getTargetNpc() const { return _targetNpc; }
  inline Subtitles* getSubtitles() const { return _subtitles.get(); }
  inline void setTargetNpc(Npc* npc) { _targetNpc = npc; }
  inline DialogueMenu* getDialogueMenu() const { return _dialogueMenu.get(); }

  Dialogue* getCurrentDialogue() const;
  void setCurrentDialogue(Dialogue* dialogue) const;

  std::string getLatestNpcDialogueTree(const std::string& npcJsonFileName);
  void setLatestNpcDialogueTree(const std::string& npcJsonFileName,
                                const std::string& dialogueTreeJsonFileName);

 private:
  cocos2d::Layer* _layer;

  Npc* _targetNpc;
  std::unique_ptr<Subtitles> _subtitles;
  std::unique_ptr<DialogueMenu> _dialogueMenu;

  std::unordered_map<std::string, std::string> _latestNpcDialogueTree;
};

}  // namespace vigilante

#endif  // VIGILANTE_DIALOGUE_MANAGER_H_
