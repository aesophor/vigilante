// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ABSTRACT_PANE_H_
#define VIGILANTE_ABSTRACT_PANE_H_

#include <cocos2d.h>
#include <ui/UILayout.h>

namespace vigilante {

// Forward declaration of PauseMenu class.
// We only need to know its existence, so
// we don't have to include the entire class.
class PauseMenu;

class AbstractPane {
 public:
  virtual ~AbstractPane() = default;

  virtual void update() = 0;
  virtual void handleInput() = 0;

  bool isVisible() const;
  void setVisible(bool visible) const;

  PauseMenu* getPauseMenu() const;
  cocos2d::ui::Layout* getLayout() const;

 protected:
  AbstractPane(PauseMenu* pauseMenu); // install cocos2d's UILayout
  AbstractPane(PauseMenu* pauseMenu, cocos2d::ui::Layout* layout); // install custom layout

  PauseMenu* _pauseMenu;
  cocos2d::ui::Layout* _layout; // auto-release object
};

} // namespace vigilante

#endif // VIGILANTE_ABSTRACT_PANE_H_
