// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_SHADE_H_
#define VIGILANTE_UI_SHADE_H_

#include <axmol.h>
#include <ui/UIImageView.h>

namespace vigilante {

class Shade final {
 public:
  Shade();

  inline ax::ui::ImageView* getImageView() const { return _imageView; }

  static inline constexpr float kFadeOutTime = .65f;
  static inline constexpr float kFadeInTime = .3f;

 private:
  ax::ui::ImageView* _imageView;
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_SHADE_H_
