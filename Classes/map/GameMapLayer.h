#ifndef VIGILANTE_GAMEMAP_LAYER_H_
#define VIGILANTE_GAMEMAP_LAYER_H_

namespace vigilante {

enum GameMapLayer {
  GROUND,        // 0: Player can only jump while standing on the ground.
  PLATFORM,      // 1: Floating and one-way passthrough platforms. Player can also jump on this.
  WALL,          // 2: Verical walls or ceiling. Colliding with these wont set isJumping back to false.
  CLIFF_MARKER,  // 3: Alert an NPC that it is near a cliff.
  PORTAL,        // 4: Portals to switch between maps.
  LIGHT_SOURCE,  // 5: Light sources for RayHandler.
  PLAYER,        // 6: Player
  NPCS;          // 7: NPCS
};

} // namespace vigilante

#endif // VIGILANTE_GAMEMAP_LAYER_H_
