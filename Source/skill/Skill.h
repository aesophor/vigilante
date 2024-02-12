// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_SKILL_H_
#define VIGILANTE_SKILL_H_

#include <memory>
#include <string>

#include <axmol.h>

#include "Importable.h"
#include "input/Keybindable.h"

namespace vigilante {

class Character;

// Skill interface
class Skill : public Importable, public Keybindable, public std::enable_shared_from_this<Skill> {
 public:
  enum Type {
    MELEE,
    MAGIC,
    DARK,
    BUFF,
    SIZE
  };

  struct Profile {
    explicit Profile(const std::string& jsonFilePath);
    virtual ~Profile() = default;

    std::string jsonFilePath;
    Skill::Type skillType;

    std::string characterFramesName;
    std::string textureResDir;  // the animation of skill itself
    float spriteOffsetX;
    float spriteOffsetY;
    float spriteScaleX;
    float spriteScaleY;
    float framesDuration;
    float frameInterval;

    std::string name;
    std::string desc;
    bool isToggleable;
    bool shouldForkInstance;
    int requiredLevel;
    float cooldown;

    int physicalDamage;
    int magicalDamage;
    int deltaHealth;
    int deltaMagicka;
    int deltaStamina;
    int numTimesInflictDamage;
    float damageInflictionInterval;

    std::string sfxActivate;
    std::string sfxHit;

    ax::EventKeyboard::KeyCode hotkey;
  };

  // Create a skill by automatically deducing its concrete type
  // based on the json passed in.
  static std::shared_ptr<Skill> create(const std::string& jsonFilePath, Character* user);

  virtual ~Skill() = default;
  virtual void import(const std::string& jsonFilePath) = 0;  // Importable

  virtual ax::EventKeyboard::KeyCode getHotkey() const = 0;  // Keybindable
  virtual void setHotkey(ax::EventKeyboard::KeyCode hotkey) = 0;  // Keybindable

  virtual bool canActivate() = 0;
  virtual void activate() = 0;
  virtual void deactivate() = 0;

  virtual Skill::Profile& getSkillProfile() = 0;
  virtual const std::string& getName() const = 0;
  virtual const std::string& getDesc() const = 0;
  virtual std::string getIconPath() const = 0;
};

}  // namespace vigilante

#endif  // VIGILANTE_SKILL_H_
