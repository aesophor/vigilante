#ifndef VIGILANTE_GAME_ASSET_MANAGER_H_
#define VIGILANTE_GAME_ASSET_MANAGER_H_

#include <string>

namespace vigilante {

namespace asset_manager {

// Fonts
const std::string kRegularFont = "Font/HeartbitXX.ttf";
const std::string kBoldFont = "Font/HeartbitXX2Px.ttf";
const std::string kTitleFont = "Font/MatchupPro.ttf";
const float kRegularFontSize = 16.0f;

// PauseMenu - InventoryPane
const std::string kPauseMenu = "Texture/UI/PauseMenu/";
const std::string kInventoryBg = kPauseMenu + "inventory_bg.png";
const std::string kTabRegular = kPauseMenu + "tab_regular.png";
const std::string kTabHighlighted = kPauseMenu + "tab_highlighted.png";
const std::string kItemRegular = kPauseMenu + "item_regular.png";
const std::string kItemHighlighted = kPauseMenu + "item_highlighted.png";

// Item icons
const std::string kItemIcons = "Texture/Item/";
const std::string kEmptyItemIcon = kItemIcons + "Empty/icon.png";
const std::string kRustyAxeIcon = kItemIcons + "RustyAxe/icon.png";

} // namespace asset_manager

} // namespace vigilante

#endif // VIGILANTE_GAME_ASSET_MANAGER_H_
