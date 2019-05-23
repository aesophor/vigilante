#ifndef VIGILANTE_CHARACTER_PROFILE_H_
#define VIGILANTE_CHARACTER_PROFILE_H_

#include <string>
#include <array>

#include "Importable.h"

namespace vigilante {

struct CharacterProfile : public Importable {
  CharacterProfile() = default;
  virtual void import(const std::string& jsonFileName) override;

  std::string textureResPath;
  float spriteOffsetX;
  float spriteOffsetY;
  float spriteScaleX;
  float spriteScaleY;
  int frameWidth;
  int frameHeight;
  std::array<float, 14> frameInterval;

  std::string name;
  int level;
  int exp;

  int fullHealth;
  int fullStamina;
  int fullMagicka;
  int health;
  int stamina;
  int magicka;

  int strength;
  int dexterity;
  int intelligence;
  int luck;

  int bodyWidth;
  int bodyHeight;
  float moveSpeed;
  float jumpHeight;

  float attackForce;
  float attackTime;
  float attackRange;

  int baseMeleeDamage;
};

} // namespace vigilante

#endif // VIGILANTE_CHARACTER_PROFILE_H_
