// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Subtitles.h"

#include <vector>

#include "Assets.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/ds/Algorithm.h"

using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

namespace {

constexpr auto kShowCharInterval = .03f;
constexpr auto kLetterboxHeight = 50;
constexpr auto kSubtitlesY = 38;

}  // namespace

Subtitles::Subtitles()
    : _layer{Layer::create()},
      _label{Label::createWithTTF("", string{kRegularFont}, kRegularFontSize)},
      _nextSubtitleIcon{ui::ImageView::create(string{kDialogueTriangle})},
      _upperLetterbox{ui::ImageView::create(string{kShade})},
      _lowerLetterbox{ui::ImageView::create(string{kShade})} {
  auto winSize = Director::getInstance()->getWinSize();
  _label->setPosition(winSize.width / 2, kSubtitlesY);
  _label->getFontAtlas()->setAliasTexParameters();

  _upperLetterbox->setAnchorPoint({0, 0});
  _upperLetterbox->setPositionY(winSize.height);
  _upperLetterbox->setScaleX(winSize.width);
  _upperLetterbox->setScaleY(kLetterboxHeight);

  _lowerLetterbox->setAnchorPoint({0, 0});
  _lowerLetterbox->setPositionY(-kLetterboxHeight);
  _lowerLetterbox->setScaleX(winSize.width);
  _lowerLetterbox->setScaleY(kLetterboxHeight);

  _nextSubtitleIcon->setVisible(false);

  _layer->addChild(_upperLetterbox);
  _layer->addChild(_lowerLetterbox);
  _layer->addChild(_label);
  _layer->addChild(_nextSubtitleIcon);
  _layer->setVisible(false);
}

void Subtitles::update(const float delta) {
  if (!_layer->isVisible() || _label->getString().size()  == _currentSubtitle.text.size()) {
    return;
  }

  if (_timer >= kShowCharInterval) {
    int nextCharIdx = static_cast<int>(_label->getString().size());
    _label->setString(string{_label->getString()} + _currentSubtitle.text.at(nextCharIdx));
    _timer = 0;
  }
  if (_label->getString().size() == _currentSubtitle.text.size()) {
    float x = _label->getPositionX() + _label->getContentSize().width / 2;
    float y = _label->getPositionY();
    _nextSubtitleIcon->setPosition({x + 25, y});
  }
  _timer += delta;
}

void Subtitles::handleInput() {
  if (!IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER) &&
      !IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_SPACE)) {
    return;
  }

  if (_label->getString().size()  != _currentSubtitle.text.size()) {
    finishCurrentSubtitle();
  } else {
    showNextSubtitle();
  }
}

void Subtitles::addSubtitle(const string& s) {
  _subtitleQueue.push(Subtitle(s));
}

void Subtitles::beginSubtitles() {
  if (_isTransitioning || _subtitleQueue.empty()) {
    return;
  }

  auto hud = SceneManager::the().getCurrentScene<GameScene>()->getHud();
  hud->getLayer()->setVisible(false);

  _isTransitioning = true;
  _layer->setVisible(true);
  _upperLetterbox->runAction(MoveBy::create(kLetterboxTransitionDuration, {0, -kLetterboxHeight}));
  _lowerLetterbox->runAction(Sequence::createWithTwoActions(
    MoveBy::create(kLetterboxTransitionDuration, {0, kLetterboxHeight}),
    CallFunc::create([this]() {
      _isTransitioning = false;
      showNextSubtitle();
    })
  ));
}

void Subtitles::endSubtitles() {
  if (_isTransitioning) {
    return;
  }

  _isTransitioning = true;
  _upperLetterbox->runAction(MoveBy::create(kLetterboxTransitionDuration, {0, kLetterboxHeight}));
  _lowerLetterbox->runAction(Sequence::createWithTwoActions(
    MoveBy::create(kLetterboxTransitionDuration, {0, -kLetterboxHeight}),
    CallFunc::create([this]() {
      _isTransitioning = false;
      _layer->setVisible(false);

      auto hud = SceneManager::the().getCurrentScene<GameScene>()->getHud();
      hud->getLayer()->setVisible(true);

      auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
      auto targetNpc = dialogueMgr->getTargetNpc();
      if (!targetNpc) {
        return;
      }

      targetNpc->onDialogueEnd();
    })
  ));
}

void Subtitles::finishCurrentSubtitle() {
  if (_isTransitioning) {
    return;
  }

  _label->setString(_currentSubtitle.text);
}

void Subtitles::showNextSubtitle() {
  if (_isTransitioning) {
    return;
  }

  _nextSubtitleIcon->setVisible(false);

  if (!_subtitleQueue.empty()) {
    _currentSubtitle = _subtitleQueue.front();
    _subtitleQueue.pop();
    _label->setString("");
    _timer = 0;
    return;
  }

  _currentSubtitle.text.clear();
  _label->setString("");

  // If all subtitles has been displayed, show DialogueMenu if possible.
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  Dialogue* currentDialogue = dialogueMgr->getCurrentDialogue();
  if (!currentDialogue) {
    endSubtitles();
    return;
  }

  auto console = SceneManager::the().getCurrentScene<GameScene>()->getConsole();
  for (const auto& cmd : currentDialogue->getCmds()) {
    console->executeCmd(cmd);
  }

  vector<DialogueTree::Node*> children = currentDialogue->getChildren();
  if (children.empty()) {  // end of dialogue
    endSubtitles();
    dialogueMgr->getTargetNpc()->getDialogueTree().resetCurrentNode();
  } else {  // still has children dialogue
    DialogueMenu* dialogueMenu = dialogueMgr->getDialogueMenu();
    DialogueListView* dialogueListView = dialogueMenu->getDialogueListView();
    dialogueListView->setObjects<vector>(children);
    dialogueListView->updatePosition();
    dialogueMenu->getLayer()->setVisible(true);
  }
}

}  // namespace requiem
