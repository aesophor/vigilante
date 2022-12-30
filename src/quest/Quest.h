// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_QUEST_H_
#define VIGILANTE_QUEST_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Importable.h"

namespace vigilante {

class Quest : public Importable {
 public:
  explicit Quest(const std::string& jsonFileName);
  virtual ~Quest() = default;

  class Objective {
   public:
    virtual ~Objective() = default;

    enum Type {
      KILL,
      COLLECT,
      ESCORT,
      DELIVERY,
      TALK_TO,
      AD_HOC
    };

    virtual bool isCompleted() const = 0;

    inline Objective::Type getObjectiveType() const { return _objectiveType; }
    inline const std::string& getDesc() const { return _desc; }

   protected:
    Objective(Objective::Type objectiveType, const std::string& desc);

    Objective::Type _objectiveType;
    std::string _desc;
  };

  struct Stage final {
    std::string getHint() const;

    bool isFinished;
    std::string questDesc;  // optionally update questDesc when this stage is reached.
    std::unique_ptr<Objective> objective; 
    std::vector<std::string> cmds;
  };

  struct Profile final {
    explicit Profile(const std::string& jsonFileName);

    std::string jsonFileName;
    std::string title;
    std::string desc;
    std::vector<Quest::Stage> stages;
  };

  virtual void import(const std::string& jsonFileName) override;  // Importable
  
  void unlock();
  void advanceStage();

  inline bool isUnlocked() const { return _isUnlocked; }
  inline bool isCompleted() const {
    return _currentStageIdx >= (int) _questProfile.stages.size();
  }

  inline const Quest::Profile& getQuestProfile() const { return _questProfile; }
  inline const Quest::Stage& getCurrentStage() const {
    return _questProfile.stages.at(_currentStageIdx);
  }

 private:
  Quest::Profile _questProfile;
  bool _isUnlocked;
  int _currentStageIdx;
};

}  // namespace vigilante

#endif  // VIGILANTE_QUEST_H_
