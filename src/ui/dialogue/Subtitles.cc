// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Subtitles.h"

#include <vector>

#include "AssetManager.h"
#include "input/InputManager.h"
#include "ui/console/Console.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/hud/Hud.h"
#include "util/ds/Algorithm.h"

#define SHOW_CHAR_INTERVAL .03f
#define LETTERBOX_HEIGHT 50
#define LETTERBOX_TRANSITION_DURATION 1.0f
#define SUBTITLES_Y 38

using std::string;
using std::queue;
using std::vector;
using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::Label;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using cocos2d::MoveBy;
using cocos2d::Sequence;
using cocos2d::CallFunc;
using vigilante::asset_manager::kShade;
using vigilante::asset_manager::kDialogueTriangle;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

Subtitles::Subtitles()
    : _layer(Layer::create()),
      _label(Label::createWithTTF("", kRegularFont, kRegularFontSize)),
      _nextSubtitleIcon(ImageView::create(kDialogueTriangle)),
      _upperLetterbox(ImageView::create(kShade)),
      _lowerLetterbox(ImageView::create(kShade)),
      _currentSubtitle(""),
      _isTransitioning(),
      _timer() {
  auto winSize = Director::getInstance()->getWinSize();
  _label->setPosition(winSize.width / 2, SUBTITLES_Y);
  _label->getFontAtlas()->setAliasTexParameters();

  _upperLetterbox->setAnchorPoint({0, 0});
  _upperLetterbox->setPositionY(winSize.height);
  _upperLetterbox->setScaleX(winSize.width);
  _upperLetterbox->setScaleY(LETTERBOX_HEIGHT);

  _lowerLetterbox->setAnchorPoint({0, 0});
  _lowerLetterbox->setPositionY(-LETTERBOX_HEIGHT);
  _lowerLetterbox->setScaleX(winSize.width);
  _lowerLetterbox->setScaleY(LETTERBOX_HEIGHT);

  _nextSubtitleIcon->setVisible(false);

  _layer->addChild(_upperLetterbox);
  _layer->addChild(_lowerLetterbox);
  _layer->addChild(_label);
  _layer->addChild(_nextSubtitleIcon);
  _layer->setVisible(false);
}


void Subtitles::update(float delta) {
  if (!_layer->isVisible() || _label->getString().size()  == _currentSubtitle.text.size()) {
    return;
  }

  if (_timer >= SHOW_CHAR_INTERVAL) {
    int nextCharIdx = _label->getString().size();
    _label->setString(_label->getString() + _currentSubtitle.text.at(nextCharIdx));
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
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
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

  _isTransitioning = true;
  Hud::getInstance()->getLayer()->setVisible(false);
  _layer->setVisible(true);
  _upperLetterbox->runAction(MoveBy::create(LETTERBOX_TRANSITION_DURATION, {0, -LETTERBOX_HEIGHT}));
  _lowerLetterbox->runAction(Sequence::createWithTwoActions(
    MoveBy::create(LETTERBOX_TRANSITION_DURATION, {0, LETTERBOX_HEIGHT}),
    CallFunc::create([=]() {
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
  _upperLetterbox->runAction(MoveBy::create(LETTERBOX_TRANSITION_DURATION, {0, LETTERBOX_HEIGHT}));
  _lowerLetterbox->runAction(Sequence::createWithTwoActions(
    MoveBy::create(LETTERBOX_TRANSITION_DURATION, {0, -LETTERBOX_HEIGHT}),
    CallFunc::create([=]() {
      _isTransitioning = false;
      _layer->setVisible(false);
      Hud::getInstance()->getLayer()->setVisible(true);
      DialogueManager::getInstance()->getTargetNpc()->onDialogueEnd();
    })
  ));
}

void Subtitles::showNextSubtitle() {
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
  DialogueManager* dialogueMgr = DialogueManager::getInstance();
  DialogueMenu* dialogueMenu = dialogueMgr->getDialogueMenu();
  DialogueListView* dialogueListView = dialogueMenu->getDialogueListView();
  Dialogue* currentDialogue = dialogueMgr->getCurrentDialogue();

  for (const auto& cmd : currentDialogue->getCmds()) {
    Console::getInstance()->executeCmd(cmd);
  }

  vector<DialogueTree::Node*> children = currentDialogue->getChildren();
  if (children.empty()) {  // end of dialogue
    endSubtitles();
    dialogueMgr->getTargetNpc()->getDialogueTree().resetCurrentNode();
  } else {  // still has children dialogue
    dialogueListView->setObjects<vector>(children);
    dialogueListView->updatePosition();
    dialogueMenu->getLayer()->setVisible(true);
  }
}


Layer* Subtitles::getLayer() const {
  return _layer;
}


Subtitles::Subtitle::Subtitle(const string& text) : text(text) {}

}  // namespace vigilante
