#include "PauseMenu.h"

using std::string;
using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::ui::ImageView;

namespace vigilante {

const string PauseMenu::_kPauseMenu = "Texture/UI/PauseMenu/";

const string PauseMenu::_kBackground = _kPauseMenu + "pause.png";

PauseMenu::PauseMenu()
    : _layer(Layer::create()),
      _background(ImageView::create(_kBackground)) {
  auto visibleSize = Director::getInstance()->getVisibleSize();
  _background->setScaleX(visibleSize.width / _background->getContentSize().width);
  _background->setScaleY(visibleSize.height / _background->getContentSize().height);
  _background->setAnchorPoint({0, 0});
  _background->setPosition({0, 0});
  _layer->addChild(_background, 0);
}


Layer* PauseMenu::getLayer() const {
  return _layer;
}

} // namespace vigilante
