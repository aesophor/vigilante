#include "StatsPane.h"

using std::string;
using cocos2d::Label;
using cocos2d::Layer;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;

namespace vigilante {

const string StatsPane::_kPauseMenu = "Texture/UI/PauseMenu/";
const string StatsPane::_kStatsBg = _kPauseMenu + "stats_bg.png";
const string StatsPane::_kFont = "Font/HeartbitXX2Px.ttf";

const float StatsPane::_kFontSize = 16.0f;
const float StatsPane::_kPadLeft = 10.0f;
const float StatsPane::_kPadRight = 10.0f;
const float StatsPane::_kRowHeight = 16.0f;
const float StatsPane::_kSectionHeight = 8.0f;

StatsPane::StatsPane()
    : _background(ImageView::create(_kStatsBg)),
      _layout(TableLayout::create(_background->getContentSize().width, _kRowHeight)),
      _name(Label::createWithTTF("Aesophor", _kFont, _kFontSize)),
      _level(Label::createWithTTF("Level 1", _kFont, _kFontSize)), 
      _health(Label::createWithTTF("100 / 100", _kFont, _kFontSize)),
      _magicka(Label::createWithTTF("100 / 100", _kFont, _kFontSize)),
      _stamina(Label::createWithTTF("100 / 100", _kFont, _kFontSize)),
      _attackRange(Label::createWithTTF("5", _kFont, _kFontSize)),
      _attackSpeed(Label::createWithTTF("10", _kFont, _kFontSize)),
      _moveSpeed(Label::createWithTTF("100", _kFont, _kFontSize)),
      _jumpHeight(Label::createWithTTF("100", _kFont, _kFontSize)),
      _str(Label::createWithTTF("5", _kFont, _kFontSize)),
      _dex(Label::createWithTTF("5", _kFont, _kFontSize)),
      _int(Label::createWithTTF("5", _kFont, _kFontSize)),
      _luk(Label::createWithTTF("5", _kFont, _kFontSize)) {
  _layout->setLayoutType(Layout::Type::RELATIVE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)
  _layout->addChild(_background);

  // Add name and level label at the top.
  _layout->addChild(_name);
  _layout->align(TableLayout::Direction::LEFT)->padLeft(_kPadLeft)->padBottom(15.0f);
  _level->setTextColor({0xb0, 0x30, 0x60, 0xff});
  _layout->addChild(_level);
  _layout->align(TableLayout::Direction::RIGHT)->padRight(_kPadRight)->padBottom(15.0f);
  _layout->row(4.0f);

  addEntry("Health", _health);
  addEntry("Magicka", _magicka);
  addEntry("Stamina", _stamina);
  _layout->row(_kSectionHeight);

  addEntry("Attack Range", _attackRange);
  addEntry("Attack Speed", _attackSpeed);
  addEntry("Move Speed", _moveSpeed);
  addEntry("Jump Height", _jumpHeight);
  _layout->row(_kSectionHeight);

  addEntry("Str", _str);
  addEntry("Dex", _dex);
  addEntry("Int", _int);
  addEntry("Luk", _luk);
}


void StatsPane::addEntry(const string& title, Label* label) const {
  // Create title label and disable antialiasing.
  Label* titleLabel = Label::createWithTTF(title, _kFont, _kFontSize);
  titleLabel->setTextColor({0x93, 0x9b, 0xb0, 0xff}); // TODO: define this shit as a CONSTANT
  titleLabel->getFontAtlas()->setAliasTexParameters();
  label->getFontAtlas()->setAliasTexParameters();

  // Add title (the label on the LHS of the pane).
  _layout->addChild(titleLabel);
  _layout->align(TableLayout::Direction::LEFT)->padLeft(_kPadLeft);

  // Add label (on the RHS of the pane)
  _layout->addChild(label);
  _layout->align(TableLayout::Direction::RIGHT)->padRight(_kPadRight)->row();
}

Layout* StatsPane::getLayout() const {
  return static_cast<Layout*>(_layout);
}

} // namespace vigilante
