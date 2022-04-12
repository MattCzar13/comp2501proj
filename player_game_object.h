#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"
#include <string>

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include <vector>

namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

        public:
            PlayerGameObject(const glm::vec3 &position, GLuint texture, GLint num_elements, std::string tag, GLuint shield);

            // Update function for moving the player object around
            void Update(double delta_time) override;
            void Render(Shader& shader) override;

            void addHealth(int h);
            void addShieldTimer(int t);
            void subtractHealth(int h);
            int GetHealth();
            int GetWeaponType();
            void setWeaponType(int wt);

        private:
           
            int health_;
            float shield_timer_;
            GLuint shield_;
            int weapon_type_;


    }; // class PlayerGameObject

} // namespace game

#endif // PLAYER_GAME_OBJECT_H_
