// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "SceneManager.h"

#include "input/InputManager.h"
#include "scene/GameScene.h"

using namespace std;
USING_NS_CC;

namespace vigilante {

SceneManager& SceneManager::the() {
  static SceneManager instance;
  return instance;
}

SceneManager::SceneManager()
    : _director(Director::getInstance()),
      _scenes() {}

void SceneManager::runWithScene(Scene* scene) {
  _director->runWithScene(scene);
  _scenes.push(scene);
}

void SceneManager::pushScene(Scene* scene) {
  _director->pushScene(scene);
  _scenes.push(scene);

  InputManager::the().deactivate();
  InputManager::the().activate(scene);
}

void SceneManager::popScene() {
  if (auto gameScene = getCurrentScene<GameScene>()) {
    gameScene->quit();
  }

  _director->popScene();
  _scenes.pop();

  InputManager::the().deactivate();
  InputManager::the().activate(getCurrentScene());
}

}  // namespace vigilante
