#ifndef VIGILANTE_BACK_DASH_H_
#define VIGILANTE_BACK_DASH_H_

#include <string>

#include "Skill.h"

namespace vigilante {

class Character;

class BackDash : public Skill {
 public:
  BackDash(const std::string& jsonFileName);
  virtual ~BackDash() = default;

  virtual void import(const std::string& jsonFileName) override; // Skill
  virtual bool canActivate(Character* user) override; // Skill
  virtual void activate(Character* user) override; // Skill

  virtual Skill::Profile& getSkillProfile() override; // Skill
  virtual const std::string& getName() const override; // Skill
  virtual const std::string& getDesc() const override; // Skill
  virtual std::string getIconPath() const override; // Skill

 private:
  Skill::Profile _skillProfile;
  bool _hasActivated;
};

} // namespace vigilante

#endif // VIGILANTE_BACK_DASH_H_
