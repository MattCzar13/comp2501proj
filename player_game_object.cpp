#include "player_game_object.h"

namespace game {

/*
	PlayerGameObject inherits from GameObject
	It overrides GameObject's update method, so that you can check for input to change the velocity of the player
*/

PlayerGameObject::PlayerGameObject(const glm::vec3 &position, GLuint texture, GLint num_elements, std::string tag, GLuint shield)
	: GameObject(position, texture, num_elements, tag) {

	health_ = 3;
	shield_timer_ = 0;
	shield_ = shield;
	weapon_type_ = 1;


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

	if (weapon_type_ == 1) {
		rof_ = 0.5;
	}
	else {
		rof_ = 0.8;
	}

	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);

	//std::printf("Health: %d, Shield timer: %f\n", health_, shield_timer_);
}

// Update function for moving the player object around
void PlayerGameObject::Render(Shader& shader) {

	if (shield_timer_ > 0) {
		// Setup the scaling matrix for the shader
		glm::mat4 shield_scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_ * 1.2, scale_ * 1.2, 1.0));
		// Set up the translation matrix for the shader
		glm::mat4 shield_translation_matrix = glm::translate(glm::mat4(1.0f), position_);
		// Setup the transformation matrix for the shader
		glm::mat4 shield_transformation_matrix = shield_translation_matrix * shield_scaling_matrix;

		glBindTexture(GL_TEXTURE_2D, shield_);
		shader.SetUniformMat4("transformation_matrix", shield_transformation_matrix);
		glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, 0);
	}

	GameObject::Render(shader);
}

void PlayerGameObject::addHealth(int h) {
	if (health_ < 3) {
		health_ += h;
	}
}

void PlayerGameObject::subtractHealth(int h) {

	if (health_ > 0 && shield_timer_ == 0) {
		health_ -= h;
	}


}

void PlayerGameObject::addShieldTimer(int t) {
	shield_timer_ += (float) t;
}

int PlayerGameObject::GetHealth() {
	return health_;
}

int PlayerGameObject::GetWeaponType() {
	return weapon_type_;
}

void PlayerGameObject::setWeaponType(int wt) {
	weapon_type_ = wt;
}

} // namespace game
