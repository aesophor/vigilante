#ifndef VIGILANTE_QUEST_H_
#define VIGILANTE_QUEST_H_

#include <string>
#include <vector>

#include "Importable.h"

namespace vigilante {

class Quest : public Importable {
 public:
  Quest(const std::string& jsonFileName);
  virtual ~Quest() = default;

  struct Stage {
    Stage(const std::string& objective);
    virtual ~Stage() = default;

    bool isFinished;
    std::string objective;
    std::string questDesc; // optionally update quest desc when this stage is reached.
  };

  struct Profile {
    Profile(const std::string& jsonFileName);
    virtual ~Profile() = default;

    std::string jsonFileName;
    std::string title;
    std::string desc;
    std::vector<Quest::Stage> stages;
  };

  virtual void import(const std::string& jsonFileName) override; // Importable
  
  void advanceStage();
  bool isCompleted() const;
  const Quest::Profile& getQuestProfile() const;

 private:
  Quest::Profile _questProfile;
  int _currentStageIdx;
};

} // namespace vigilante

#endif // VIGILANTE_QUEST_H_
