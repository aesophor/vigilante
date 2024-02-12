// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CollectItemObjective.h"

#include "scene/GameScene.h"
#include "scene/SceneManager.h"

using namespace std;

namespace vigilante {

bool CollectItemObjective::isCompleted() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  return gmMgr->getPlayer()->getItemAmount(_itemJsonFilePath) >= _amount;
}

}  // namespace vigilante
