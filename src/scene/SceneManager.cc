// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "SceneManager.h"

using cocos2d::Director;
using cocos2d::Scene;

namespace vigilante {

SceneManager* SceneManager::getInstance() {
  static SceneManager instance;
  return &instance;
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
}

void SceneManager::popScene() {
  _director->popScene();
  _scenes.pop();
}

Scene* SceneManager::getCurrentScene() const {
  return _scenes.top();
}

}  // namespace vigilante
