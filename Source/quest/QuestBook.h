// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_QUEST_BOOK_H_
#define VIGILANTE_QUEST_BOOK_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Quest.h"

namespace vigilante {

class QuestBook {
 public:
  explicit QuestBook(const std::string& questsListFilePath);
  virtual ~QuestBook() = default;

  void update(const Quest::Objective::Type objectiveType);

  bool unlockQuest(Quest* quest);
  bool startQuest(Quest* quest);
  bool setStage(Quest* quest, const int stageIdx);
  bool markCompleted(Quest* quest);

  bool unlockQuest(const std::string& questJsonFilePath);
  bool startQuest(const std::string& questJsonFilePath);
  bool setStage(const std::string&, const int stageIdx);
  bool markCompleted(const std::string& questJsonFilePath);

  std::vector<Quest*> getAllQuests() const;
  inline const std::vector<Quest*>& getInProgressQuests() const { return _inProgressQuests; }
  inline const std::vector<Quest*>& getCompletedQuests() const { return _completedQuests; }

 private:
  std::unordered_map<std::string, std::unique_ptr<Quest>> _questMapper;
  std::vector<Quest*> _inProgressQuests;
  std::vector<Quest*> _completedQuests;
};

} // namespace vigilante

#endif // VIGILANTE_QUEST_BOOK_H_
