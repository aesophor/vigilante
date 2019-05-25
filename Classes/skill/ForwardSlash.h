#ifndef VIGILANTE_FORWARD_SLASH_H_
#define VIGILANTE_FORWARD_SLASH_H_

#include <string>

#include "Skill.h"

namespace vigilante {

class Character;

class ForwardSlash : public Skill {
 public:
  ForwardSlash(const std::string& jsonFileName, Character* user);
  virtual ~ForwardSlash() = default;

  virtual void import(const std::string& jsonFileName) override; // Skill
  virtual bool canActivate() override; // Skill
  virtual void activate() override; // Skill
  virtual Skill::Profile& getSkillProfile() override; // Skill

 private:
  Skill::Profile _skillProfile;
  Character* _user;

  bool _hasActivated;
};

} // namespace vigilante

#endif // VIGILANTE_FORWARD_SLASH_H_
