#ifndef VIGILANTE_ITEM_H_
#define VIGILANTE_ITEM_H_

#include <string>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace vigilante {

enum ItemType {
  EQUIPMENT,
  CONSUMABLE,
  MISC
};

class Item {
 public:
  virtual ~Item() = default;
  virtual void update(float delta);

  const ItemType getItemType() const;
  const std::string& getName() const;
  const std::string& getDesc() const;

  b2Body* getB2Body() const;
  cocos2d::Sprite* getSprite() const;

 protected:
  Item(const ItemType itemType,
       const std::string& name,
       const std::string& desc,
       const std::string& imgPath,
       float x,
       float y);

  void defineBody(b2BodyType bodyType,
                  short categoryBits,
                  short maskBits,
                  float x,
                  float y);

  static const float _kIconWidth;
  static const float _kIconHeight;

  ItemType _itemType;
  std::string _name;
  std::string _desc;

  // When the item is dropped in the world, it needs a body and a sprite.
  b2Body* _b2body;
  cocos2d::Sprite* _sprite;
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_H_
