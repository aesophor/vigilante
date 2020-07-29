// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_STATUS_BAR_H_
#define VIGILANTE_STATUS_BAR_H_

#include <string>

#include <cocos2d.h>
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

  cocos2d::ui::Layout* getLayout() const;

 private:
  cocos2d::ui::Layout* _layout;
  cocos2d::ui::ImageView* _leftPaddingImg;
  cocos2d::ui::ImageView* _rightPaddingImg;
  cocos2d::ui::ImageView* _statusBarImg;
  const float _maxLength;
};

}  // namespace vigilante

#endif  // VIGILANTE_STATUS_BAR_H_
