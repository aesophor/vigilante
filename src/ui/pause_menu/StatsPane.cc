// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "StatsPane.h"

#include "AssetManager.h"
#include "character/Player.h"
#include "ui/Colorscheme.h"
#include "ui/pause_menu/PauseMenu.h"
#include "util/StringUtil.h"

using std::string;
using cocos2d::Label;
using cocos2d::Layer;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kStatsBg;

namespace vigilante {

const float StatsPane::_kPadLeft = 10.0f;
const float StatsPane::_kPadRight = 10.0f;
const float StatsPane::_kRowHeight = 16.0f;
const float StatsPane::_kSectionHeight = 8.0f;

StatsPane::StatsPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu, TableLayout::create()), // install TableLayout to base class
      _background(ImageView::create(kStatsBg)),
      _name(Label::createWithTTF("Aesophor", kRegularFont, kRegularFontSize)),
      _level(Label::createWithTTF("Level 1", kRegularFont, kRegularFontSize)), 
      _health(Label::createWithTTF("100 / 100", kRegularFont, kRegularFontSize)),
      _magicka(Label::createWithTTF("100 / 100", kRegularFont, kRegularFontSize)),
      _stamina(Label::createWithTTF("100 / 100", kRegularFont, kRegularFontSize)),
      _attackRange(Label::createWithTTF("5", kRegularFont, kRegularFontSize)),
      _attackSpeed(Label::createWithTTF("10", kRegularFont, kRegularFontSize)),
      _moveSpeed(Label::createWithTTF("100", kRegularFont, kRegularFontSize)),
      _jumpHeight(Label::createWithTTF("100", kRegularFont, kRegularFontSize)),
      _str(Label::createWithTTF("5", kRegularFont, kRegularFontSize)),
      _dex(Label::createWithTTF("5", kRegularFont, kRegularFontSize)),
      _int(Label::createWithTTF("5", kRegularFont, kRegularFontSize)),
      _luk(Label::createWithTTF("5", kRegularFont, kRegularFontSize)) {
  // AbstractPane::_layout is a cocos2d::ui::Layout,
  // but we know it's a TableLayout in StatsPane
  TableLayout* layout = dynamic_cast<TableLayout*>(_layout);

  layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)
  layout->addChild(_background);
  layout->setTableWidth(_background->getContentSize().width);
  layout->setRowHeight(_kRowHeight);

  // Add name and level label at the top.
  layout->addChild(_name);
  layout->align(TableLayout::Alignment::LEFT)->padLeft(_kPadLeft)->padBottom(15.0f);
  _level->setTextColor(colorscheme::kRed);
  layout->addChild(_level);
  layout->align(TableLayout::Alignment::RIGHT)->padRight(_kPadRight)->padBottom(15.0f);
  layout->row(4.0f);

  addEntry("HEALTH", _health);
  addEntry("MAGICKA", _magicka);
  addEntry("STAMINA", _stamina);
  layout->row(_kSectionHeight);

  addEntry("ATTACK RANGE", _attackRange);
  addEntry("ATTACK SPEED", _attackSpeed);
  addEntry("MOVE SPEED", _moveSpeed);
  addEntry("JUMP HEIGHT", _jumpHeight);
  layout->row(_kSectionHeight);

  addEntry("STR", _str);
  addEntry("DEX", _dex);
  addEntry("INT", _int);
  addEntry("LUK", _luk);
}


void StatsPane::update() {
  Character::Profile& profile = _pauseMenu->getPlayer()->getCharacterProfile();

  _level->setString(string_util::format("Level %d", profile.level));
  _health->setString(string_util::format("%d / %d", profile.health, profile.fullHealth));
  _magicka->setString(string_util::format("%d / %d", profile.magicka, profile.fullMagicka));
  _stamina->setString(string_util::format("%d / %d", profile.stamina, profile.fullStamina));

  _attackRange->setString(string_util::format("%.2f", profile.attackRange));
  _attackSpeed->setString(string_util::format("%.2f", profile.attackTime));
  _moveSpeed->setString(string_util::format("%.2f", profile.moveSpeed));
  _jumpHeight->setString(string_util::format("%.2f", profile.jumpHeight));

  _str->setString(string_util::format("%d", profile.strength));
  _dex->setString(string_util::format("%d", profile.dexterity));
  _int->setString(string_util::format("%d", profile.intelligence));
  _luk->setString(string_util::format("%d", profile.luck));
}

void StatsPane::handleInput() {

}

void StatsPane::addEntry(const string& title, Label* label) const {
  TableLayout* layout = dynamic_cast<TableLayout*>(_layout);

  // Create title label and disable antialiasing.
  Label* titleLabel = Label::createWithTTF(title, kRegularFont, kRegularFontSize);
  titleLabel->setTextColor(colorscheme::kGrey);
  titleLabel->getFontAtlas()->setAliasTexParameters();
  label->getFontAtlas()->setAliasTexParameters();

  // Add title (the label on the LHS of the pane).
  layout->addChild(titleLabel);
  layout->align(TableLayout::Alignment::LEFT)->padLeft(_kPadLeft);

  // Add label (on the RHS of the pane)
  layout->addChild(label);
  layout->align(TableLayout::Alignment::RIGHT)->padRight(_kPadRight)->row();
}

} // namespace vigilante
