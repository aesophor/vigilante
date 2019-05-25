#ifndef VIGILANTE_REGENERATABLE_H_
#define VIGILANTE_REGENERATABLE_H_

namespace vigilante {

// Stats (health, magicka, stamina) regeneration interface.
class Regeneratable {
 public:
  virtual ~Regeneratable() = default;

 protected:
  virtual void regenHealth(int deltaHealth) = 0;
  virtual void regenMagicka(int deltaMagicka) = 0;
  virtual void regenStamina(int deltaStamina) = 0;
};

} // namespace vigilante

#endif // VIGILANTE_REGENERATABLE_H_
