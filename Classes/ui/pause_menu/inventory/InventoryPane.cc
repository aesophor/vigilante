#include "InventoryPane.h"

#include "GameAssetManager.h"

using std::string;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using vigilante::asset_manager::kBoldFont;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kInventoryBg;
using vigilante::asset_manager::kTabRegular;
using vigilante::asset_manager::kTabHighlighted;

namespace vigilante {

InventoryPane::InventoryPane()
    : _layout(Layout::create()),
      _background(ImageView::create(kInventoryBg)),
      _itemDesc(Label::createWithTTF("Item desc", kRegularFont, kRegularFontSize)),
      _tabView(new TabView(kTabRegular, kTabHighlighted)),
      _itemScrollView(new ItemScrollView()) {
  _background->setAnchorPoint({0, 1});

  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)
  _layout->addChild(_background);

  // Place item category tabs.
  _tabView->addTab("EQUIP");
  _tabView->addTab("USE");
  _tabView->addTab("MISC");
  _tabView->selectTab(0);
  _layout->addChild(_tabView->getLayout());

  // Place item scroll view.
  _layout->addChild(_itemScrollView->getScrollView());

  // Place item description label.
  _itemDesc->getFontAtlas()->setAliasTexParameters();
  _itemDesc->setAnchorPoint({0, 1});
  _itemDesc->setPosition({10, -137});
  _layout->addChild(_itemDesc);
}


void InventoryPane::selectTab(Item::Type itemType) {
  _tabView->selectTab(static_cast<int>(itemType));
}

Layout* InventoryPane::getLayout() const {
  return _layout;
}

} // namespace vigilante
