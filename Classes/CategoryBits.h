#ifndef VIGILANTE_CATEGORY_BITS_H_
#define VIGILANTE_CATEGORY_BITS_H_

namespace vigilante {

namespace category_bits {

const short kGround = 1;
const short kPlatform = 2;
const short kFeet = 4;
const short kWall = 8;
const short kCliffMarker = 16;
const short kPortal = 32;

const short kPlayer = 64;
const short kEnemy = 128;
const short kNpc = 256;
const short kObject = 512;
const short kItem = 1024;
const short kMeleeWeapon = 2048;
const short kProjectile = 4096;
const short kDestroyed = 8192;

} // namespace category_bits

} // namespace vigilante

#endif // VIGILANTE_CATEGORY_BITS_H_
