// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_SHADE_H_
#define REQUIEM_UI_SHADE_H_

#include <axmol.h>
#include <ui/UIImageView.h>

namespace requiem {

class Shade final {
 public:
  Shade();

  inline ax::ui::ImageView* getImageView() const { return _imageView; }

  static inline constexpr float kFadeInSec = .3f;
  static inline constexpr float kFadeOutSec = .65f;

 private:
  ax::ui::ImageView* _imageView;
};

}  // namespace requiem

#endif  // REQUIEM_UI_SHADE_H_
