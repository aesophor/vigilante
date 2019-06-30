#include "input/InputManager.h"

#include "map/GameMapManager.h"
#include "ui/pause_menu/PauseMenuDialog.h"
#include "util/KeyCodeUtil.h"

using std::set;
using std::array;
using cocos2d::Scene;
using cocos2d::Event;
using cocos2d::EventKeyboard;
using cocos2d::EventListenerKeyboard;

namespace vigilante {

InputManager* InputManager::_instance = nullptr;

const array<EventKeyboard::KeyCode, InputManager::Hotkey::SIZE> InputManager::_kBindableKeys = {{
  EventKeyboard::KeyCode::KEY_LEFT_SHIFT,
  EventKeyboard::KeyCode::KEY_LEFT_CTRL,
  EventKeyboard::KeyCode::KEY_X,
  EventKeyboard::KeyCode::KEY_C,
  EventKeyboard::KeyCode::KEY_V
}};


InputManager* InputManager::getInstance() {
  if (!_instance) {
    _instance = new InputManager();
  }
  return _instance;
}

InputManager::InputManager()
    : _scene(),
      _keyboardEvLstnr(),
      _pressedKeys(),
      _hotkeys(),
      _isAssigningHotkey(),
      _keybindable(),
      _pauseMenuDialog() {}


void InputManager::activate(Scene* scene) {
  _scene = scene;
  _keyboardEvLstnr = EventListenerKeyboard::create();

  _keyboardEvLstnr->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
    if (!_isAssigningHotkey) {
      if (_pressedKeys.find(keyCode) == _pressedKeys.end()) {
        _pressedKeys.insert(keyCode);
      }
    } else {
      setHotkeyAction(keyCode, _keybindable);
      _keybindable->setHotkey(keycode_util::keyCodeToString(keyCode));
      _isAssigningHotkey = false;
      _keybindable = nullptr;
      _pauseMenuDialog->setVisible(false);
    }
  };

  _keyboardEvLstnr->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
    _pressedKeys.erase(keyCode);
  };
  
  _scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_keyboardEvLstnr, scene);
}

void InputManager::deactivate() {
  _scene->getEventDispatcher()->removeEventListener(_keyboardEvLstnr);
  _keyboardEvLstnr = nullptr;
  _scene = nullptr;
}


bool InputManager::isKeyPressed(EventKeyboard::KeyCode keyCode) const {
  return _pressedKeys.find(keyCode) != _pressedKeys.end();
}

bool InputManager::isKeyJustPressed(EventKeyboard::KeyCode keyCode) {
  // std::set::erase() returns the number of elements erased.
  return _pressedKeys.erase(keyCode) > 0;
}


Keybindable* InputManager::getHotkeyAction(EventKeyboard::KeyCode keyCode) const {
  for (size_t i = 0; i < _kBindableKeys.size(); i++) {
    if (keyCode == _kBindableKeys[i]) {
      return _hotkeys[i];
    }
  }
  return nullptr;
}

void InputManager::setHotkeyAction(EventKeyboard::KeyCode keyCode, Keybindable* keybindable) {
  for (size_t i = 0; i < _kBindableKeys.size(); i++) {
    if (keyCode == _kBindableKeys[i]) {
      _hotkeys[i] = keybindable;
      return;
    }
  }
}

void InputManager::promptHotkey(Keybindable* keybindable, PauseMenuDialog* pauseMenuDialog) {
  _isAssigningHotkey = true;
  _keybindable = keybindable;
  _pauseMenuDialog = pauseMenuDialog;
}

} // namespace vigilante
