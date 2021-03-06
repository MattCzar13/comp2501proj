#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "shader.h"
#include "game_object.h"
#include "collision.h"

namespace game {

    // A class for holding the main game objects
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();

            // Call Init() before calling any other method
            // Initialize graphics libraries and main window
            void Init(void); 

            // Set up the game (scene, game objects, etc.)
            void Setup(void);

            // Run the game (keep the game active)
            void MainLoop(void); 

        private:
            // Main window: pointer to the GLFW window structure
            GLFWwindow *window_;

            // Shader for rendering the scene
            Shader shader_;

            // Size of geometry to be rendered
            int size_;

            //if shooting
            bool shoot = false;
            int type_weapon = 1;

            // game state
            std::string state;

            // References to textures
#define NUM_TEXTURES 30
            GLuint tex_[NUM_TEXTURES];

            // List of game objects
            std::vector<GameObject*> game_objects_;

            // List of background objects
            std::vector<GameObject*> bg_objects_;

            // List of foreground objects
            std::vector<GameObject*> fg_objects_;

            // Callback for when the window is resized
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            // Create a square for drawing textures
            int CreateSprite(void);

            // Set a specific texture
            void SetTexture(GLuint w, const char *fname);

            // Load all textures
            void SetAllTextures();

            // Handle user input
            void Controls(void);

            // Update the game based on user input and simulation
            void Update(double delta_time);

            // Function that handles enemy spawning
            void SpawnEnemies(void);
            void SpawnPowerups(void);
            double enemySpawnTimer_ = 1;
            double powerupSpawnTimer_ = 1;

            // Function that checks if an object is outside of the viewport
            bool CheckOutOfBounds(GameObject* object);

            // Function that handles bullet spawning, automatically assumes whether the object is a player or enemy
            void SpawnBullet(GameObject* plane, int speed);

    }; // class Game

} // namespace game

#endif // GAME_H_
