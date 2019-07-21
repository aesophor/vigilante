// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ASSET_MANAGER_H_
#define VIGILANTE_ASSET_MANAGER_H_

#include <string>

namespace vigilante {

namespace asset_manager {

const std::string kExpPointTable = "Resources/Gameplay/exp_point_table.txt";
const std::string kSpritesheetList = "Resources/Texture/spritesheets.txt";
const std::string kQuestList = "Resources/Gameplay/quest_list.txt";

// Fonts
const std::string kRegularFont = "Font/HeartbitXX.ttf";
const std::string kBoldFont = "Font/HeartbitXX2Px.ttf";
const std::string kTitleFont = "Font/MatchupPro.ttf";
const float kRegularFontSize = 16.0f;

// MainMenu
const std::string kMainMenuBg = "Texture/ui/mainmenu_bg.png";

// Shade (an overlay image used for screen fadeout/fadein effects)
const std::string kShade = "Texture/ui/shade.png";

// Hud
const std::string kHud = "Texture/ui/hud/";
const std::string kBarLeftPadding = kHud + "bar_padding_left.png";
const std::string kBarRightPadding = kHud + "bar_padding_right.png";
const std::string kHealthBar = kHud + "health_bar.png";
const std::string kMagickaBar = kHud + "magicka_bar.png";
const std::string kStaminaBar = kHud + "stamina_bar.png";
const std::string kEquippedWeaponBg = kHud + "equipped_weapon_slot.png";
const std::string kEquippedWeaponDescBg = kHud + "item_desc.png";

// Dialogue
const std::string kDialogue = "Texture/ui/dialogue/";
const std::string kDialogueTriangle = kDialogue + "triangle.png";

// PauseMenu
const std::string kPauseMenu = "Texture/ui/pause_menu/";
const std::string kPauseMenuBg = kPauseMenu + "pause.png";

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
const std::string kEmptyImage = "Texture/empty.png";

// Spritesheets
void loadSpritesheets(const std::string& spritesheetListFileName);

} // namespace asset_manager

} // namespace vigilante

#endif // VIGILANTE_ASSET_MANAGER_H_
