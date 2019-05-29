#ifndef VIGILANTE_SHADE_H_
#define VIGILANTE_SHADE_H_

#include "cocos2d.h"
#include "ui/UIImageView.h"

namespace vigilante {

class Shade {
 public:
  static Shade* getInstance();
  virtual ~Shade() = default;

  static const float _kFadeOutTime;
  static const float _kFadeInTime;

  cocos2d::ui::ImageView* getImageView() const;

 private:
  static Shade* _instance;
  Shade();

  cocos2d::ui::ImageView* _imageView;
};

} // namespace vigilante

#endif // VIGILANTE_SHADE_H_
