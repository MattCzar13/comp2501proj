#include <glm/gtc/matrix_transform.hpp>

#include "game_object.h"

namespace game {

GameObject::GameObject(const glm::vec3 &position, GLuint texture, GLint num_elements, std::string tag)
{

    // Initialize all attributes
    position_ = position;
    scale_ = 1.0;
    velocity_ = glm::vec3(0.0f, 0.0f, 0.0f); // Starts out stationary
    num_elements_ = num_elements;
    tag_ = tag;
    texture_ = texture;
    radius_ = 0.5;

}


void GameObject::Update(double delta_time) {

    // Update object position with Euler integration
    position_ += velocity_ * ((float) delta_time);
}


void GameObject::Render(Shader &shader) {

    // Bind the entity's texture
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Setup the scaling matrix for the shader
    glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_, scale_, 1.0));

    // Set up the translation matrix for the shader
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

    // Setup the transformation matrix for the shader
    glm::mat4 transformation_matrix = scaling_matrix * translation_matrix;

    // TODO: Add other types of transformations

    // Set the transformation matrix in the shader
    shader.SetUniformMat4("transformation_matrix", transformation_matrix);

    // Draw the entity
    glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, 0);
}

} // namespace game
