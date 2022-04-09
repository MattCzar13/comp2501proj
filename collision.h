#ifndef COLLISION_H_
#define COLLISION_H_

#include "collision.h"
#include "game_object.h"
#include "game.h"
#include <vector>
#include "player_game_object.h"
#include <string>

namespace game {

	bool distanceCheck(GameObject* o1, GameObject* o2, float distance);
	bool RayCircleCollision(GameObject* r, GameObject* c, float delta_time);
	bool CircleCircleCollision(GameObject* c1, GameObject* c2);

	void CheckAllCollisions(std::vector<GameObject*>& gameObjects, float delta_time);

	int CheckCollisionType(std::string tag1, std::string tag2);

	void CollisionResponce(GameObject* current_game_object, GameObject* other_game_object, std::vector<GameObject*>& gameObjects, float delta_time);

} // namespace game

#endif // COLLISION_H_
