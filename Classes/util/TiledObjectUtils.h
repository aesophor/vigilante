#ifndef VIGILANTE_TILED_OBJECT_UTILS_H_
#define VIGILANTE_TILED_OBJECT_UTILS_H_

#include "Box2D/Box2D.h"

#include "map/GameMap.h"

namespace vigilante {

void parseLayers(b2World* world, const vigilante::GameMap& gameMap);
//void createRectangles(b2World* world, const vigilante::GameMap& gameMap)

} // namespace vigilante

#endif // VIGILANTE_TILED_OBJECTS_UTILS_H_
