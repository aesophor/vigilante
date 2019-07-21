// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueListView.h"

#define VISIBLE_ITEM_COUNT 3
#define WIDTH vigilante::kVirtualWidth / 2
#define REGULAR_BG vigilante::asset_manager::kEmptyImage
#define HIGHLIGHTED_BG vigilante::asset_manager::kEmptyImage

namespace vigilante {

DialogueListView::DialogueListView()
    : ListView<DialogueTree*>(VISIBLE_ITEM_COUNT, WIDTH, REGULAR_BG, HIGHLIGHTED_BG),
      _dialogueTree() {}


void DialogueListView::confirm() {

}

void DialogueListView::showDialogue(DialogueTree* dialogueTree) {

}

} // namespace vigilante
