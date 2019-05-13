#include "HeaderPane.h"

using std::array;
using std::string;
using cocos2d::Label;
using cocos2d::ui::Layout;

#include "ui/Colorscheme.h"

namespace vigilante {

const string HeaderPane::_kFont = "Font/MatchupPro.ttf";
const float HeaderPane::_kFontSize = 16.0f;
const float HeaderPane::_kOptionGap = 50.0f;
const int HeaderPane::_kOptionCount = 5;

HeaderPane::HeaderPane() : _layout(Layout::create()), _current() {
  _layout->setLayoutType(Layout::Type::RELATIVE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)

  _labels.resize(_kOptionCount);
  float nextX = 0;

  _labels[0] = Label::createWithTTF("INVENTORY", _kFont, _kFontSize);
  _labels[0]->setTextColor(colorscheme::kWhite);
  _labels[0]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[0]->getContentSize().width;

  _labels[1] = Label::createWithTTF("EQUIPMENT", _kFont, _kFontSize);
  _labels[1]->setTextColor(colorscheme::kGrey);
  _labels[1]->setPositionX(nextX + _kOptionGap * 1);
  _labels[1]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[1]->getContentSize().width;

  _labels[2] = Label::createWithTTF("SKILLS", _kFont, _kFontSize);
  _labels[2]->setTextColor(colorscheme::kGrey);
  _labels[2]->setPositionX(nextX + _kOptionGap * 2);
  _labels[2]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[2]->getContentSize().width;

  _labels[3] = Label::createWithTTF("QUESTS", _kFont, _kFontSize);
  _labels[3]->setTextColor(colorscheme::kGrey);
  _labels[3]->setPositionX(nextX + _kOptionGap * 3);
  _labels[3]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[3]->getContentSize().width;

  _labels[4] = Label::createWithTTF("OPTIONS", _kFont, _kFontSize);
  _labels[4]->setTextColor(colorscheme::kGrey);
  _labels[4]->setPositionX(nextX + _kOptionGap * 4);
  _labels[4]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[4]->getContentSize().width;

  // Add labels to the layout.
  for (auto label : _labels) {
    _layout->addChild(label);
  }
}


void HeaderPane::selectPrev() {
  _labels[_current]->setTextColor(colorscheme::kGrey);
  _current--;
  if (_current < 0) {
    _current = _kOptionCount - 1;
  }
  _labels[_current]->setTextColor(colorscheme::kWhite);
}

void HeaderPane::selectNext() {
  _labels[_current]->setTextColor(colorscheme::kGrey);
  _current = (_current + 1) % _kOptionCount;
  _labels[_current]->setTextColor(colorscheme::kWhite);
}

Layout* HeaderPane::getLayout() const {
  return _layout;
}

} // namespace vigilante
