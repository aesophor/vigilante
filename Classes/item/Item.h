#ifndef VIGILANTE_ITEM_H_
#define VIGILANTE_ITEM_H_

#include <string>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "Importable.h"

namespace vigilante {

class Item : public Importable {
 public:
  enum Type {
    EQUIPMENT,
    CONSUMABLE,
    MISC,
    SIZE
  };

  struct Profile {
    Profile(const std::string& jsonFileName);
    virtual ~Profile() = default;

    Item::Type itemType;
    std::string textureResPath;
    std::string name;
    std::string desc;
  };

  virtual ~Item();
  virtual void update(float delta);
  virtual void import(const std::string& jsonFileName) override;

  Item::Profile& getItemProfile();
  std::string getIconPath() const;

  b2Body* getB2Body() const;
  cocos2d::Sprite* getSprite() const;

 protected:
  Item(const std::string& jsonFileName, float x, float y);

  void defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y);

  static const float _kIconWidth;
  static const float _kIconHeight;

  Item::Profile _itemProfile;

  // When the item is dropped in the world, it needs a body and a sprite.
  bool _isBodyExisted;
  b2Body* _b2body;
  cocos2d::Sprite* _sprite;
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_H_
