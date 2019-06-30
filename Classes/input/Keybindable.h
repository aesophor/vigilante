#ifndef VIGILANTE_KEYBINDABLE_H_
#define VIGILANTE_KEYBINDABLE_H_

#include <string>

namespace vigilante {

class Keybindable {
 public:
  virtual ~Keybindable() = default;

  virtual const std::string& getHotkey() const = 0;
  virtual void setHotkey(const std::string& hotkey) = 0;
};

} // namespace vigilante

#endif // VIGILANTE_KEYBINDABLE_H_
