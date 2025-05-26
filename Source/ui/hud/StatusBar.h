// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_HUD_STATUS_BAR_H_
#define REQUIEM_UI_HUD_STATUS_BAR_H_

#include <string>

#include <axmol.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>

namespace requiem {

class StatusBar final {
 public:
  StatusBar(const std::string& leftPaddingImgPath,
            const std::string& rightPaddingImgPath,
            const std::string& statusBarImgPath,
            const float maxLength,
            const float numSecAutoHide = _kNoAutoHide);
  void update(const float delta);
  void update(const int currentVal, const int fullVal);

  inline const ax::Vec2& getPosition() const { return _layout->getPosition(); }
  inline void setPosition(const ax::Vec2& pos) { _layout->setPosition(pos); }
  inline bool isVisible() const { return _layout->isVisible(); }
  inline void setVisible(const bool visible) { _layout->setVisible(visible); }
  inline ax::ui::Layout* getLayout() const { return _layout; }
  inline float getMaxLength() const { return _maxLength; }

 private:
  static constexpr inline float _kNoAutoHide = -1.0f;

  ax::ui::Layout* _layout;
  ax::ui::ImageView* _leftPaddingImg;
  ax::ui::ImageView* _rightPaddingImg;
  ax::ui::ImageView* _statusBarImg;
  const float _maxLength;
  const float _numSecAutoHide;
  float _autoHideTimer{};
};

}  // namespace requiem

#endif  // REQUIEM_UI_HUD_STATUS_BAR_H_
