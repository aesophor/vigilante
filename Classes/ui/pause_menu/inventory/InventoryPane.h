#ifndef VIGILANTE_INVENTORY_PANE_H_
#define VIGILANTE_INVENTORY_PANE_H_

#include <string>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"

namespace vigilante {

class InventoryPane {
 public:
  InventoryPane();
  virtual ~InventoryPane() = default;

  cocos2d::ui::Layout* getLayout() const;

 private:
  // Paths to resource files.
  static const std::string _kPauseMenu;
  static const std::string _kInventoryBg;
  static const std::string _kFont;
  static const float _kFontSize;

  cocos2d::ui::Layout* _layout;
  cocos2d::ui::ImageView* _background;
  cocos2d::Label* _itemDesc;
};

} // namespace vigilante

#endif // VIGILANTE_INVENTORY_PANE_H_
