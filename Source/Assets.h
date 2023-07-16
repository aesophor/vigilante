// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ASSETS_H_
#define VIGILANTE_ASSETS_H_

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace vigilante::assets {

// Dirs
inline const fs::path kDataDir = "Data";
inline const fs::path kGameplayDir = kDataDir / "gameplay";
inline const fs::path kFontDir = "Font";
inline const fs::path kMapDir = "Map";
inline const fs::path kMusicDir = "Music";
inline const fs::path kSfxDir = "Sfx";
inline const fs::path kSfxEnvDir = kSfxDir / "environment";
inline const fs::path kSfxInvDir = kSfxDir / "inventory";
inline const fs::path kTextureDir = "Texture";
inline const fs::path kUIDir = kTextureDir / "ui";

inline const fs::path kExpPointTable = kGameplayDir / "exp_point_table.txt";
inline const fs::path kItemPriceTable = kGameplayDir / "item_price_table.txt";
inline const fs::path kQuestsList = kGameplayDir / "quests_list.txt";
inline const fs::path kSpritesheetsList = kTextureDir / "spritesheets.txt";
inline const fs::path kPlayerJson = kDataDir / "character/alucard.json";

// Fonts
inline constexpr float kRegularFontSize = 16.0f;
inline constexpr float kSmallFontSize = 12.0f;
inline const fs::path kRegularFont = kFontDir / "at01.ttf";
inline const fs::path kBoldFont = kFontDir / "HeartbitXX2Px.ttf";
inline const fs::path kTitleFont = kFontDir / "MatchupPro.ttf";

// MainMenu
inline const fs::path kMainMenuBg = kUIDir / "mainmenu_bg.png";

// NewGame initial GameMap
inline const fs::path kNewGameInitialMap = kMapDir / "VampireCastle/PrisonCell.tmx";

// Common UI Textures
inline const fs::path kShade = kUIDir / "shade.png";
inline const fs::path kHudDir = kUIDir / "hud";
inline const fs::path kBarLeftPadding = kHudDir / "bar_padding_left.png";
inline const fs::path kBarRightPadding = kHudDir / "bar_padding_right.png";
inline const fs::path kHealthBar = kHudDir / "health_bar.png";
inline const fs::path kMagickaBar = kHudDir / "magicka_bar.png";
inline const fs::path kStaminaBar = kHudDir / "stamina_bar.png";
inline const fs::path kEquippedWeaponBg = kHudDir / "equipped_weapon_slot.png";
inline const fs::path kEquippedWeaponDescBg = kHudDir / "item_desc.png";

// Dialogue
inline const fs::path kDialogueDir = kUIDir / "dialogue";
inline const fs::path kDialogueMenuBg = kDialogueDir / "dialogue_menu_bg.png";
inline const fs::path kDialogueTriangle = kDialogueDir / "triangle.png";

// PauseMenu
inline const fs::path kPauseMenuDir = kUIDir / "pause_menu";
inline const fs::path kPauseMenuBg = kPauseMenuDir / "pause.png";
inline const fs::path kStatsBg = kPauseMenuDir / "stats_bg.png";
inline const fs::path kInventoryBg = kPauseMenuDir / "inventory_bg.png";
inline const fs::path kTabRegular = kPauseMenuDir / "tab_regular.png";
inline const fs::path kTabHighlighted = kPauseMenuDir / "tab_highlighted.png";
inline const fs::path kItemRegular = kPauseMenuDir / "item_regular.png";
inline const fs::path kItemHighlighted = kPauseMenuDir / "item_highlighted.png";
inline const fs::path kScrollBar = kUIDir / "scroll_bar.png";
inline const fs::path kEquipmentRegular = kPauseMenuDir / "equipment_regular.png";
inline const fs::path kEquipmentHighlighted = kPauseMenuDir / "equipment_highlighted.png";

// Item icons
inline const fs::path kItemIconDir = kTextureDir / "item";
inline const fs::path kEmptyImage = kTextureDir / "empty.png";

// Window
inline const fs::path kWindowDir = kUIDir / "window";
inline const fs::path kWindowContentBg = kWindowDir / "window_content_bg.png";
inline const fs::path kWindowTopLeftBg = kWindowDir / "window_top_left_bg.png";
inline const fs::path kWindowTopRightBg = kWindowDir / "window_top_right_bg.png";
inline const fs::path kWindowBottomLeftBg = kWindowDir / "window_bottom_left_bg.png";
inline const fs::path kWindowBottomRightBg = kWindowDir / "window_bottom_right_bg.png";
inline const fs::path kWindowTopBg = kWindowDir / "window_top_bg.png";
inline const fs::path kWindowLeftBg = kWindowDir / "window_left_bg.png";
inline const fs::path kWindowRightBg = kWindowDir / "window_right_bg.png";
inline const fs::path kWindowBottomBg = kWindowDir / "window_bottom_bg.png";
inline const fs::path kTextFieldBg = kUIDir / "text_field_bg.png";

// Trade
inline const fs::path kTradeDir = kUIDir / "trade";
inline const fs::path kTradeBg = kTradeDir / "trade_bg.png";

// Control Hints
inline const fs::path kControlHintsDir = kUIDir / "control_hints";

// Important items
inline const fs::path kGoldCoin = kDataDir / "item/misc/gold_coin.json";

// BGM
inline const fs::path kMainThemeBgm = kMusicDir / "MainMenu.mp3";

// Sound Effects
inline const fs::path kSfxOpenClosePauseMenu = kSfxInvDir / "open_and_close.wav";
inline const fs::path kSfxEquipUnequipItem = kSfxInvDir / "equip.wav";
inline const fs::path kSfxChestOpened = kSfxEnvDir / "chest_opened.mp3";
inline const fs::path kSfxDoorLocked = kSfxEnvDir / "locked.mp3";
inline const fs::path kSfxDoorUnlocked = kSfxEnvDir / "unlocked.mp3";

void loadSpritesheets(const fs::path& spritesheetsListFileName);

}  // namespace vigilante::assets

#endif  // VIGILANTE_ASSETS_H_
