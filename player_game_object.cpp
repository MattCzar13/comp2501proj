#include "player_game_object.h"

namespace game {

/*
	PlayerGameObject inherits from GameObject
	It overrides GameObject's update method, so that you can check for input to change the velocity of the player
*/

PlayerGameObject::PlayerGameObject(const glm::vec3 &position, GLuint texture, GLint num_elements, std::string tag)
	: GameObject(position, texture, num_elements, tag) {

	health_ = 3;
	shield_timer_ = 0;


	}

// Update function for moving the player object around
void PlayerGameObject::Update(double delta_time) {

	// Special player updates go here
	if (shield_timer_ > 0) {
		shield_timer_ -= delta_time;
	}
	else {
		shield_timer_ = 0;
	}

	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);

	//std::printf("Health: %d, Shield timer: %f\n", health_, shield_timer_);
}

void PlayerGameObject::addHealth(int h) {
	if (health_ < 3) {
		health_ += h;
	}
}

void PlayerGameObject::subtractHealth(int h) {

	if (health_ > 0) {
		health_ -= h;
	}


}

void PlayerGameObject::addShieldTimer(int t) {
	shield_timer_ += (float) t;
}

int PlayerGameObject::GetHealth() {
	return health_;
}

} // namespace game
