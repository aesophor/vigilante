// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "input/InputManager.h"

#include "util/Logger.h"

using std::set;
using cocos2d::Scene;
using cocos2d::Event;
using cocos2d::EventKeyboard;
using cocos2d::EventListenerKeyboard;

namespace vigilante {

InputManager* InputManager::_instance = nullptr;

InputManager* InputManager::getInstance() {
  if (!_instance) {
    _instance = new InputManager();
  }
  return _instance;
}

InputManager::InputManager()
    : _scene(), _keyboardEvLstnr(), _pressedKeys(), _hasScheduledPopEvLstnr() {
  // Push the default OnKeyPressedEvLstnr.
  pushEvLstnr([=](EventKeyboard::KeyCode keyCode, Event*) {
    _pressedKeys.insert(keyCode);
  });
}


void InputManager::activate(Scene* scene) {
  _scene = scene;
  _keyboardEvLstnr = EventListenerKeyboard::create();

  // Capture "this" by value.
  _keyboardEvLstnr->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* e) {
    _onKeyPressedEvLstnrs.top()(keyCode, e);
  };

  _keyboardEvLstnr->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event*) {
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


void InputManager::pushEvLstnr(const OnKeyPressedEvLstnr& evLstnr) {
  _onKeyPressedEvLstnrs.push(evLstnr);
}

void InputManager::popEvLstnr() {
  // Do not pop the default OnKeyPressedEvLstnr.
  if (_onKeyPressedEvLstnrs.size() > 1) {
    _onKeyPressedEvLstnrs.pop();
  }
}

} // namespace vigilante
