#ifndef VIGILANTE_TABLE_LAYOUT_H_
#define VIGILANTE_TABLE_LAYOUT_H_

#include "cocos2d.h"
#include "ui/UILayout.h"

namespace vigilante {

class TableLayout : public cocos2d::ui::Layout {
 public:
  static TableLayout* create(float tableWidth=100.0f, float rowHeight=8.0f);
  virtual bool init(float tableWidth, float rowHeight);
  virtual ~TableLayout() = default;

  enum Direction {
    LEFT,
    CENTER,
    RIGHT
  };

  virtual void addChild(cocos2d::Node* child) override;
  virtual TableLayout* align(TableLayout::Direction direction); // align last added child
  virtual TableLayout* padLeft(float padding);
  virtual TableLayout* padRight(float padding);
  virtual TableLayout* padTop(float padding);
  virtual TableLayout* padBottom(float padding);
  virtual TableLayout* row(float height); // change row
  virtual TableLayout* row();

  float getTableWidth() const;
  float getRowHeight() const;
  cocos2d::Vec2 getNextChildPosition() const;
  float getNextChildPositionX() const;
  float getNextChildPositionY() const;

  void setTableWidth(float tableWidth);
  void setRowHeight(float rowHeight);
  void setNextChildPosition(const cocos2d::Vec2& pos);
  void setNextChildPositionX(float x);
  void setNextChildPositionY(float y);

 protected:
  float _tableWidth; // the width of this table layout
  float _rowHeight; // the height between any two rows

  cocos2d::Node* _lastAddedChild;
  cocos2d::Vec2 _nextChildPosition; // the x,y of the next child to be added
};

} // namespace vigilante

#endif // VIGILANTE_TABLE_LAYOUT_H_
