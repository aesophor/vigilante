#include "QuestBook.h"

using std::string;

namespace vigilante {

QuestBook::QuestBook(const string& jsonFileName) {
  import(jsonFileName);
}


void QuestBook::import(const string& jsonFileName) {

}


void QuestBook::startQuest(const Quest& quest) {

}

} // namespace vigilante
