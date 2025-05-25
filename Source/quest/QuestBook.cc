// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "QuestBook.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>

#include "quest/KillTargetObjective.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/console/Console.h"
#include "util/ds/Algorithm.h"
#include "util/StringUtil.h"
#include "util/Logger.h"

namespace fs = std::filesystem;
using namespace std;

namespace vigilante {

QuestBook::QuestBook() {
  reset();
}

void QuestBook::update(Quest::Objective::Type objectiveType) {
  auto questHints = SceneManager::the().getCurrentScene<GameScene>()->getQuestHints();

  VGLOG(LOG_INFO, "Updating quests");
  for (const auto quest : _inProgressQuests) {
    if (quest->getCurrentStage().objective->getObjectiveType() != objectiveType) {
      continue;
    }

    while (!quest->isCompleted() && quest->getCurrentStage().objective->isCompleted()) {
      quest->advanceStage();

      if (quest->isCompleted()) {
        markCompleted(quest);
      } else {
        questHints->show(quest->getCurrentStage().objective->getDesc());
      }
    }
  }
}

bool QuestBook::reset() {
  _questMapper.clear();
  _inProgressQuests.clear();
  _completedQuests.clear();

  ifstream fin{assets::kQuestsList};
  if (!fin.is_open()) {
    throw runtime_error("Failed to open quest list: " + assets::kQuestsList.native());
  }

  string line;
  while (std::getline(fin, line)) {
    const fs::path path = fs::path{line}.lexically_normal();
    _questMapper[path] = std::make_unique<Quest>(path);
  }
}

bool QuestBook::unlockQuest(Quest* quest) {
  quest->unlock();
  return true;
}

bool QuestBook::startQuest(Quest* quest) {
  // If the quest is already completed or is already in progress, return at once.
  if (quest->isCompleted()) {
    VGLOG(LOG_ERR, "Failed to start quest [%s], quest already completed.",
                   quest->getQuestProfile().jsonFilePath.c_str());
    return false;
  }

  const auto it = std::find(_inProgressQuests.begin(), _inProgressQuests.end(), quest);
  if (it != _inProgressQuests.end()) {
    VGLOG(LOG_ERR, "Failed to start quest [%s], quest has already started.",
                   quest->getQuestProfile().jsonFilePath.c_str());
    return false;
  }

  // Add this quest to _inProgressQuests.
  _inProgressQuests.push_back(quest);
  quest->advanceStage();

  auto questHints = SceneManager::the().getCurrentScene<GameScene>()->getQuestHints();
  questHints->show("Started: " + quest->getQuestProfile().title);
  questHints->show(quest->getCurrentStage().objective->getDesc());

  return true;
}

bool QuestBook::setStage(Quest* quest, const int stageIdx) {
  auto it = std::find(_inProgressQuests.begin(), _inProgressQuests.end(), quest);
  if (it == _inProgressQuests.end()) {
    VGLOG(LOG_ERR, "Failed to set stage of quest [%s], quest has not started.",
                   quest->getQuestProfile().jsonFilePath.c_str());
    return false;
  }

  if (stageIdx <= quest->getCurrentStageIdx()) {
    VGLOG(LOG_ERR, "Skipped setting stage of quest [%s], current stage [%d], new stage [%d].",
                   quest->getQuestProfile().jsonFilePath.c_str(), quest->getCurrentStageIdx(), stageIdx);
    return true;
  }

  const Quest::Stage& prevStage = quest->getCurrentStage();
  auto console = SceneManager::the().getCurrentScene<GameScene>()->getConsole();
  for (const auto& cmd : prevStage.cmds) {
    console->executeCmd(cmd);
  }

  if (stageIdx >= static_cast<int>(quest->getQuestProfile().stages.size())) {
    return markCompleted(quest);
  }

  auto questHints = SceneManager::the().getCurrentScene<GameScene>()->getQuestHints();
  quest->setCurrentStageIdx(stageIdx);
  questHints->show("Completed: " + prevStage.objective->getDesc());
  questHints->show(quest->getCurrentStage().objective->getDesc());

  return true;
}

bool QuestBook::markCompleted(Quest* quest) {
  auto it = std::find(_inProgressQuests.begin(), _inProgressQuests.end(), quest);
  if (it == _inProgressQuests.end()) {
    VGLOG(LOG_ERR, "Failed to mark quest [%s] as completed, quest has not started.",
                   quest->getQuestProfile().jsonFilePath.c_str());
    return false;
  }

  _inProgressQuests.erase(std::remove(_inProgressQuests.begin(), _inProgressQuests.end(), quest), _inProgressQuests.end());
  _completedQuests.push_back(quest);

  auto questHints = SceneManager::the().getCurrentScene<GameScene>()->getQuestHints();
  questHints->show("Completed: " + quest->getQuestProfile().title);

  return true;
}

bool QuestBook::unlockQuest(const string& questJsonFilePath) {
  auto it = _questMapper.find(questJsonFilePath);
  if (it == _questMapper.end()) {
    VGLOG(LOG_ERR, "Failed to unlock quest [%s]", questJsonFilePath.c_str());
    return false;
  }

  return unlockQuest(it->second.get());
}

bool QuestBook::startQuest(const string& questJsonFilePath) {
  auto it = _questMapper.find(questJsonFilePath);
  if (it == _questMapper.end()) {
    VGLOG(LOG_ERR, "Failed to start quest [%s]", questJsonFilePath.c_str());
    return false;
  }

  return startQuest(it->second.get());
}

bool QuestBook::setStage(const string& questJsonFilePath, const int stageIdx) {
  auto it = _questMapper.find(questJsonFilePath);
  if (it == _questMapper.end()) {
    VGLOG(LOG_ERR, "Failed to start quest [%s]", questJsonFilePath.c_str());
    return false;
  }

  return setStage(it->second.get(), stageIdx);
}

bool QuestBook::markCompleted(const string& questJsonFilePath) {
  auto it = _questMapper.find(questJsonFilePath);
  if (it == _questMapper.end()) {
    VGLOG(LOG_ERR, "Failed to mark quest [%s] as completed", questJsonFilePath.c_str());
    return false;
  }

  return markCompleted(it->second.get());
}

vector<Quest*> QuestBook::getAllQuests() const {
  vector<Quest*> allQuests(_inProgressQuests.begin(), _inProgressQuests.end());
  allQuests.insert(allQuests.end(), _completedQuests.begin(), _completedQuests.end());
  return allQuests;
}

optional<Quest*> QuestBook::getQuest(const string& questJsonFilePath) const {
  auto it = _questMapper.find(questJsonFilePath);
  if (it == _questMapper.end()) {
    VGLOG(LOG_ERR, "Failed to find quest [%s].", questJsonFilePath.c_str());
    return nullopt;
  }

  return it->second.get();
}

}  // namespace vigilante
