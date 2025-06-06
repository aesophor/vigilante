// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_GAMEPLAY_DIALOGUE_MANAGER_H_
#define REQUIEM_GAMEPLAY_DIALOGUE_MANAGER_H_

#include <memory>
#include <optional>

#include <axmol.h>

#include "Controllable.h"
#include "character/Npc.h"
#include "ui/dialogue/Subtitles.h"
#include "ui/dialogue/DialogueMenu.h"

namespace requiem {

class DialogueManager final : public Controllable {
 public:
  friend class GameState;

  DialogueManager();
  virtual ~DialogueManager() override = default;

  virtual void handleInput() override;  // Controllable

  void update(const float delta);

  inline ax::Layer* getLayer() const { return _layer; }
  inline Npc* getTargetNpc() const { return _targetNpc; }
  inline Subtitles* getSubtitles() const { return _subtitles.get(); }
  inline void setTargetNpc(Npc* npc) { _targetNpc = npc; }
  inline DialogueMenu* getDialogueMenu() const { return _dialogueMenu.get(); }

  Dialogue* getCurrentDialogue() const;
  void setCurrentDialogue(Dialogue* dialogue) const;

  std::optional<std::string> getLatestNpcDialogueTree(const std::string& npcJsonFilePath);
  void setLatestNpcDialogueTree(const std::string& npcJsonFilePath,
                                const std::string& dialogueTreeJsonFilePath);
  void clearLatestNpcDialogueTree(const std::string& npcJsonFilePath);

 private:
  ax::Layer* _layer;

  Npc* _targetNpc;
  std::unique_ptr<Subtitles> _subtitles;
  std::unique_ptr<DialogueMenu> _dialogueMenu;

  std::unordered_map<std::string, std::string> _latestNpcDialogueTrees;
};

}  // namespace requiem

#endif  // REQUIEM_GAMEPLAY_DIALOGUE_MANAGER_H_
