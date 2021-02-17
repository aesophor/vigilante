// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "WindowManager.h"

#include "Constants.h"
#include "util/Logger.h"

using std::unique_ptr;
using cocos2d::Scene;
using cocos2d::CameraFlag;
using vigilante::graphical_layers::kWindowBottom;
using vigilante::graphical_layers::kWindowTop;

namespace vigilante {

WindowManager* WindowManager::getInstance() {
  static WindowManager instance;
  return &instance;
}


WindowManager::WindowManager()
    : _kMaxWindowCount(kWindowTop - kWindowBottom + 1),
      _scene(),
      _defaultCameraMask(static_cast<uint16_t>(CameraFlag::USER1)),
      _windows() {}


void WindowManager::update(float delta) {
  for (auto& w : _windows) {
    w->update(delta);
  }
}

void WindowManager::push(unique_ptr<Window> window) {
  if (_windows.size() >= _kMaxWindowCount) {
    VGLOG(LOG_WARN, "Unable to render more window(s), maximum: %ld", _kMaxWindowCount);
    return;
  }
  
  // Ensure that `_defaultCameraMask` is set on newly rendered window.
  window->getLayer()->setCameraMask(_defaultCameraMask);

  // Show window.
  window->getLayer()->setVisible(true);
  _scene->addChild(window->getLayer(), kWindowBottom + _windows.size());

  // Manage window.
  _windows.push_back(std::move(window));
}

unique_ptr<Window> WindowManager::pop() {
  if (_windows.empty()) {
    VGLOG(LOG_ERR, "Unable to pop more window(s).");
    return nullptr;
  }

  // Unmanage window.
  unique_ptr<Window> removedWindow = std::move(_windows.back());
  _windows.pop_back();

  // Set window to invisible.
  removedWindow->setVisible(false);
  _scene->removeChild(removedWindow->getLayer());

  return removedWindow;
}

Window* WindowManager::top() const {
  if (_windows.empty()) {
    return nullptr;
  }
  return _windows.back().get();
}


bool WindowManager::isEmpty() const {
  return _windows.empty();
}

int WindowManager::getSize() const {
  return _windows.size();
}

void WindowManager::setScene(Scene* scene) {
  _scene = scene;
}

uint16_t WindowManager::getDefaultCameraMask() const {
  return _defaultCameraMask;
}

void WindowManager::setDefaultCameraMask(uint16_t cameraMask) {
  _defaultCameraMask = cameraMask;
}

}  // namespace vigilante
