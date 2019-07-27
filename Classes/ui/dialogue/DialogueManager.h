// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_MANAGER_H_
#define VIGILANTE_DIALOGUE_MANAGER_H_

#include <memory>

#include "Controllable.h"
#include "character/Npc.h"
#include "ui/dialogue/Subtitles.h"
#include "ui/dialogue/DialogueMenu.h"

namespace vigilante {

class DialogueManager : public Controllable {
 public:
  static DialogueManager* getInstance();
  virtual ~DialogueManager() = default;

  void update(float delta);
  virtual void handleInput() override;

  Npc* getTargetNpc() const;
  Subtitles* getSubtitles() const;
  DialogueMenu* getDialogueMenu() const;
  Dialogue* getCurrentDialogue() const;

  void setTargetNpc(Npc* npc);
  void setCurrentDialogue(Dialogue* dialogue) const;

 private:
  static DialogueManager* _instance;
  DialogueManager();

  Npc* _targetNpc;
  std::unique_ptr<Subtitles> _subtitles;
  std::unique_ptr<DialogueMenu> _dialogueMenu;
};

} // namespace vigilante

#endif // VIGILANTE_DIALOGUE_MANAGER_H_
