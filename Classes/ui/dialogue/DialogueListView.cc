// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueListView.h"

#define VISIBLE_ITEM_COUNT 3
#define WIDTH vigilante::kVirtualWidth / 2
#define REGULAR_BG vigilante::asset_manager::kEmptyImage
#define HIGHLIGHTED_BG vigilante::asset_manager::kEmptyImage

using cocos2d::Label;

namespace vigilante {

DialogueListView::DialogueListView(DialogueMenu* dialogMenu)
    : ListView<Dialogue*>(VISIBLE_ITEM_COUNT, WIDTH, REGULAR_BG, HIGHLIGHTED_BG),
      _dialogueMenu(dialogMenu),
      _dialogueTree() {

  this->_onSelect = [](ListView::ListViewItem* listViewItem, Dialogue* dialogue) {
    listViewItem->getLabel()->setString(dialogue->dialogue); // FIXME: rename this shit...?
  };
}


void DialogueListView::confirm() {

}

void DialogueListView::showDialogue(DialogueTree* dialogueTree) {
  _dialogueTree = dialogueTree;
}

} // namespace vigilante
