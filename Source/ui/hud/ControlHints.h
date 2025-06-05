// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_HUD_CONTROL_HINTS_H_
#define REQUIEM_UI_HUD_CONTROL_HINTS_H_

#include <array>
#include <stack>
#include <string>
#include <vector>
#include <unordered_map>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>

#include "ui/Colorscheme.h"

namespace requiem {

// ControlHints contains a fixed number of "profiles"
// for the game to switch between.
//
// Each profile may contains up to 3 ControlHints::Hint,
// where a ControlHints::Hint consists of an icon and a label.

class ControlHints final {
 public:
   enum Profile {
    PAUSE_MENU_INVENTORY,
    PAUSE_MENU_EQUIPMENT,
    PAUSE_MENU_SKILLS,
    PAUSE_MENU_QUESTS,
    PAUSE_MENU_OPTIONS,
    TRADE,
    GAME,
    SIZE
  };

  ControlHints();

  ControlHints::Profile getCurrentProfile() const;
  void switchToProfile(ControlHints::Profile profile);
  void pushProfile(ControlHints::Profile profile);
  void popProfile();

  bool isShown(const std::vector<ax::EventKeyboard::KeyCode>& keyCodes);
  void insert(const std::vector<ax::EventKeyboard::KeyCode>& keyCodes,
              const std::string& text,
              const ax::Color4B& textColor=colorscheme::kWhite);
  void remove(const std::vector<ax::EventKeyboard::KeyCode>& keyCodes);

  inline bool isVisible() const { return _layer->isVisible(); }
  inline void setVisible(bool visible) { _layer->setVisible(visible); }
  inline ax::Layer* getLayer() const { return _layer; }

 private:
  class Hint final {
   public:
    Hint(const std::vector<ax::EventKeyboard::KeyCode>& keyCodes,
         const std::string& text,
         const ax::Color4B& textColor);

    ax::Size getContentSize() const;
    inline ax::ui::Layout* getLayout() const { return _layout; }
    inline const std::vector<ax::EventKeyboard::KeyCode>& getKeyCodes() const {
      return _keyCodes;
    }

   private:
    static inline constexpr int _kIconLabelGap = 4;

    ax::ui::Layout* _layout;
    std::vector<ax::ui::ImageView*> _icons;
    ax::Label* _label;
    std::vector<ax::EventKeyboard::KeyCode> _keyCodes;
  };

  void normalize();
  void showAll();
  void hideAll();
  std::vector<ControlHints::Hint>& getCurrentProfileHints();

  static inline constexpr int _kHintGap = 16;

  ax::Layer* _layer;
  std::array<std::vector<ControlHints::Hint>, ControlHints::Profile::SIZE> _profiles;
  std::stack<ControlHints::Profile> _currentProfileStack;
};

}  // namespace requiem

#endif  // REQUIEM_UI_HUD_CONTROL_HINTS_H_
