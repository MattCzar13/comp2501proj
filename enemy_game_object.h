
#ifndef ENEMY_GAME_OBJECT_H_
#define ENEMY_GAME_OBJECT_H_

#include "game_object.h"
#include "collision.h"
#include <string>

namespace game {

    // Inherits from GameObject
    class EnemyGameObject : public GameObject {

    public:
        EnemyGameObject(const glm::vec3& position, GLuint texture, GLint num_elements, std::string tag, GameObject* player);

        // Update function for moving the player object around
        void Update(double delta_time) override;

        inline std::string SetState(std::string state) { state_ = state; }
        inline void SetPlayer(GameObject* player) { player_ = player; }

    private:

        std::string state_;    //the movement state of enemy
        float time_;           //time used fro parametric motion
        GameObject* player_;   //pointer to the player

    }; // class EnemyGameObject

} // namespace game

#endif // ENEMY_GAME_OBJECT_H_