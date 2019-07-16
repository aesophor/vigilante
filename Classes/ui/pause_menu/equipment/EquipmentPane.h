// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_EQUIPMENT_PANE_H_
#define VIGILANTE_EQUIPMENT_PANE_H_

#include <vector>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"

#include "ui/pause_menu/AbstractPane.h"
#include "ui/TableLayout.h"
#include "input/InputManager.h"
#include "character/Character.h"
#include "item/Equipment.h"

namespace vigilante {

class EquipmentPane : public AbstractPane {
 public:
  EquipmentPane(PauseMenu* pauseMenu);
  virtual ~EquipmentPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

  void selectUp();
  void selectDown();
  void confirm();

  Equipment* getSelectedEquipment() const;
  Equipment::Type getSelectedEquipmentType() const;

 private:
  class EquipmentItem {
   public:
    friend class EquipmentPane;
    EquipmentItem(EquipmentPane* parent, const std::string& title, float x, float y);
    virtual ~EquipmentItem() = default;

    Equipment* getEquipment() const;
    void setEquipment(Equipment* equipment);

    void setSelected(bool selected) const;
    cocos2d::ui::Layout* getLayout() const;

   private:
    static const int _kEquipmentIconSize;

    EquipmentPane* _parent;
    TableLayout* _layout;
    cocos2d::ui::ImageView* _background;
    cocos2d::ui::ImageView* _icon;
    cocos2d::Label* _equipmentTypeLabel;
    cocos2d::Label* _equipmentNameLabel;
    Equipment* _equipment;
  };

  InputManager* _inputMgr;
  std::vector<std::unique_ptr<EquipmentItem>> _equipmentItems;
  int _current;
};

} // namespace vigilante

#endif // VIGILANTE_EQUIPMENT_PANE_H_
