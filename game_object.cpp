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


    angle_ = 0.0f;
    time_ = 0;
    rof_ = 2.5;

    if (tag == "plane2") {
        velocity_ = glm::vec3(0.0f, -1.0f, 0.0f);
        angle_ = rand() % 360 + 1;
    }
    if (tag == "bullet_p" || tag == "bullet_e") {
        radius_ = 0.2;
    }

}


void GameObject::Update(double delta_time) {

    // Update object position with Euler integration
    position_ += velocity_ * ((float) delta_time);
}

void GameObject::PerformMatrixCalcs() {
    // Setup the scaling matrix for the shader
    glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_, scale_, 1.0));

    // Set up the translation matrix for the shader
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_ + pos_origin_);

    // Setup the rotation matrix for the shader
    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0f, 0.0f, 1.0f));

    if (tag_ == "orbit") {
        // this tag will make the object orbit around the parent, rather than rotate on its anchor
        transformation_matrix = parent_matrix * (rotation_matrix * translation_matrix * scaling_matrix);
    }
    else {
        transformation_matrix = parent_matrix * (translation_matrix * rotation_matrix * scaling_matrix);
    }

}


void GameObject::Render(Shader& shader) {

    PerformMatrixCalcs();

    // Offset children's matrices
    for (GameObject* c : child_) {
        c->parent_matrix = transformation_matrix;

        if (c->GetTag() == "orbit") {
            c->SetAngle(c->GetAngle() + 10);
        }
        c->Render(shader);
    }

    // Bind the entity's texture
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Set the transformation matrix in the shader
    shader.SetUniformMat4("transformation_matrix", transformation_matrix);

    // Draw the entity
    glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, 0);
}

} // namespace game
