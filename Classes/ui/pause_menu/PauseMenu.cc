#include "PauseMenu.h"

using std::string;
using cocos2d::Layer;
using cocos2d::ui::ImageView;

namespace vigilante {

const string PauseMenu::_kPauseMenu = "Texture/UI/PauseMenu/";

const string PauseMenu::_kBackground = _kPauseMenu + "pause.png";

PauseMenu::PauseMenu()
    : _layer(Layer::create()),
      _background(ImageView::create(_kBackground)) {
}


Layer* PauseMenu::getLayer() const {
  return _layer;
}

} // namespace vigilante
