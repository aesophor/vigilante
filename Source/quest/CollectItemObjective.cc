// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CollectItemObjective.h"

#include "scene/GameScene.h"
#include "scene/SceneManager.h"

using namespace std;

namespace vigilante {

CollectItemObjective::CollectItemObjective(const string& desc,
                                           const string& itemJsonFileName,
                                           int amount)
    : Quest::Objective(Quest::Objective::Type::COLLECT, desc),
      _itemJsonFileName(itemJsonFileName),
      _amount(amount) {}

bool CollectItemObjective::isCompleted() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  return gmMgr->getPlayer()->getItemAmount(_itemJsonFileName) >= _amount;
}

}  // namespace vigilante
