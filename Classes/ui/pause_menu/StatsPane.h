#ifndef VIGILANTE_STATS_PANE_H_
#define VIGILANTE_STATS_PANE_H_

#include <string>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"

#include "ui/TableLayout.h"
#include "ui/pause_menu/AbstractPane.h"

namespace vigilante {

class StatsPane : public AbstractPane {
 public:
  StatsPane();
  virtual ~StatsPane() = default;
  virtual void update() override;
  virtual void handleInput() override;

 private:
  void addEntry(const std::string& title, cocos2d::Label* label) const;

  static const float _kPadLeft;
  static const float _kPadRight;
  static const float _kRowHeight;
  static const float _kSectionHeight;

  cocos2d::ui::ImageView* _background;

  cocos2d::Label* _name;
  cocos2d::Label* _level;
  cocos2d::Label* _health;
  cocos2d::Label* _magicka;
  cocos2d::Label* _stamina;

  cocos2d::Label* _attackRange;
  cocos2d::Label* _attackSpeed;
  cocos2d::Label* _moveSpeed;
  cocos2d::Label* _jumpHeight;

  cocos2d::Label* _str;
  cocos2d::Label* _dex;
  cocos2d::Label* _int;
  cocos2d::Label* _luk;
};

} // namespace vigilante

#endif // VIGILANTE_STATS_PANE_H_
