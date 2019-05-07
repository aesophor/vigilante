#ifndef VIGILANTE_CATEGORY_BITS_H_
#define VIGILANTE_CATEGORY_BITS_H_

namespace vigilante {

const short kGround = 1;
const short kPlatform = 2;
const short kFeet = 4;
const short kWall = 8;
const short kCliffMarker = 16;
const short kPortal = 32;

const short kPlayer = 64;
const short kEnemy = 128;
const short kObject = 256;
const short kItem = 512;
const short kMeleeWeapon = 1024;
const short kDestroyed = 2048;

const short kLight = 4096;

} // namespace vigilante

#endif // VIGILANTE_CATEGORY_BITS_H_
