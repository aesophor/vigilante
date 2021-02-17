// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ASSET_MANAGER_H_
#define VIGILANTE_ASSET_MANAGER_H_

#include <string>

namespace vigilante {

namespace asset_manager {

#ifdef __linux__
const std::string kExpPointTable = "Resources/Gameplay/exp_point_table.txt";
const std::string kItemPriceTable = "Resources/Gameplay/item_price_table.txt";
const std::string kSpritesheetsList = "Resources/Texture/spritesheets.txt";
const std::string kQuestsList = "Resources/Gameplay/quests_list.txt";
const std::string kPlayerJson = "Resources/Database/character/vlad.json";
#else
const std::string kExpPointTable = "Gameplay/exp_point_table.txt";
const std::string kItemPriceTable = "Gameplay/item_price_table.txt";
const std::string kSpritesheetsList = "Texture/spritesheets.txt";
const std::string kQuestsList = "Gameplay/quests_list.txt";
const std::string kPlayerJson = "Database/character/vlad.json";
#endif

// Fonts
const std::string kRegularFont = "Font/at01.ttf";
const std::string kBoldFont = "Font/HeartbitXX2Px.ttf";
const std::string kTitleFont = "Font/MatchupPro.ttf";
const float kRegularFontSize = 16.0f;
const float kSmallFontSize = 12.0f;

// MainMenu
const std::string kMainMenuBg = "Texture/ui/mainmenu_bg.png";

// NewGame initial GameMap
const std::string kNewGameInitialMap = "Map/VampireCastle/PrisonCell.tmx";

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
const std::string kDialogueMenuBg = kDialogue + "dialogue_menu_bg.png";
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
const std::string kScrollBar = "Texture/ui/scroll_bar.png";

// PauseMenu - EquipmentPane
const std::string kEquipmentRegular = kPauseMenu + "equipment_regular.png";
const std::string kEquipmentHighlighted = kPauseMenu + "equipment_highlighted.png";

// Item icons
const std::string kItemIcons = "Texture/item/";
const std::string kEmptyImage = "Texture/empty.png";

// Window
const std::string kWindow = "Texture/ui/window/";
const std::string kWindowContentBg = kWindow + "window_content_bg.png";
const std::string kWindowTopLeftBg = kWindow + "window_top_left_bg.png";
const std::string kWindowTopRightBg = kWindow + "window_top_right_bg.png";
const std::string kWindowBottomLeftBg = kWindow + "window_bottom_left_bg.png";
const std::string kWindowBottomRightBg = kWindow + "window_bottom_right_bg.png";
const std::string kWindowTopBg = kWindow + "window_top_bg.png";
const std::string kWindowLeftBg = kWindow + "window_left_bg.png";
const std::string kWindowRightBg = kWindow + "window_right_bg.png";
const std::string kWindowBottomBg = kWindow + "window_bottom_bg.png";

const std::string kTextFieldBg = "Texture/ui/text_field_bg.png";

// Trade
const std::string kTrade = "Texture/ui/trade/";
const std::string kTradeBg = kTrade + "trade_bg.png";

// Control Hints
const std::string kControlHints = "Texture/ui/control_hints/";

// Important items
const std::string kGoldCoin = "Database/item/misc/gold_coin.json";

// BGM
const std::string kBgm = "Music/";
const std::string kMainThemeBgm = kBgm + "main_theme.mp3";

// Spritesheets
void loadSpritesheets(const std::string& spritesheetsListFileName);

}  // namespace asset_manager

}  // namespace vigilante

#endif  // VIGILANTE_ASSET_MANAGER_H_
