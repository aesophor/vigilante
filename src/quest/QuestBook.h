// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_QUEST_BOOK_H_
#define VIGILANTE_QUEST_BOOK_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "Quest.h"

namespace vigilante {

class QuestBook {
 public:
  explicit QuestBook(const std::string& questsListFileName);
  virtual ~QuestBook() = default;

  void update(const Quest::Objective::Type objectiveType);

  void unlockQuest(Quest* quest);
  void startQuest(Quest* quest);
  void markCompleted(Quest* quest);

  void unlockQuest(const std::string& questJsonFileName);
  void startQuest(const std::string& questJsonFileName);
  void markCompleted(const std::string& questJsonFileName);

  std::vector<Quest*> getAllQuests() const;
  const std::vector<Quest*>& getInProgressQuests() const;
  const std::vector<Quest*>& getCompletedQuests() const;

 private:
  std::unordered_map<std::string, std::unique_ptr<Quest>> _questMapper;
  std::vector<Quest*> _inProgressQuests;
  std::vector<Quest*> _completedQuests;
};

} // namespace vigilante

#endif // VIGILANTE_QUEST_BOOK_H_
