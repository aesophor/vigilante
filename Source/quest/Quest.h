// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
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
  explicit Quest(const std::string& jsonFileName) : _questProfile{jsonFileName} {}
  virtual ~Quest() = default;

  class Objective {
   public:
    virtual ~Objective() = default;

    enum Type {
      GENERAL,
      KILL,
      COLLECT,
      ESCORT,
      DELIVERY,
      INTERACT_WITH,
    };

    virtual bool isCompleted() const = 0;

    inline Objective::Type getObjectiveType() const { return _objectiveType; }
    inline const std::string& getDesc() const { return _desc; }

   protected:
    Objective(Objective::Type objectiveType, const std::string& desc)
        : _objectiveType{objectiveType},
          _desc{desc} {}

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

  void unlock() { _isUnlocked = true; }
  void advanceStage();

  inline bool isUnlocked() const { return _isUnlocked; }
  inline bool isCompleted() const { return _currentStageIdx >= static_cast<int>(_questProfile.stages.size()); }

  inline const Quest::Profile& getQuestProfile() const { return _questProfile; }
  inline const Quest::Stage& getCurrentStage() const { return _questProfile.stages.at(_currentStageIdx); }
  inline int getCurrentStageIdx() const { return _currentStageIdx; }
  inline void setCurrentStageIdx(const int stageIdx) { _currentStageIdx = stageIdx; }

 private:
  Quest::Profile _questProfile;
  bool _isUnlocked{};
  int _currentStageIdx{-1};
};

}  // namespace vigilante

#endif  // VIGILANTE_QUEST_H_
