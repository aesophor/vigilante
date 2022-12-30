// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ASSETS_H_
#define VIGILANTE_ASSETS_H_

#include <string>

namespace vigilante::assets {

inline const std::string kExpPointTable = "Gameplay/exp_point_table.txt";
inline const std::string kItemPriceTable = "Gameplay/item_price_table.txt";
inline const std::string kSpritesheetsList = "Texture/spritesheets.txt";
inline const std::string kQuestsList = "Gameplay/quests_list.txt";
inline const std::string kPlayerJson = "Database/character/vlad.json";

// Fonts
inline const std::string kRegularFont = "Font/at01.ttf";
inline const std::string kBoldFont = "Font/HeartbitXX2Px.ttf";
inline const std::string kTitleFont = "Font/MatchupPro.ttf";
inline constexpr float kRegularFontSize = 16.0f;
inline constexpr float kSmallFontSize = 12.0f;

// MainMenu
inline const std::string kMainMenuBg = "Texture/ui/mainmenu_bg.png";

// NewGame initial GameMap
inline const std::string kNewGameInitialMap = "Map/VampireCastle/PrisonCell.tmx";

// Shade (an overlay image used for screen fadeout/fadein effects)
inline const std::string kShade = "Texture/ui/shade.png";

// Hud
inline const std::string kHud = "Texture/ui/hud/";
inline const std::string kBarLeftPadding = kHud + "bar_padding_left.png";
inline const std::string kBarRightPadding = kHud + "bar_padding_right.png";
inline const std::string kHealthBar = kHud + "health_bar.png";
inline const std::string kMagickaBar = kHud + "magicka_bar.png";
inline const std::string kStaminaBar = kHud + "stamina_bar.png";
inline const std::string kEquippedWeaponBg = kHud + "equipped_weapon_slot.png";
inline const std::string kEquippedWeaponDescBg = kHud + "item_desc.png";

// Dialogue
inline const std::string kDialogue = "Texture/ui/dialogue/";
inline const std::string kDialogueMenuBg = kDialogue + "dialogue_menu_bg.png";
inline const std::string kDialogueTriangle = kDialogue + "triangle.png";

// PauseMenu
inline const std::string kPauseMenu = "Texture/ui/pause_menu/";
inline const std::string kPauseMenuBg = kPauseMenu + "pause.png";

// PauseMenu - StatsPane
inline const std::string kStatsBg = kPauseMenu + "stats_bg.png";

// PauseMenu - InventoryPane
inline const std::string kInventoryBg = kPauseMenu + "inventory_bg.png";
inline const std::string kTabRegular = kPauseMenu + "tab_regular.png";
inline const std::string kTabHighlighted = kPauseMenu + "tab_highlighted.png";
inline const std::string kItemRegular = kPauseMenu + "item_regular.png";
inline const std::string kItemHighlighted = kPauseMenu + "item_highlighted.png";
inline const std::string kScrollBar = "Texture/ui/scroll_bar.png";

// PauseMenu - EquipmentPane
inline const std::string kEquipmentRegular = kPauseMenu + "equipment_regular.png";
inline const std::string kEquipmentHighlighted = kPauseMenu + "equipment_highlighted.png";

// Item icons
inline const std::string kItemIcons = "Texture/item/";
inline const std::string kEmptyImage = "Texture/empty.png";

// Window
inline const std::string kWindow = "Texture/ui/window/";
inline const std::string kWindowContentBg = kWindow + "window_content_bg.png";
inline const std::string kWindowTopLeftBg = kWindow + "window_top_left_bg.png";
inline const std::string kWindowTopRightBg = kWindow + "window_top_right_bg.png";
inline const std::string kWindowBottomLeftBg = kWindow + "window_bottom_left_bg.png";
inline const std::string kWindowBottomRightBg = kWindow + "window_bottom_right_bg.png";
inline const std::string kWindowTopBg = kWindow + "window_top_bg.png";
inline const std::string kWindowLeftBg = kWindow + "window_left_bg.png";
inline const std::string kWindowRightBg = kWindow + "window_right_bg.png";
inline const std::string kWindowBottomBg = kWindow + "window_bottom_bg.png";

inline const std::string kTextFieldBg = "Texture/ui/text_field_bg.png";

// Trade
inline const std::string kTrade = "Texture/ui/trade/";
inline const std::string kTradeBg = kTrade + "trade_bg.png";

// Control Hints
inline const std::string kControlHints = "Texture/ui/control_hints/";

// Important items
inline const std::string kGoldCoin = "Database/item/misc/gold_coin.json";

// BGM
inline const std::string kBgm = "Music/";
inline const std::string kMainThemeBgm = kBgm + "main_theme.mp3";

// Spritesheets
void loadSpritesheets(const std::string& spritesheetsListFileName);

}  // namespace vigilante::assets

#endif  // VIGILANTE_ASSETS_H_
