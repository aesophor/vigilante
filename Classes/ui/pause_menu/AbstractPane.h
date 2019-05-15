#ifndef VIGILANTE_ABSTRACT_PANE_H_
#define VIGILANTE_ABSTRACT_PANE_H_

#include "cocos2d.h"
#include "ui/UILayout.h"

namespace vigilante {

class AbstractPane {
 public:
  virtual ~AbstractPane() = default;
  virtual void update() = 0;
  virtual void handleInput() = 0;

  bool isVisible() const;
  void setVisible(bool visible) const;
  cocos2d::ui::Layout* getLayout() const;

 protected:
  AbstractPane(); // install cocos2d's UILayout
  AbstractPane(cocos2d::ui::Layout* layout); // install custom layout
  cocos2d::ui::Layout* _layout; // auto-release object
};

} // namespace vigilante

#endif // VIGILANTE_ABSTRACT_PANE_H_
