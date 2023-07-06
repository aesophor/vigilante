// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_STATUS_BAR_H_
#define VIGILANTE_STATUS_BAR_H_

#include <string>

#include <axmol.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>

namespace vigilante {

class StatusBar {
 public:
  StatusBar(const std::string& leftPaddingImgPath,
            const std::string& rightPaddingImgPath,
            const std::string& statusBarImgPath,
            float maxLength);
  virtual ~StatusBar() = default;
  void update(int currentVal, int fullVal);

  ax::ui::Layout* getLayout() const {
    return _layout;
  }

 private:
  ax::ui::Layout* _layout;
  ax::ui::ImageView* _leftPaddingImg;
  ax::ui::ImageView* _rightPaddingImg;
  ax::ui::ImageView* _statusBarImg;
  const float _maxLength;
};

}  // namespace vigilante

#endif  // VIGILANTE_STATUS_BAR_H_
