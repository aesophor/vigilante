// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CONSTANTS_H_
#define VIGILANTE_CONSTANTS_H_

namespace vigilante {

const float kFps = 60.0f;
const int kVelocityIterations = 6;
const int kPositionIterations = 2;

const float kPpm = 100;
const int kVirtualWidth = 600;
const int kVirtualHeight = 300;

const int kGravity = -10;
const int kGroundFriction = 3;
const int kWallFriction = 0;

const int kIconSize = 16;


namespace category_bits {

const short kGround = 1;
const short kPlatform = 2;
const short kFeet = 4;
const short kWall = 8;
const short kCliffMarker = 16;
const short kPortal = 32;
const short kInteractableObject = 64;

const short kPlayer = 128;
const short kEnemy = 256;
const short kNpc = 512;
const short kItem = 1024;
const short kMeleeWeapon = 2048;
const short kProjectile = 4096;
const short kDestroyed = 8192;

} // namespace category_bits


namespace graphical_layers {

const int kTmxTiledMap = 0;
const int kChest = 15;
const int kSpell = 20;

const int kNpcBody = 24;
const int kEnemyBody = 25;
const int kPlayerBody = 30;
const int kEquipment = 37;

const int kDefault = 50;

const int kFloatingDamage = 91;
const int kNotification = 93;
const int kQuestHint = 94;
const int kHud = 95;
const int kDialog = 96;
const int kShade = 97;
const int kPauseMenu = 99;

} // namespace graphical_layers

} // namespace vigilante

#endif // VIGILANTE_CONSTANTS_H_
