// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_CONSTANTS_H_
#define REQUIEM_CONSTANTS_H_

#include <2d/Camera.h>

namespace requiem {

inline constexpr float kFps = 60.0f;
inline constexpr int kVelocityIterations = 6;
inline constexpr int kPositionIterations = 2;

inline constexpr float kPpm = 100;
inline constexpr int kVirtualWidth = 600;
inline constexpr int kVirtualHeight = 300;

inline constexpr float kDensity = 80.0f;
inline constexpr float kGravity = -9.8f;
inline constexpr float kBodyVolumeToMoveForceFactor = 0.0010416666666666667;
inline constexpr float kGroundFriction = 2.5f;
inline constexpr int kWallFriction = 0.01f;

inline constexpr int kIconSize = 16;

inline constexpr float kHintBubbleFxSpriteOffsetY = 45.0f;

namespace category_bits {

inline constexpr short kGround = 1;
inline constexpr short kPlatform = 2;
inline constexpr short kFeet = 4;
inline constexpr short kWall = 8;
inline constexpr short kPivotMarker = 16;
inline constexpr short kCliffMarker = 32;
inline constexpr short kPortal = 64;
inline constexpr short kInteractable = 128;

inline constexpr short kPlayer = 256;
inline constexpr short kEnemy = 512;
inline constexpr short kNpc = 1024;
inline constexpr short kItem = 2048;
inline constexpr short kMeleeWeapon = 4096;
inline constexpr short kProjectile = 8192;
inline constexpr short kDestroyed = 16384;

}  // namespace category_bits

namespace camera {

inline constexpr float kNearPlane = 1.0f;
inline constexpr float kFarPlane = 1000.0f;

inline constexpr ax::CameraFlag kParallaxCameraFlag = ax::CameraFlag::USER1;
inline constexpr ax::CameraFlag kGameCameraFlag = ax::CameraFlag::USER2;
inline constexpr ax::CameraFlag kHudCameraFlag = ax::CameraFlag::USER3;

inline constexpr uint16_t kParallaxCameraMask = static_cast<uint16_t>(kParallaxCameraFlag);
inline constexpr uint16_t kGameCameraMask = static_cast<uint16_t>(kGameCameraFlag);
inline constexpr uint16_t kHudCameraMask = static_cast<uint16_t>(kHudCameraFlag);

}  // namespace camera

namespace z_order {

inline constexpr int kTmxTiledMap = 10;
inline constexpr int kChest = 15;
inline constexpr int kStaticObjects = 18;
inline constexpr int kSpell = 20;
inline constexpr int kNpcBody = 24;
inline constexpr int kEnemyBody = 25;
inline constexpr int kItem = 28;
inline constexpr int kPlayerBody = 30;
inline constexpr int kEquipment = 37;
inline constexpr int kFx = 45;

inline constexpr int kDefault = 60;

inline constexpr int kFloatingDamage = 80;
inline constexpr int kNotification = 82;
inline constexpr int kQuestHint = 84;
inline constexpr int kHud = 86;
inline constexpr int kTimeLocationInfo = 87;
inline constexpr int kWindowBottom = 88;
inline constexpr int kWindowTop = 92;
inline constexpr int kDialogue = 94;
inline constexpr int kPauseMenu = 96;
inline constexpr int kControlHints = 98;
inline constexpr int kConsole = 99;
inline constexpr int kShade = 100;

}  // namespace z_order

}  // namespace requiem

#endif  // REQUIEM_CONSTANTS_H_
