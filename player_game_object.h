#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"
#include <string>

namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

        public:
            PlayerGameObject(const glm::vec3 &position, GLuint texture, GLint num_elements, std::string tag);

            // Update function for moving the player object around
            void Update(double delta_time) override;

            void addHealth(int h);
            void addShieldTimer(int t);

        private:
           
            int health_;
            float shield_timer_;


    }; // class PlayerGameObject

} // namespace game

#endif // PLAYER_GAME_OBJECT_H_
