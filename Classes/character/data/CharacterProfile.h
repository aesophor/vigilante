#ifndef VIGILANTE_CHARACTER_PROFILE_H_
#define VIGILANTE_CHARACTER_PROFILE_H_

#include <string>
#include <array>

namespace vigilante {

struct CharacterProfile {
  CharacterProfile(const std::string& jsonFileName);

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
  int str;
  int dex;
  int _int;
  int luk;

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
