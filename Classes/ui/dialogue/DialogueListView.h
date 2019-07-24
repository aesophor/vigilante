// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_LISTVIEW_H_
#define VIGILANTE_DIALOGUE_LISTVIEW_H_

#include "gameplay/DialogueTree.h"
#include "ui/ListView.h"
#include "ui/dialogue/DialogueMenu.h"

namespace vigilante {

// using Dialogue = DialogueTree::Node
// See gameplay/DialogueTree.h for this alias.

class DialogueListView : ListView<Dialogue*> {
 public:
  DialogueListView(DialogueMenu* dialogueMenu);
  virtual ~DialogueListView() = default;

  virtual void confirm() override; // ListView<DialogueTree*>
  
  void showDialogue(DialogueTree* dialogueTree);

 private:
  DialogueMenu* _dialogueMenu;
  DialogueTree* _dialogueTree;
};

} // namespace vigilante

#endif // VIGILANTE_DIALOGUE_LISTVIEW_H_
