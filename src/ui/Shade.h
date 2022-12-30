// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_SHADE_H_
#define VIGILANTE_SHADE_H_

#include <cocos2d.h>
#include <ui/UIImageView.h>

namespace vigilante {

class Shade {
 public:
  static Shade* getInstance();
  virtual ~Shade() = default;

  static inline constexpr float kFadeOutTime = .65f;
  static inline constexpr float kFadeInTime = .3f;

  cocos2d::ui::ImageView* getImageView() const { return _imageView; }

 private:
  Shade();

  cocos2d::ui::ImageView* _imageView;
};

} // namespace vigilante

#endif // VIGILANTE_SHADE_H_
