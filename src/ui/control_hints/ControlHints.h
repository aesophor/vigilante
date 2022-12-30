// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CONTROL_HINTS_H_
#define VIGILANTE_CONTROL_HINTS_H_

#include <array>
#include <stack>
#include <string>
#include <vector>
#include <unordered_map>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>
#include "ui/Colorscheme.h"

namespace vigilante {

// ControlHints contains a fixed number of "profiles"
// for the game to switch between.
//
// Each profile may contains up to 3 ControlHints::Hint,
// where a ControlHints::Hint consists of an icon and a label.

class ControlHints {
 public:
   enum Profile {
    PAUSE_MENU_INVENTORY,
    PAUSE_MENU_EQUIPMENT,
    PAUSE_MENU_SKILLS,
    PAUSE_MENU_QUESTS,
    PAUSE_MENU_OPTIONS,
    GAME,
    SIZE
  };

  static ControlHints* getInstance();
  virtual ~ControlHints() = default;

  ControlHints::Profile getCurrentProfile() const;
  void switchToProfile(ControlHints::Profile profile);
  void pushProfile(ControlHints::Profile profile);
  void popProfile();

  bool isShown(const std::vector<cocos2d::EventKeyboard::KeyCode>& keyCodes);
  void insert(const std::vector<cocos2d::EventKeyboard::KeyCode>& keyCodes,
              const std::string& text,
              const cocos2d::Color4B& textColor=colorscheme::kWhite);
  void remove(const std::vector<cocos2d::EventKeyboard::KeyCode>& keyCodes);

  inline bool isVisible() const { return _layer->isVisible(); }
  inline void setVisible(bool visible) { _layer->setVisible(visible); }
  inline cocos2d::Layer* getLayer() const { return _layer; }

 private:
  class Hint final {
   public:
    Hint(const std::vector<cocos2d::EventKeyboard::KeyCode>& keyCodes,
         const std::string& text,
         const cocos2d::Color4B& textColor);

    cocos2d::Size getContentSize() const;
    inline cocos2d::ui::Layout* getLayout() const { return _layout; }
    inline const std::vector<cocos2d::EventKeyboard::KeyCode>& getKeyCodes() const {
      return _keyCodes;
    }

   private:
    static inline constexpr int _kIconLabelGap = 4;

    cocos2d::ui::Layout* _layout;
    std::vector<cocos2d::ui::ImageView*> _icons;
    cocos2d::Label* _label;
    std::vector<cocos2d::EventKeyboard::KeyCode> _keyCodes;
  };

  ControlHints();
  void normalize();
  void showAll();
  void hideAll();
  std::vector<ControlHints::Hint>& getCurrentProfileHints();

  static inline constexpr int _kHintGap = 16;

  cocos2d::Layer* _layer;
  std::array<std::vector<ControlHints::Hint>, ControlHints::Profile::SIZE> _profiles;
  std::stack<ControlHints::Profile> _currentProfileStack;
};

}  // namespace vigilante

#endif  // VIGILANTE_CONTROL_HINTS_H_
