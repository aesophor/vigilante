#ifndef VIGILANTE_HEADER_PANE_H_
#define VIGILANTE_HEADER_PANE_H_

#include <vector>
#include <string>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"

namespace vigilante {

class HeaderPane {
 public:
  HeaderPane();
  virtual ~HeaderPane() = default;

  virtual void selectPrev();
  virtual void selectNext();
  cocos2d::ui::Layout* getLayout() const;

 private:
  enum Options {
    INVENTORY,
    EQUIPMENT,
    SKILLS,
    QUESTS,
    OPTIONS
  };

  static const std::string _kFont;
  static const float _kFontSize;
  static const float _kOptionGap;
  static const int _kOptionCount;

  cocos2d::ui::Layout* _layout;
  std::vector<cocos2d::Label*> _labels;
  int _current;
};

} // namespace vigilante

#endif // VIGILANTE_HEADER_PANE_H_
