#include "AbstractPane.h"

using cocos2d::ui::Layout;

namespace vigilante {

AbstractPane::AbstractPane() : _layout(Layout::create()) {}

AbstractPane::AbstractPane(Layout* layout) : _layout(layout) {}


Layout* AbstractPane::getLayout() const {
  return _layout;
}

} // namespace vigilante
