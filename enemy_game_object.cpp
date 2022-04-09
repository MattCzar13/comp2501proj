
#include "enemy_game_object.h"

namespace game {


	EnemyGameObject::EnemyGameObject(const glm::vec3& position, GLuint texture, GLint num_elements, std::string tag, GameObject* player) : GameObject(position, texture, num_elements, tag) {
		state_ = "patrolling";
		time_ = 0;
		player_ = player;
	}


	void EnemyGameObject::Update(double delta_time) {

		//checking the state of the enemy
		if (state_ == "patrolling") {

		}
		else {

		}
	}

} // namespace game