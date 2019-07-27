// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueListView.h"

#include "AssetManager.h"
#include "gameplay/DialogueTree.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/dialogue/DialogueListView.h"

#define VISIBLE_ITEM_COUNT 3
#define WIDTH vigilante::kVirtualWidth / 2
#define REGULAR_BG vigilante::asset_manager::kEmptyImage
#define HIGHLIGHTED_BG vigilante::asset_manager::kEmptyImage

namespace vigilante {

DialogueListView::DialogueListView(DialogueMenu* dialogMenu)
    : ListView<Dialogue*>(VISIBLE_ITEM_COUNT, WIDTH, REGULAR_BG, HIGHLIGHTED_BG),
      _dialogueMenu(dialogMenu) {

  this->_setSelectedCallback = [](ListView::ListViewItem* listViewItem, bool selected) {
    listViewItem->getIcon()->loadTexture((selected) ? asset_manager::kDialogueTriangle : asset_manager::kEmptyImage);
  };

  this->_setObjectCallback = [](ListView::ListViewItem* listViewItem, Dialogue* dialogue) {
    listViewItem->getLabel()->setString(dialogue->text);
  };
}


void DialogueListView::confirm() {
  auto dialogueMgr = DialogueManager::getInstance();
  auto dialogueMenu = dialogueMgr->getDialogueMenu();
  auto subtitles = dialogueMgr->getSubtitles();
  
  Dialogue* nextDialogue = getSelectedObject()->children.front();
  subtitles->addSubtitle(nextDialogue->text);
  subtitles->showNextSubtitle();
  dialogueMenu->getLayer()->setVisible(false);

  dialogueMgr->setCurrentDialogue(nextDialogue);
}

} // namespace vigilante
