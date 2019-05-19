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

// MainMenu
const std::string kMainMenuBg = "Texture/ui/mainmenu_bg.png";

// Shade (an overlay image used for screen fadeout/fadein effects)
const std::string kShade = "Texture/ui/shade.png";

// PauseMenu
const std::string kPauseMenu = "Texture/ui/pause_menu/";
const std::string kPauseMenuBg = kPauseMenu + "pause.png";

// PauseMenu - Dialog
const std::string kDialogTriangle = kPauseMenu + "triangle.png";

// PauseMenu - StatsPane
const std::string kStatsBg = kPauseMenu + "stats_bg.png";

// PauseMenu - InventoryPane
const std::string kInventoryBg = kPauseMenu + "inventory_bg.png";
const std::string kTabRegular = kPauseMenu + "tab_regular.png";
const std::string kTabHighlighted = kPauseMenu + "tab_highlighted.png";
const std::string kItemRegular = kPauseMenu + "item_regular.png";
const std::string kItemHighlighted = kPauseMenu + "item_highlighted.png";

// PauseMenu - EquipmentPane
const std::string kEquipmentRegular = kPauseMenu + "equipment_regular.png";
const std::string kEquipmentHighlighted = kPauseMenu + "equipment_highlighted.png";

// Item icons
const std::string kItemIcons = "Texture/item/";
const std::string kEmptyItemIcon = kItemIcons + "empty/icon.png";
const std::string kRustyAxeIcon = kItemIcons + "rusty_axe/icon.png";

// Character spritesheets
const std::string kPlayerSpritesheet = "Texture/character/player/sprites/player";

// Equipment spritesheets
const std::string kRustyAxeSpritesheet = "Texture/item/rusty_axe/sprites/rusty_axe";

// FX spritesheets
const std::string kDustSpritesheet = "Texture/fx/dust/dust";

std::string getFrameNamePrefix(const std::string& spritesheetPath);

} // namespace asset_manager

} // namespace vigilante

#endif // VIGILANTE_GAME_ASSET_MANAGER_H_
