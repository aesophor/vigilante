#include "DialogManager.h"

#include "AssetManager.h"
#include "input/GameInputManager.h"

using std::string;
using std::queue;
using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::Label;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

DialogManager* DialogManager::_instance = nullptr;
const float DialogManager::_kShowCharInterval = .05f;

DialogManager* DialogManager::getInstance() {
  if (!_instance) {
    _instance = new DialogManager();
  }
  return _instance;
}

DialogManager::DialogManager()
    : _layer(Layer::create()),
      _label(Label::createWithTTF("", kRegularFont, kRegularFontSize)),
      _nextDialogIcon(ImageView::create(asset_manager::kDialogNextIcon)),
      _currentDialog("", ""),
      _timer() {
  auto winSize = Director::getInstance()->getWinSize();
  _label->setPosition(winSize.width / 2, 20);
  _label->getFontAtlas()->setAliasTexParameters();

  _layer->addChild(_label);
  //_layer->addChild(_nextDialogIcon);
  _layer->setVisible(false);
}


void DialogManager::update(float delta) {
  if (!_layer->isVisible() || _label->getString().size() - _currentDialog.speakerName.size() - 2 == _currentDialog.content.size()) {
    return;
  }

  if (_timer >= _kShowCharInterval) {
    int nextCharIdx = _label->getString().size() - _currentDialog.speakerName.size() - 2;
    _label->setString(_label->getString() + _currentDialog.content.at(nextCharIdx));
    _timer = 0;
  }
  _timer += delta;
}

void DialogManager::handleInput() {
  auto inputMgr = GameInputManager::getInstance();

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {
    showNextDialog();
  }
}

void DialogManager::add(const string& speakerName, const string& content) {
  _dialogQueue.push(Dialog(speakerName, content));
}

void DialogManager::showNextDialog() {
  if (_dialogQueue.empty()) {
    _layer->setVisible(false);
  } else {
    _layer->setVisible(true);
    _currentDialog = _dialogQueue.front();
    _dialogQueue.pop();

    _timer = 0;
    _label->setString(_currentDialog.speakerName + ": ");
  }
}

Layer* DialogManager::getLayer() const {
  return _layer;
}


DialogManager::Dialog::Dialog(const string& speakerName, const string& content)
    : speakerName(speakerName), content(content) {}

} // namespace vigilante
