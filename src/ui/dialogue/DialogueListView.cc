// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueListView.h"

#include "AssetManager.h"
#include "Constants.h"
#include "gameplay/DialogueTree.h"
#include "ui/console/Console.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/dialogue/DialogueListView.h"

#define VISIBLE_ITEM_COUNT 3
#define WIDTH 0  /* The position of _scrollBar will be updated dynamically */
#define HEIGHT 53
#define ITEM_GAP_HEIGHT 15
#define REGULAR_BG vigilante::asset_manager::kEmptyImage
#define HIGHLIGHTED_BG vigilante::asset_manager::kEmptyImage

using cocos2d::Director;

namespace vigilante {

DialogueListView::DialogueListView(DialogueMenu* dialogMenu)
    : ListView<Dialogue*>(VISIBLE_ITEM_COUNT, WIDTH, HEIGHT, ITEM_GAP_HEIGHT, REGULAR_BG, HIGHLIGHTED_BG),
      _dialogueMenu(dialogMenu) {

  _setSelectedCallback = [](ListView::ListViewItem* listViewItem, bool selected) {
    listViewItem->getIcon()->loadTexture((selected) ? asset_manager::kDialogueTriangle :
                                                      asset_manager::kEmptyImage);
  };

  _setObjectCallback = [](ListView::ListViewItem* listViewItem, Dialogue* dialogue) {
    listViewItem->getLabel()->setString(dialogue->getLines().front());
  };
}


void DialogueListView::confirm() {
  auto dialogueMgr = DialogueManager::getInstance();
  auto dialogueMenu = dialogueMgr->getDialogueMenu();
  auto subtitles = dialogueMgr->getSubtitles();

  for (const auto& cmd : getSelectedObject()->getCmds()) {
    Console::getInstance()->executeCmd(cmd);
  }

  if (getSelectedObject()->getChildren().empty()) {
    subtitles->endSubtitles();
    dialogueMgr->getTargetNpc()->getDialogueTree().resetCurrentNode();
  } else {
    Dialogue* nextDialogue = getSelectedObject()->getChildren().front();
    for (const auto& line : nextDialogue->getLines()) {
      subtitles->addSubtitle(line);
    }
    subtitles->showNextSubtitle();
    dialogueMgr->setCurrentDialogue(nextDialogue);
  }

  dialogueMgr->getTargetNpc()->getDialogueTree().update();
  dialogueMenu->getLayer()->setVisible(false);
}


void DialogueListView::updatePosition() {
  auto winSize = Director::getInstance()->getWinSize();
  auto listViewSize = getContentSize();  // defined in ui/ListView.h

  // Try to re-center the DialogueListView by calculating a `newPositionX` for it.
  float newPositionX = winSize.width / 2 - listViewSize.width / 2 - 12.0;
  _layout->setPositionX(newPositionX);

  // Update the positionX of _scrollBar.
  _scrollBar->setPositionX(listViewSize.width + 15.0);

  // FIXME: See if we can get rid of the floating-point literals above.
}

}  // namespace vigilante
