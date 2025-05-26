// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_DIALOGUE_DIALOGUE_MENU_H_
#define REQUIEM_UI_DIALOGUE_DIALOGUE_MENU_H_

#include <memory>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UIImageView.h>
#include "Controllable.h"
#include "ui/dialogue/DialogueListView.h"

namespace requiem {

class Npc;

class DialogueMenu final : public Controllable {
 public:
  DialogueMenu();
  virtual ~DialogueMenu() = default;

  virtual void handleInput() override; // Controllable

  ax::Layer* getLayer() const { return _layer; }
  DialogueListView* getDialogueListView() const { return _dialogueListView.get(); }

 private:
  ax::Layer* _layer{};

  ax::ui::ImageView* _background{};
  std::unique_ptr<DialogueListView> _dialogueListView;
};

}  // namespace requiem

#endif  // REQUIEM_UI_DIALOGUE_DIALOGUE_MENU_H_
