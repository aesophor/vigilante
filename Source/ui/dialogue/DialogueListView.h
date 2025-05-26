// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_DIALOGUE_DIALOGUE_LISTVIEW_H_
#define REQUIEM_UI_DIALOGUE_DIALOGUE_LISTVIEW_H_

#include "gameplay/DialogueTree.h"
#include "ui/ListView.h"

namespace requiem {

// Forward declaration
class DialogueMenu;

// using Dialogue = DialogueTree::Node
// See gameplay/DialogueTree.h for this alias.
using Dialogue = DialogueTree::Node;

class DialogueListView final : public ListView<Dialogue*> {
 public:
  explicit DialogueListView(DialogueMenu* dialogueMenu);
  virtual ~DialogueListView() = default;

  virtual void confirm() override;  // ListView<Dialogue*>

  // Update the position and size of DialogueListView
  // See ui/dialogue/DialogueMenu.cc and ui/dialogue/Subtitles.cc
  void updatePosition();

 private:
  DialogueMenu* _dialogueMenu;
};

}  // namespace requiem

#endif  // REQUIEM_UI_DIALOGUE_DIALOGUE_LISTVIEW_H_
