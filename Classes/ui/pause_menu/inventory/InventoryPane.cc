#include "InventoryPane.h"

using std::string;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;

namespace vigilante {

const string InventoryPane::_kPauseMenu = "Texture/UI/PauseMenu/";
const string InventoryPane::_kInventoryBg = _kPauseMenu + "inventory_bg.png";
const string InventoryPane::_kFont = "Font/HeartbitXX2Px.ttf";
const float InventoryPane::_kFontSize = 16.0f;

InventoryPane::InventoryPane()
    : _layout(Layout::create()),
      _background(ImageView::create(_kInventoryBg)),
      _itemDesc(Label::createWithTTF("Item description here", _kFont, _kFontSize)) {
  _layout->setLayoutType(Layout::Type::RELATIVE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)
  _layout->addChild(_background);

  _itemDesc->getFontAtlas()->setAliasTexParameters();
  _itemDesc->setAnchorPoint({0, 1});
  _itemDesc->setPosition({10, -137});
  _layout->addChild(_itemDesc);
}

Layout* InventoryPane::getLayout() const {
  return _layout;
}

} // namespace vigilante
