<div align="center">
<h3>VIGILANTE</h3>
<img src="/.meta/combat.gif">
</div>

## Overview
In the following context I'll give you an overview on each subsystem of this game.

## Development
* [cocos2d-x-3.17.1](https://cocos2d-x.org/filedown/cocos2d-x-3.17.1)
* [box2d](https://box2d.org/) - 2D Physics Engine
* [rapidjson](http://rapidjson.org/) - serialization
* g++ (Gentoo 8.3.0-r1 p1.1) 8.3.0

# Characters
![](https://raw.githubusercontent.com/aesophor/Vigilante/readme/.meta/character.jpg)

Vigilante's **[Character](https://github.com/aesophor/Vigilante/blob/master/Classes/character/Character.h)** has the following concrete subclasses:
* **[Player](https://github.com/aesophor/Vigilante/blob/master/Classes/character/Player.h)**
* **[Enemy](https://github.com/aesophor/Vigilante/blob/master/Classes/character/Enemy.h)**
* **[Npc](https://github.com/aesophor/Vigilante/blob/master/Classes/character/Npc.h)**

It is probably one of the most complicated class in this game. Let's have a look at some Character's important methods:
```
// Trimmed
virtual void moveLeft();
virtual void moveRight();
virtual void jump();
virtual void jumpDown();
virtual void crouch();
virtual void getUp();

virtual void sheathWeapon();
virtual void unsheathWeapon();
virtual void attack();
virtual void activateSkill(Skill* skill);
virtual void knockBack(Character* target, float forceX, float forceY) const;
virtual void inflictDamage(Character* target, int damage);
virtual void receiveDamage(Character* source, int damage);
virtual void lockOn(Character* target);

virtual void addItem(Item* item, int amount=1);
virtual void removeItem(Item* item, int amount=1);
virtual void useItem(Consumable* consumable);
virtual void equip(Equipment* equipment);
virtual void unequip(Equipment::Type equipmentType);
virtual void pickupItem(Item* item);
virtual void discardItem(Item* item, int amount);
virtual void interact(Interactable* target);
```

# Items and Equipment
![](https://raw.githubusercontent.com/aesophor/Vigilante/readme/.meta/item.png)

Vigilante's **[Item](https://github.com/aesophor/Vigilante/blob/master/Classes/item/Item.h)** has the following concrete subclasses:
* **[Equipment](https://github.com/aesophor/Vigilante/blob/master/Classes/item/Equipment.h)** (e.g., leather armor, leather boots, short sword)
* **[Consumable](https://github.com/aesophor/Vigilante/blob/master/Classes/item/Consumable.h)** (e.g., potions)
* **[Misc Item](https://github.com/aesophor/Vigilante/blob/master/Classes/item/MiscItem.h)** (e.g., gold coins)

Items can be acquired by opening a chest or looting from enemies. Also, the items equipped **will be reflected on player's appearance**! 

![](https://raw.githubusercontent.com/aesophor/Vigilante/readme/.meta/pausemenu_item.png)

Items and equipment can be accessed via **[PauseMenu](https://github.com/aesophor/Vigilante/blob/readme/Classes/ui/pause_menu/PauseMenu.h)**. Depending on the item's type, there are different actions to perform:
* Equipment - **equip**, discard, cancel
* Consumable - **use**, discard, cancel
* Misc Item - discard, cancel

Some UI classes worth looking at:
[1] [ListView](https://github.com/aesophor/Vigilante/blob/readme/Classes/ui/ListView.h) - ItemListView, SkillListView, DialogueListView...
[2] [TabView](https://github.com/aesophor/Vigilante/blob/readme/Classes/ui/TabView.h)
[3] [PauseMenuDialog](https://github.com/aesophor/Vigilante/blob/readme/Classes/ui/pause_menu/PauseMenuDialog.h)

# Combat and Item Dropping
![](https://raw.githubusercontent.com/aesophor/Vigilante/readme/.meta/combat.gif)

Vigilante's **Combat System** mainly consists of the following:
* **melee** regular attack (e.g., sword, axe)
* **distant** regular attack  (e.g., bow, staff) -- not implemented
* **special skills** (e.g., forward slash, ice spike)

When an enemy dies, it will give **exp point** to the character who kills it (could be the Player or an ally NPC), and then drop some items (defined in enemy's json file). See [`Enemy::receiveDamage`]([https://github.com/aesophor/Vigilante/blob/readme/Classes/character/Enemy.cc#L87](https://github.com/aesophor/Vigilante/blob/readme/Classes/character/Enemy.cc#L87)).

# Inventory and Equipment
![](https://raw.githubusercontent.com/aesophor/Vigilante/readme/.meta/inventoryequipment.gif)

Vigilante's **Inventory and Equipment System** is just like those which can be found in regular RPG.

# Copyright
Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
