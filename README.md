<div align="center">
<h3>VIGILANTE</h3>
<img src="/.meta/combat.gif">
</div>

## Overview
In the following context I'll give you an overview on each subsystem of this game.

## Development
Framework and Libraries:
* [cocos2d-x-3.17.1](https://cocos2d-x.org/filedown/cocos2d-x-3.17.1)
* [box2d](https://box2d.org/) - 2D Physics Engine
* [rapidjson](http://rapidjson.org/) - Object {,de}serialization

Compiler:
* g++ (Gentoo 8.3.0-r1 p1.1) 8.3.0

## Character
**[Character](https://github.com/aesophor/Vigilante/blob/master/Classes/character/Character.h)** is an abstract base class which you cannot directly instatiate. It has the following concrete derived classes:
* **Player**
* **Enemy**
* **Npc**

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

## Combat
![](https://raw.githubusercontent.com/aesophor/Vigilante/readme/.meta/combat.gif)


# Copyright
Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
