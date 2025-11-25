#include <box2d/box2d.h>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <iostream>

#include "collisionBits.h"

class CollisionLoader {
public:
// recibe pixelsToMeters, pero por el momento la relacion metros <-> pixeles es 1 a 1 (box2d puede andar mal con esta escala)
// worldHeight se recibe para poder invertir el eje Y y que las colisiones estén donde tienen que estar (porque se generan de la imagen, que tecnicamente para box2d está al revés)
static std::vector<b2Body*> LoadCollisions(const std::string& yamlPath, std::unique_ptr<b2World>& world, float pixelsToMeters = 1.0f, float worldHeight = 0.0f, uint8_t layer = 0, bool isSensor = false);

};