// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "WindowManager.h"

#include "Constants.h"
#include "util/Logger.h"

using namespace std;
using namespace vigilante::z_order;
USING_NS_AX;

namespace vigilante {

WindowManager::WindowManager()
    : _kMaxWindowCount{kWindowTop - kWindowBottom + 1},
      _defaultCameraMask(static_cast<uint16_t>(CameraFlag::USER1)) {}

void WindowManager::update(const float delta) {
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
  _scene->addChild(window->getLayer(), kWindowBottom + static_cast<int>(_windows.size()));

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

}  // namespace vigilante
