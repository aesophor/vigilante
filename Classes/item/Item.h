#ifndef VIGILANTE_ITEM_H_
#define VIGILANTE_ITEM_H_

#include <string>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace vigilante {

class Item {
 public:
  enum Type {
    EQUIPMENT,
    CONSUMABLE,
    MISC,
    SIZE
  };

  virtual ~Item();
  virtual void update(float delta);

  const Item::Type getItemType() const;
  const std::string& getName() const;
  const std::string& getDesc() const;
  const std::string& getIconPath() const;

  b2Body* getB2Body() const;
  cocos2d::Sprite* getSprite() const;

 protected:

  Item(const Item::Type itemType,
       const std::string& name,
       const std::string& desc,
       const std::string& iconPath,
       float x,
       float y);

  void defineBody(b2BodyType bodyType,
                  short categoryBits,
                  short maskBits,
                  float x,
                  float y);

  static const float _kIconWidth;
  static const float _kIconHeight;

  Item::Type _itemType;
  std::string _name;
  std::string _desc;
  std::string _iconPath;

  // When the item is dropped in the world, it needs a body and a sprite.
  b2Body* _b2body;
  cocos2d::Sprite* _sprite;
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_H_
