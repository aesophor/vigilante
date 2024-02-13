// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueListView.h"

#include "Assets.h"
#include "Constants.h"
#include "gameplay/DialogueTree.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/dialogue/DialogueListView.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr auto kVisibleItemCount = 3;
constexpr auto kWidth = 0;  // The position of _scrollBar will be updated dynamically
constexpr auto kHeight = 53;
constexpr auto kItemGapHeight = 15;

}  // namespace

DialogueListView::DialogueListView(DialogueMenu* dialogMenu)
    : ListView<Dialogue*>{kVisibleItemCount, kWidth, kHeight, kItemGapHeight, kEmptyImage, kEmptyImage},
      _dialogueMenu{dialogMenu} {

  _setSelectedCallback = [](ListView::ListViewItem* listViewItem, bool selected) {
    listViewItem->getIcon()->loadTexture((selected) ? string{kDialogueTriangle} : string{kEmptyImage});
  };

  _setObjectCallback = [](ListView::ListViewItem* listViewItem, Dialogue* dialogue) {
    listViewItem->getLabel()->setString(dialogue->getLines().front());
  };
}

void DialogueListView::confirm() {
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  auto dialogueMenu = dialogueMgr->getDialogueMenu();
  auto subtitles = dialogueMgr->getSubtitles();

  auto console = SceneManager::the().getCurrentScene<GameScene>()->getConsole();
  bool hasAllCmdsSucceeded = true;
  for (const auto& cmd : getSelectedObject()->getCmds()) {
    if (!console->executeCmd(cmd)) {
      hasAllCmdsSucceeded = false;
    }
  }

  const vector<DialogueTree::Node*> children = hasAllCmdsSucceeded ? getSelectedObject()->getChildren()
                                                                   : getSelectedObject()->getChildrenOnExecFail();
  if (children.empty()) {
    subtitles->endSubtitles();
    dialogueMgr->getTargetNpc()->getDialogueTree().resetCurrentNode();
  } else {
    Dialogue* nextDialogue = children.front();
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
