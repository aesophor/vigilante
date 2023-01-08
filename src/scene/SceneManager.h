// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_SCENE_MANAGER_H_
#define VIGILANTE_SCENE_MANAGER_H_

#include <stack>

#include <cocos2d.h>

#include "scene/GameScene.h"

namespace vigilante {

// cocos2d-x's Director is a pile of bullshit because
// it doesn't allow me to fucking pop a scene and
// get the next scene immediately, so I end up implementing
// this class.
class SceneManager final {
 public:
  static SceneManager& the();

  void runWithScene(cocos2d::Scene* scene);
  void pushScene(cocos2d::Scene* scene);
  void popScene();
  
  template <typename T = cocos2d::Scene>
  T* getCurrentScene() const {
    return dynamic_cast<T*>(_scenes.top());
  }

 private:
  SceneManager();

  cocos2d::Director* _director;
  std::stack<cocos2d::Scene*> _scenes;
};

}  // namespace vigilante

#endif  // VIGILANTE_SCENE_MANAGER_H_
