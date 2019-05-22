#ifndef VIGILANTE_SPELL_H_
#define VIGILANTE_SPELL_H_

namespace vigilante {

class Character;

class Spell {
 public:
  Spell() = default;
  virtual ~Spell() = default;

  virtual void activate() = 0;
  virtual void onHit() = 0;

  virtual Character* getSpellUser() const = 0;
  virtual int getDamage() const = 0;
};

} // namespace vigilante

#endif // VIGILANTE_SPELL_H_
