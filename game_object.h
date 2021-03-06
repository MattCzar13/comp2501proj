#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>
#include <string>

#include "shader.h"
#include <vector>

namespace game {

    /*
        GameObject is responsible for handling the rendering and updating of objects in the game world
        The update method is virtual, so you can inherit from GameObject and override the update functionality (see PlayerGameObject for reference)
    */
    class GameObject {

        public:
            // Constructor
            GameObject(const glm::vec3 &position, GLuint texture, GLint num_elements, std::string tag);

            // Update the GameObject's state. Can be overriden for children
            virtual void Update(double delta_time);

            // Renders the GameObject using a shader
            virtual void Render(Shader &shader);

            // Getters
            inline glm::vec3& GetPosition(void) { return position_; }
            inline float GetScale(void) { return scale_; }
            inline glm::vec3& GetVelocity(void) { return velocity_; }
            inline GLuint GetTex(void) { return  texture_; }
            inline float GetRadius(void) { return radius_; }
            inline void SetTex(GLuint texture) { texture_ = texture; }
            inline std::string GetTag(void) { return tag_; }
            inline double GetTime(void) { return time_; }
            inline double GetROF(void) { return rof_; }
            inline double GetAngle(void) { return angle_; }
            inline int getHealth(void) { return health_; }

            // Setters
            inline void SetPosition(const glm::vec3& position) { position_ = position; }
            inline void SetScale(float scale) { scale_ = scale; }
            inline void SetRadius(float radius) { radius_ = radius; }
            inline void SetTime(double time) { time_ = time; }
            inline void SetROF(double rof) { rof_ = rof; }
            inline void SetAngle(double angle) { angle_ = angle; }
            inline void addHealth(int h) { health_ += h; }
            inline void subtractHealth(int h) { health_ -= h; }

            inline void SetVelocity(const glm::vec3& velocity) { velocity_ = velocity; }

            // Others

            void PerformMatrixCalcs(void);

            // Object's children
            std::vector<GameObject*> child_;

        protected:
            // Object's Transform Variables
            // TODO: Add more transformation variables
            glm::vec3 position_;
            float scale_; 
            glm::vec3 velocity_;
            float radius_;

            float angle_;

            glm::vec3 pos_origin_ = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::mat4 transformation_matrix;
            glm::mat4 parent_matrix;

            // Object's details
            GLint num_elements_;
            std::string tag_;
            double time_;
            double rof_;
            int health_ = 1;

            // Object's texture reference
            GLuint texture_;

    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_
