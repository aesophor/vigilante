// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_QUEST_H_
#define VIGILANTE_QUEST_H_

#include <string>
#include <vector>

#include "Importable.h"

namespace vigilante {

class Quest : public Importable {
 public:
  Quest(const std::string& jsonFileName);
  virtual ~Quest();


  class Objective {
   public:
    virtual ~Objective() = default;

    enum Type {
      KILL,
      COLLECT,
      ESCORT,
      DELIVERY
    };

    virtual bool isCompleted() const = 0;
    Objective::Type getObjectiveType() const;
    const std::string& getDesc() const;

   protected:
    Objective(Objective::Type objectiveType, const std::string& desc);

    Objective::Type _objectiveType;
    std::string _desc;
  };


  struct Stage {
    Stage(Quest::Objective* objective);
    virtual ~Stage() = default;

    bool isFinished;
    std::string questDesc; // optionally update quest desc when this stage is reached
    Quest::Objective* objective;
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
  
  void unlock();
  void advanceStage();

  bool isUnlocked() const;
  bool isCompleted() const;

  const Quest::Profile& getQuestProfile() const;
  const Quest::Stage& getCurrentStage() const;

 private:
  Quest::Profile _questProfile;
  bool _isUnlocked;
  int _currentStageIdx;
};

} // namespace vigilante

#endif // VIGILANTE_QUEST_H_
