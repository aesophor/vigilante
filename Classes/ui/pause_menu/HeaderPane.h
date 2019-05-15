#ifndef VIGILANTE_HEADER_PANE_H_
#define VIGILANTE_HEADER_PANE_H_

#include <vector>
#include <string>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"

#include "AbstractPane.h"
#include "PauseMenu.h"

namespace vigilante {

class PauseMenu;

class HeaderPane : public AbstractPane {
 public:
  HeaderPane(PauseMenu* pauseMenu);
  virtual ~HeaderPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

  void selectPrev();
  void selectNext();
  int getCurrentIndex() const;

 private:
  enum Options {
    INVENTORY,
    EQUIPMENT,
    SKILLS,
    QUESTS,
    OPTIONS
  };

  static const float _kOptionGap;
  static const int _kOptionCount;

  PauseMenu* _pauseMenu;

  std::vector<cocos2d::Label*> _labels;
  int _currentIndex;
};

} // namespace vigilante

#endif // VIGILANTE_HEADER_PANE_H_
