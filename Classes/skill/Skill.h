#ifndef VIGILANTE_SKILL_H_
#define VIGILANTE_SKILL_H_

#include <string>

#include "Importable.h"

namespace vigilante {

class Character;

// Skill interface
class Skill : public Importable {
 public:
  struct Profile {
    Profile(const std::string& jsonFileName);
    virtual ~Profile() = default;

    std::string characterFramesName;
    float framesDuration;
    float frameInterval;

    std::string textureResDir; // the animation of skill itself
    std::string name;
    std::string desc;
    
    int requiredLevel;
    float cooldown;

    int physicalDamage;
    int magicalDamage;

    int healthDelta;
    int magickaDelta;
    int staminaDelta;
  };

  virtual ~Skill() = default;
  virtual void import(const std::string& jsonFileName) = 0; // Importable

  virtual bool canActivate() = 0;
  virtual void activate() = 0;
  virtual Skill::Profile& getSkillProfile() = 0;
};

} // namespace vigilante

#endif // VIGILANTE_SKILL_H_
