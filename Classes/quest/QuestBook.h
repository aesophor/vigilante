#ifndef VIGILANTE_QUEST_BOOK_H_
#define VIGILANTE_QUEST_BOOK_H_

#include <string>
#include <deque>

#include "Importable.h"
#include "quest/Quest.h"

namespace vigilante {

class QuestBook : public Importable {
 public:
  QuestBook() = default;
  QuestBook(const std::string& jsonFileName);
  virtual ~QuestBook() = default;

  virtual void import(const std::string& jsonFileName) override; // Importable

  void startQuest(const Quest& quest);

 private:
  std::deque<Quest> _quests;
};

} // namespace vigilante

#endif // VIGILANTE_QUEST_BOOK_H_
