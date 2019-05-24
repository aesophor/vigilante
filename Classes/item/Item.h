#ifndef VIGILANTE_ITEM_H_
#define VIGILANTE_ITEM_H_

#include <string>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "DynamicActor.h"
#include "Importable.h"

namespace vigilante {

class Item : public DynamicActor, public Importable {
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

  virtual ~Item() = default;
  virtual void update(float delta) override;
  virtual void import(const std::string& jsonFileName) override; // Importable

  virtual void showInMap(float x, float y) override;
  virtual void removeFromMap() override;

  Item::Profile& getItemProfile();
  std::string getIconPath() const;

 protected:
  Item(const std::string& jsonFileName);
  void defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y);

  static const float _kIconWidth;
  static const float _kIconHeight;
  Item::Profile _itemProfile;
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_H_
