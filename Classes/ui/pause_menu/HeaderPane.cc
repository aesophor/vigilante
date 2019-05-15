#include "HeaderPane.h"

#include "GameAssetManager.h"
#include "ui/Colorscheme.h"

using std::array;
using std::string;
using cocos2d::Label;
using cocos2d::ui::Layout;
using vigilante::asset_manager::kTitleFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

const float HeaderPane::_kOptionGap = 25.0f;
const int HeaderPane::_kOptionCount = 5;

HeaderPane::HeaderPane() : AbstractPane() , _currentIndex() {
  _layout->setLayoutType(Layout::Type::RELATIVE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)

  _labels.resize(_kOptionCount);
  float nextX = 0;

  _labels[0] = Label::createWithTTF("INVENTORY", kTitleFont, kRegularFontSize);
  _labels[0]->setTextColor(colorscheme::kWhite);
  _labels[0]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[0]->getContentSize().width;

  _labels[1] = Label::createWithTTF("EQUIPMENT", kTitleFont, kRegularFontSize);
  _labels[1]->setTextColor(colorscheme::kGrey);
  _labels[1]->setPositionX(nextX + _kOptionGap * 1);
  _labels[1]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[1]->getContentSize().width;

  _labels[2] = Label::createWithTTF("SKILLS", kTitleFont, kRegularFontSize);
  _labels[2]->setTextColor(colorscheme::kGrey);
  _labels[2]->setPositionX(nextX + _kOptionGap * 2);
  _labels[2]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[2]->getContentSize().width;

  _labels[3] = Label::createWithTTF("QUESTS", kTitleFont, kRegularFontSize);
  _labels[3]->setTextColor(colorscheme::kGrey);
  _labels[3]->setPositionX(nextX + _kOptionGap * 3);
  _labels[3]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[3]->getContentSize().width;

  _labels[4] = Label::createWithTTF("OPTIONS", kTitleFont, kRegularFontSize);
  _labels[4]->setTextColor(colorscheme::kGrey);
  _labels[4]->setPositionX(nextX + _kOptionGap * 4);
  _labels[4]->getFontAtlas()->setAliasTexParameters();
  nextX += _labels[4]->getContentSize().width;

  // Add labels to the layout.
  for (auto label : _labels) {
    _layout->addChild(label);
  }
}


void HeaderPane::handleInput() {

}

void HeaderPane::selectPrev() {
  _labels[_currentIndex]->setTextColor(colorscheme::kGrey);
  _currentIndex--;
  if (_currentIndex < 0) {
    _currentIndex = _kOptionCount - 1;
  }
  _labels[_currentIndex]->setTextColor(colorscheme::kWhite);
}

void HeaderPane::selectNext() {
  _labels[_currentIndex]->setTextColor(colorscheme::kGrey);
  _currentIndex = (_currentIndex + 1) % _kOptionCount;
  _labels[_currentIndex]->setTextColor(colorscheme::kWhite);
}

int HeaderPane::getCurrentIndex() const {
  return _currentIndex;
}

} // namespace vigilante
