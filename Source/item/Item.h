// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ITEM_H_
#define VIGILANTE_ITEM_H_

#include <memory>
#include <string>

#include <axmol.h>
#include <box2d/box2d.h>

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

  struct Profile final {
    explicit Profile(const std::string& jsonFilePath);

    std::string jsonFilePath;
    Item::Type itemType;
    std::string textureResDir;
    std::string name;
    std::string desc;
    int price;
  };

  // Create an item by automatically deducing its concrete type
  // based on the json passed in.
  static std::unique_ptr<Item> create(const std::string& jsonFilePath);

  virtual ~Item() override = default;

  virtual bool showOnMap(float x, float y) override;  // DynamicActor
  virtual void update(float delta) override;  // DynamicActor
  virtual void import(const std::string& jsonFilePath) override;  // Importable

  inline Item::Profile& getItemProfile() { return _itemProfile; }
  inline const Item::Profile& getItemProfile() const { return _itemProfile; }
  inline const std::string& getName() const { return _itemProfile.name; }
  inline const std::string& getDesc() const { return _itemProfile.desc; }
  std::string getIconPath() const;
  bool isGold() const;

  inline int getAmount() const { return _amount; }
  inline void setAmount(int amount) { _amount = amount; }

 protected:
  explicit Item(const std::string& jsonFilePath);

  void defineBody(b2BodyType bodyType,
                  float x,
                  float y,
                  short categoryBits,
                  short maskBits);

  void handleVerticalFloatingMovement(float delta);

  Item::Profile _itemProfile;
  int _amount{1};
  float _accumulatedDelta{};  // used for in-game-map up & down floating.
};

}  // namespace vigilante

#endif  // VIGILANTE_ITEM_H_
