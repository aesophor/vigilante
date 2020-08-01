// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
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
const short kPivotMarker = 16;
const short kCliffMarker = 32;
const short kPortal = 64;
const short kInteractable = 128;

const short kPlayer = 256;
const short kEnemy = 512;
const short kNpc = 1024;
const short kItem = 2048;
const short kMeleeWeapon = 4096;
const short kProjectile = 8192;
const short kDestroyed = 16384;

}  // namespace category_bits


namespace graphical_layers {

const int kTmxTiledMap = 0;
const int kChest = 15;
const int kSpell = 20;

const int kNpcBody = 24;
const int kEnemyBody = 25;
const int kPlayerBody = 30;
const int kEquipment = 37;

const int kDefault = 50;

const int kFloatingDamage = 80;
const int kNotification = 82;
const int kQuestHint = 84;
const int kHud = 86;
const int kWindowBottom = 88;
const int kWindowTop = 92;
const int kDialogue = 94;
const int kShade = 96;
const int kPauseMenu = 98;
const int kConsole = 100;

}  // namespace graphical_layers

}  // namespace vigilante

#endif  // VIGILANTE_CONSTANTS_H_
