// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TABLE_LAYOUT_H_
#define VIGILANTE_TABLE_LAYOUT_H_

#include <axmol.h>
#include <ui/UILayout.h>

namespace vigilante {

class TableLayout : public ax::ui::Layout {
 public:
  static TableLayout* create(float tableWidth=100.0f, float rowHeight=8.0f);
  virtual bool init(float tableWidth, float rowHeight);
  virtual ~TableLayout() = default;

  enum Alignment {
    LEFT,
    CENTER,
    RIGHT
  };

  virtual void removeAllChildren() override;  // ax::ui::Layout

  virtual void addChild(ax::Node* child) override;
  virtual TableLayout* align(TableLayout::Alignment direction);  // align last added child

  virtual TableLayout* padLeft(float padding);
  virtual TableLayout* padRight(float padding);
  virtual TableLayout* padTop(float padding);
  virtual TableLayout* padBottom(float padding);

  virtual TableLayout* spaceX(float spacing);
  virtual TableLayout* spaceY(float spacing);

  virtual TableLayout* row(float height);  // change row
  virtual TableLayout* row();

  void reset();
  
  inline float getTableWidth() const { return _tableWidth; }
  inline float getRowHeight() const { return _rowHeight; }
  inline ax::Vec2 getNextChildPosition() const { return _nextChildPosition; }
  inline float getNextChildPositionX() const { return _nextChildPosition.x; }
  inline float getNextChildPositionY() const { return _nextChildPosition.y; }

  inline void setTableWidth(float tableWidth) { _tableWidth = tableWidth; }
  inline void setRowHeight(float rowHeight) { _rowHeight = rowHeight; }
  inline void setNextChildPosition(const ax::Vec2& pos) { _nextChildPosition = pos; }
  inline void setNextChildPositionX(float x) { _nextChildPosition.x = x; }
  inline void setNextChildPositionY(float y) { _nextChildPosition.y = y; }

 protected:
  float _tableWidth;  // the width of this table layout
  float _rowHeight;  // the height between any two rows

  ax::Node* _lastAddedChild;
  ax::Vec2 _nextChildPosition;  // the x,y of the next child to be added
};

}  // namespace vigilante

#endif  // VIGILANTE_TABLE_LAYOUT_H_
