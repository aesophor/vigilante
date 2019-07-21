// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Subtitles.h"

#include "AssetManager.h"
#include "input/InputManager.h"
#include "ui/hud/Hud.h"

using std::string;
using std::queue;
using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::Label;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using cocos2d::MoveBy;
using cocos2d::Sequence;
using cocos2d::CallFunc;
using vigilante::asset_manager::kShade;
using vigilante::asset_manager::kDialogNextIcon;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

Subtitles* Subtitles::_instance = nullptr;

const float Subtitles::_kShowCharInterval = .05f;
const int Subtitles::_kLetterboxHeight = 50;

Subtitles* Subtitles::getInstance() {
  if (!_instance) {
    _instance = new Subtitles();
  }
  return _instance;
}

Subtitles::Subtitles()
    : _layer(Layer::create()),
      _label(Label::createWithTTF("", kRegularFont, kRegularFontSize)),
      _nextDialogIcon(ImageView::create(kDialogNextIcon)),
      _upperLetterbox(ImageView::create(kShade)),
      _lowerLetterbox(ImageView::create(kShade)),
      _currentDialog(""),
      _timer() {
  auto winSize = Director::getInstance()->getWinSize();
  _label->setPosition(winSize.width / 2, 20);
  _label->getFontAtlas()->setAliasTexParameters();

  _upperLetterbox->setAnchorPoint({0, 0});
  _upperLetterbox->setPositionY(winSize.height);
  _upperLetterbox->setScaleX(winSize.width);
  _upperLetterbox->setScaleY(_kLetterboxHeight);

  _lowerLetterbox->setAnchorPoint({0, 0});
  _lowerLetterbox->setPositionY(-_kLetterboxHeight);
  _lowerLetterbox->setScaleX(winSize.width);
  _lowerLetterbox->setScaleY(_kLetterboxHeight);

  _nextDialogIcon->setVisible(false);

  _layer->addChild(_upperLetterbox);
  _layer->addChild(_lowerLetterbox);
  _layer->addChild(_label);
  _layer->addChild(_nextDialogIcon);
  _layer->setVisible(false);
}


void Subtitles::update(float delta) {
  if (!_layer->isVisible() || _label->getString().size()  == _currentDialog.dialog.size()) {
    return;
  }

  if (_timer >= _kShowCharInterval) {
    int nextCharIdx = _label->getString().size();
    _label->setString(_label->getString() + _currentDialog.dialog.at(nextCharIdx));
    _timer = 0;
  }
  if (_label->getString().size() == _currentDialog.dialog.size()) {
    float x = _label->getPositionX() + _label->getContentSize().width / 2;
    float y = _label->getPositionY();
    _nextDialogIcon->setPosition({x + 25, y});
  }
  _timer += delta;
}

void Subtitles::handleInput() {
  auto inputMgr = InputManager::getInstance();

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {
    showNextDialog();
  }
}

void Subtitles::addDialog(const string& s) {
  _dialogQueue.push(Dialog(s));
}

void Subtitles::beginDialog() {
  if (_dialogQueue.empty()) {
    return;
  }
  Hud::getInstance()->getLayer()->setVisible(false);
  _layer->setVisible(true);
  _upperLetterbox->runAction(MoveBy::create(2.0f, {0, -_kLetterboxHeight}));
  _lowerLetterbox->runAction(Sequence::createWithTwoActions(
    MoveBy::create(2.0f, {0, _kLetterboxHeight}),
    CallFunc::create([=]() {
      showNextDialog();
    })
  ));
}

void Subtitles::endDialog() {
  _upperLetterbox->runAction(MoveBy::create(2.0f, {0, _kLetterboxHeight}));
  _lowerLetterbox->runAction(Sequence::createWithTwoActions(
    MoveBy::create(2.0f, {0, -_kLetterboxHeight}),
    CallFunc::create([=]() {
      Hud::getInstance()->getLayer()->setVisible(true);
      _layer->setVisible(false);
    })
  ));
}

void Subtitles::showNextDialog() {
  _nextDialogIcon->setVisible(false);

  if (_dialogQueue.empty()) {
    _currentDialog.dialog.clear();
    _label->setString("");
    endDialog();
    return;
  }

  _currentDialog = _dialogQueue.front();
  _dialogQueue.pop();
  _label->setString("");
  _timer = 0;
}

Layer* Subtitles::getLayer() const {
  return _layer;
}


Subtitles::Dialog::Dialog(const string& dialog) : dialog(dialog) {}

} // namespace vigilante
