// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIALOGUE_LISTVIEW_H_
#define VIGILANTE_DIALOGUE_LISTVIEW_H_

#include "gameplay/DialogueTree.h"
#include "ui/ListView.h"

namespace vigilante {

class DialogueListView : ListView<DialogueTree*> {
 public:
  DialogueListView();
  virtual ~DialogueListView() = default;

  virtual void confirm() override; // ListView<DialogueTree*>
  
  void showDialogue(DialogueTree* dialogueTree);

 private:
  DialogueTree* _dialogueTree;
};

} // namespace vigilante

#endif // VIGILANTE_DIALOGUE_LISTVIEW_H_
