// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueListView.h"

#include "AssetManager.h"
#include "gameplay/DialogueTree.h"
#include "ui/console/Console.h"
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

  _setSelectedCallback = [](ListView::ListViewItem* listViewItem, bool selected) {
    listViewItem->getIcon()->loadTexture((selected) ? asset_manager::kDialogueTriangle : asset_manager::kEmptyImage);
  };

  _setObjectCallback = [](ListView::ListViewItem* listViewItem, Dialogue* dialogue) {
    listViewItem->getLabel()->setString(dialogue->lines.front());
  };

  hideScrollBar();
}


void DialogueListView::confirm() {
  auto dialogueMgr = DialogueManager::getInstance();
  auto dialogueMenu = dialogueMgr->getDialogueMenu();
  auto subtitles = dialogueMgr->getSubtitles();

  for (const auto& cmd : getSelectedObject()->cmds) {
    Console::getInstance()->executeCmd(cmd);
  }
  
  Dialogue* nextDialogue = getSelectedObject()->children.front();
  for (const auto& line : nextDialogue->lines) {
    subtitles->addSubtitle(line);
  }
  subtitles->showNextSubtitle();
  dialogueMenu->getLayer()->setVisible(false);

  dialogueMgr->setCurrentDialogue(nextDialogue);
}

} // namespace vigilante
