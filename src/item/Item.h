// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ITEM_H_
#define VIGILANTE_ITEM_H_

#include <string>

#include <cocos2d.h>
#include <Box2D/Box2D.h>
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
    explicit Profile(const std::string& jsonFileName);
    virtual ~Profile() = default;

    std::string jsonFileName;
    Item::Type itemType;
    std::string textureResDir;
    std::string name;
    std::string desc;
  };

  // Create an item by automatically deducing its concrete type
  // based on the json passed in.
  static Item* create(const std::string& jsonFileName);

  virtual ~Item() = default;
  virtual void showOnMap(float x, float y) override; // DynamicActor
  virtual void import(const std::string& jsonFileName) override; // Importable

  Item::Profile& getItemProfile();
  const std::string& getName() const;
  const std::string& getDesc() const;
  std::string getIconPath() const;

  int getAmount() const;
  void setAmount(int amount);

 protected:
  explicit Item(const std::string& jsonFileName);
  void defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y);

  static const int _kNumAnimations;
  static const int _kNumFixtures;

  Item::Profile _itemProfile;
  int _amount;
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_H_
