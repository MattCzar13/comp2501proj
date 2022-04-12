#include <stdexcept>
#include <string>
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>

#include <path_config.h>

#include "shader.h"
#include "player_game_object.h"
#include "game.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Globals that define the OpenGL window and viewport
const char *window_title_g = "HERO OF SKY";
const unsigned int window_width_g = 600;
const unsigned int window_height_g = 800;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 1.0);

// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;


Game::Game(void)
{
    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void)
{

    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
    }

    // Set window to not resizable
    // Required or else the calculation to get cursor pos to screenspace will be incorrect
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

    // Create a window and its OpenGL context
    window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's OpenGL context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
    }

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set up square geometry
    size_ = CreateSprite();

    // Initialize shader
    shader_.Init((resources_directory_g+std::string("/vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/fragment_shader.glsl")).c_str());
    shader_.Enable();

    // Set up z-buffer for rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable Alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


Game::~Game()
{

    glfwDestroyWindow(window_);
    glfwTerminate();
}


void Game::Setup(void)
{

    // Load textures
    SetAllTextures();

    // Setup the player object (position, texture, vertex count)
    // Note that, in this specific implementation, the player object should always be the first object in the game object vector 
    game_objects_.push_back(new PlayerGameObject(glm::vec3(0.0f, 0.0f, 0.0f), tex_[0], size_, "player"));
    game_objects_[0]->SetROF(0.4);

    GameObject* heart = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), tex_[14], size_, "heart");
    heart->SetScale(1);
    game_objects_.push_back(heart);

    //spawn some powerups
    //game_objects_.push_back(new GameObject(glm::vec3(-1.0f, 7.0f, 0.0f), tex_[6], size_, "health"));
    //game_objects_.push_back(new GameObject(glm::vec3(1.0f, 8.0f, 0.0f), tex_[7], size_, "shield"));

    // Setup hud
    GameObject* hud = new GameObject(glm::vec3(0.0f, -1.0f, 0.0f), tex_[15], size_, "hud_bar");
    hud->SetScale(5.0f);
    fg_objects_.push_back(hud);
    fg_objects_.push_back(new GameObject(glm::vec3(0.0f, -2.0f, 0.0f), tex_[16], size_, "hud_arrow"));
    
    // Setup background
    for (int i = 0; i < 50; i++) {
        GameObject* background = new GameObject(glm::vec3(0.0f, i * 10, 0.0f), tex_[3], size_, "ground");
        background->SetScale(10.0);
        bg_objects_.push_back(background);
    }
}


void Game::MainLoop(void)
{

    // Loop while the user did not close the window
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window_)){

        // Clear background
        glClearColor(viewport_background_color_g.r,
                     viewport_background_color_g.g,
                     viewport_background_color_g.b, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view to zoom out, centered by default at 0,0
        float cameraZoom = 0.25f;
        GameObject* player = game_objects_[0];

        // Use aspect ratio to properly scale the window
        int width, height;
        glfwGetWindowSize(window_, &width, &height);
        float aspect_ratio = ((float)width) / ((float)height);

        // Set view to zoom out, centered by default at 0,0
        glm::mat4 window_scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / aspect_ratio, 1.0f, 1.0f));
        glm::mat4 camera_zoom = glm::scale(glm::mat4(1.0f), glm::vec3(cameraZoom, cameraZoom, cameraZoom));
        camera_zoom = glm::translate(camera_zoom, -glm::vec3(0, player->GetPosition()[1]+2.0f, 0));
        glm::mat4 view_matrix = window_scale * camera_zoom;
        shader_.SetUniformMat4("view_matrix", view_matrix);

        // Calculate delta time
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Update the game
        Update(deltaTime);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
    }
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}


// Create the geometry for a sprite (a squared composed of two triangles)
// Return the number of array elements that form the square
int Game::CreateSprite(void)
{
    // The face of the square is defined by four vertices and two triangles

    // Number of attributes for vertices and faces
    // const int vertex_att = 7;  // 7 attributes per vertex: 2D (or 3D) position (2), RGB color (3), 2D texture coordinates (2)
    // const int face_att = 3; // Vertex indices (3)

    GLfloat vertex[]  = {
        // Four vertices of a square
        // Position      Color                Texture coordinates
        -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, // Top-left
         0.5f,  0.5f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // Top-right
         0.5f, -0.5f,    0.0f, 0.0f, 1.0f,    1.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f  // Bottom-left
    };

    // Two triangles referencing the vertices
    GLuint face[] = {
        0, 1, 2, // t1
        2, 3, 0  //t2
    };

    // OpenGL buffers
    GLuint vbo, ebo;

    // Create buffer for vertices
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

    // Create buffer for faces (index buffer)
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face), face, GL_STATIC_DRAW);

    // Return number of elements in array buffer (6 in this case)
    return sizeof(face) / sizeof(GLuint);
}


void Game::SetTexture(GLuint w, const char *fname)
{
    // Bind texture buffer
    glBindTexture(GL_TEXTURE_2D, w);

    // Load texture from a file to the buffer
    int width, height;
    unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Texture Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Game::SetAllTextures(void)
{
    // Load all textures that we will need
    glGenTextures(NUM_TEXTURES, tex_);
    SetTexture(tex_[0], (resources_directory_g+std::string("/textures/plane_blue.png")).c_str());
    SetTexture(tex_[1], (resources_directory_g+std::string("/textures/plane_red.png")).c_str());
    SetTexture(tex_[2], (resources_directory_g+std::string("/textures/plane_green.png")).c_str());
    SetTexture(tex_[3], (resources_directory_g+std::string("/textures/Back.bmp")).c_str());
    SetTexture(tex_[4], (resources_directory_g + std::string("/textures/bullet.png")).c_str());
    SetTexture(tex_[5], (resources_directory_g + std::string("/textures/missile.png")).c_str());
    SetTexture(tex_[6], (resources_directory_g + std::string("/textures/health.png")).c_str());
    SetTexture(tex_[7], (resources_directory_g + std::string("/textures/shield.png")).c_str());
    SetTexture(tex_[8], (resources_directory_g + std::string("/textures/enemy_red.png")).c_str());
    SetTexture(tex_[9], (resources_directory_g + std::string("/textures/enemy_spinner.png")).c_str());
    SetTexture(tex_[10], (resources_directory_g + std::string("/textures/enemy_sideshot.png")).c_str());
    SetTexture(tex_[11], (resources_directory_g + std::string("/textures/enemy_largeboss.png")).c_str());
    SetTexture(tex_[12], (resources_directory_g + std::string("/textures/heart_1.png")).c_str());
    SetTexture(tex_[13], (resources_directory_g + std::string("/textures/heart_2.png")).c_str());
    SetTexture(tex_[14], (resources_directory_g + std::string("/textures/heart_3.png")).c_str());
    SetTexture(tex_[15], (resources_directory_g + std::string("/textures/progressbar.png")).c_str());
    SetTexture(tex_[16], (resources_directory_g + std::string("/textures/progressbar_arrow.png")).c_str());
    

    glBindTexture(GL_TEXTURE_2D, tex_[0]);
}


void Game::Controls(void)
{
    // Get player game object
    GameObject *player = game_objects_[0];
    glm::vec3 curpos = player->GetPosition();
    glm::vec3 curvel = player->GetVelocity();
    glm::vec3 newPos;

    // Check for player input and make changes accordingly
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        if (glm::length(curvel) < 3) {
            player->SetVelocity(curvel + glm::vec3(0.0f,0.05f,0.0f));
        }
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        if (glm::length(curvel) > 0.5) {
            player->SetVelocity(curvel + glm::vec3(0.0f, -0.05f, 0.0f));
        }
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        newPos = curpos + glm::vec3(0.03, 0, 0);
        if (newPos[0] > 2.5) {
            newPos[0] = 2.5;
        }
        player->SetPosition(newPos);
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        newPos = curpos + glm::vec3(-0.03, 0, 0);
        if (newPos[0] < -2.5) {
            newPos[0] = -2.5;
        }
        player->SetPosition(newPos);
    }
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {

        SpawnBullet(player, 16);

    }
    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
        if (type_weapon == 1) {
            type_weapon = 2;
        }
        else {
            type_weapon = 1;
        }
        //switch wepond mode
    }
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
}

void Game::SpawnEnemies() {

    if (glfwGetTime() > enemySpawnTimer_) {
        enemySpawnTimer_ += 2;

        int randomNum = rand() % 100 + 1;

        if (randomNum > 50) {
            GameObject* enemy = new GameObject(glm::vec3(0.0f, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[8], size_, "plane");
            enemy->SetAngle(180);
            game_objects_.push_back(enemy);

            printf("[!] SPAWNED A NEW ENEMY PLANE\n");
        }
        else if(randomNum > 25){
            GameObject* enemy = new GameObject(glm::vec3(0.0f, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[9], size_, "plane2");
            game_objects_.push_back(enemy);

            printf("[!] SPAWNED A NEW ENEMY PLANE2 (SPINNER)\n");
        }
        else if(randomNum > 15) {
            GameObject* enemy = new GameObject(glm::vec3(0.0f, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[10], size_, "plane");
            enemy->SetVelocity(glm::vec3(0,0.06,0));
            enemy->SetAngle(180);
            game_objects_.push_back(enemy);

            printf("[!] SPAWNED A NEW ENEMY PLANE3 (SIDE STEPPER)\n");
        }
        else if (randomNum > 5) {
            GameObject* enemy = new GameObject(glm::vec3(0.0f, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[10], size_, "plane4");
            enemy->SetVelocity(glm::vec3(0, -0.06, 0));
            enemy->SetAngle(180);
            enemy->SetROF(0.5);
            game_objects_.push_back(enemy);

            printf("[!] SPAWNED A NEW ENEMY PLANE4 (SIDESHOT)\n");
        }
    }

}

void Game::SpawnPowerups() {

    if (glfwGetTime() > powerupSpawnTimer_) {
        powerupSpawnTimer_ += 8;

        if ((rand() % 100 + 1) > 50) {
            game_objects_.push_back(new GameObject(glm::vec3(rand() % 5 - 2.5, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[6], size_, "health"));
            printf("[!] SPAWNED A NEW HEALTH PICKUP\n");
        }
        else {
            game_objects_.push_back(new GameObject(glm::vec3(rand() % 5 - 2.5, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[7], size_, "shield"));
            printf("[!] SPAWNED A NEW SHIELD PUCKUP\n");
        }


    }

}

void Game::SpawnBullet(GameObject* plane, int speed) {

    std::string bulletTag;

    if (plane->GetTag() == "player") {
        bulletTag = "bullet_p";
    }
    else if (plane->GetTag() == "plane" || plane->GetTag() == "plane2" || plane->GetTag() == "plane3" || plane->GetTag() == "plane4") {
        bulletTag = "bullet_e";
    }

    if (plane->GetTime() < glfwGetTime()) {
        GameObject* bullet = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), tex_[4], size_, bulletTag);
        bullet->SetPosition(plane->GetPosition());
        bullet->SetAngle(plane->GetAngle());
        bullet->SetScale(0.5);
        bullet->SetVelocity((glm::vec3((speed * cos((plane->GetAngle() + 90) * ((atan(1) * 4)) / 180)), speed * sin((plane->GetAngle() + 90) * ((atan(1) * 4)) / 180), 0)));
        game_objects_.push_back(bullet);
        plane->SetTime(0);
    }
    if (plane->GetTime() == 0) {
        plane->SetTime(glfwGetTime() + plane->GetROF());
    }

}

bool Game::CheckOutOfBounds(GameObject* object) {

    int width, height;
    glfwGetWindowSize(window_, &width, &height);

    // If the object is outside the width of the screen
    if ((object->GetPosition()[0] < -(width / 2)) || (object->GetPosition()[0] > (width / 2))) {
        return true;
    }

    // If the object is outside the height of the screen (plus a little wiggle room at the top for things to spawn!!)
    if ((object->GetPosition()[1] < (game_objects_[0]->GetPosition()[1] - 3.0f)) || (object->GetPosition()[1] > (game_objects_[0]->GetPosition()[1] + 8.0f))) {
        return true;
    }

    return false;

}

void Game::Update(double delta_time)
{

    // Handle user input
    Controls();

    CheckAllCollisions(game_objects_, delta_time);

    SpawnEnemies();
    SpawnPowerups();

    // Main iteration

    // [1] FOREGROUND FG_OBJECTS_ (Heads up display)

    for (int i = 0; i < fg_objects_.size(); i++) {
        GameObject* current_game_object = fg_objects_[i];

        current_game_object->SetPosition(glm::vec3(current_game_object->GetPosition()[0], game_objects_[0]->GetPosition()[1], 0.0f));

        if (current_game_object->GetTag() == "hud_bar") {
            current_game_object->SetPosition(glm::vec3(current_game_object->GetPosition()[0], current_game_object->GetPosition()[1] + 0.5, 0.0f));
        }
        if (current_game_object->GetTag() == "hud_arrow") {
            current_game_object->SetPosition(glm::vec3((game_objects_[0]->GetPosition()[1] / 120) - 2.2, current_game_object->GetPosition()[1] - 1, 0.0f));
        }

        current_game_object->Render(shader_);
    }

    // [2] MIDDLEGROUND GAME_OBJECTS_ (Player objects, enemies, powerups, etc etc)
    for (int i = 0; i < game_objects_.size(); i++) {

        // Get the current game object
        GameObject* current_game_object = game_objects_[i];

        if (CheckOutOfBounds(current_game_object)) {
            printf("[X] Removed OOB object\n");
            game_objects_.erase(game_objects_.begin() + i);
        }

        // Update enemy
        if (current_game_object->GetTag() == "plane") {
            float distance_p_p = glm::length(current_game_object->GetPosition() - game_objects_[0]->GetPosition());
            if (distance_p_p < 9) {
                current_game_object->SetPosition(current_game_object->GetPosition() + glm::vec3(0, -0.01, 0));
            }

            SpawnBullet(current_game_object, 2);
        }
        else if (current_game_object->GetTag() == "plane2") {
            double time = current_game_object->GetTime();
            current_game_object->SetAngle(current_game_object->GetAngle() + 90);
            SpawnBullet(current_game_object, 2);
            current_game_object->SetTime(time);
            current_game_object->SetAngle(current_game_object->GetAngle() + 90);
            SpawnBullet(current_game_object, 2);
            current_game_object->SetTime(time);
            current_game_object->SetAngle(current_game_object->GetAngle() + 90);
            SpawnBullet(current_game_object, 2);
            current_game_object->SetTime(time);
            current_game_object->SetAngle(current_game_object->GetAngle() + 90);
            SpawnBullet(current_game_object, 2);
            current_game_object->SetAngle(current_game_object->GetAngle() + delta_time*40);
        }
        else if (current_game_object->GetTag() == "plane3") {
            current_game_object->SetPosition(glm::vec3(cos(glfwGetTime())*2.0, current_game_object->GetPosition()[1], 0));
            SpawnBullet(current_game_object, 2);
        }
        else if (current_game_object->GetTag() == "plane4") {
            double time = current_game_object->GetTime();
            current_game_object->SetAngle(current_game_object->GetAngle() - 90);
            SpawnBullet(current_game_object, 2);
            current_game_object->SetTime(time);
            current_game_object->SetAngle(current_game_object->GetAngle() + 180);
            SpawnBullet(current_game_object, 2);
            current_game_object->SetAngle(current_game_object->GetAngle() - 90);
        }

        if (current_game_object->GetTag() == "heart") {
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
            current_game_object->SetTex(tex_[11 + player->GetHealth()]);
            float x = player->GetPosition()[1];
            current_game_object->SetPosition(glm::vec3(2.5, 5.7 + x, 0));
        }

        // Switch weapon
        if (current_game_object->GetTag() == "bullet" & !shoot) {
            current_game_object->SetTex(tex_[3+type_weapon]);
        }
        
        // Update the current game object
        current_game_object->Update(delta_time);

        // Check for collision with other game objects
        for (int j = i + 1; j < game_objects_.size(); j++) {
            GameObject* other_game_object = game_objects_[j];

            float distance = glm::length(current_game_object->GetPosition() - other_game_object->GetPosition());
            if (distance < 0.5f ) {
               
                
                // This is where you would perform collision response between objects
            }
        }

        // Render game object
        current_game_object->Render(shader_);
    }

    // [3] BACKGROUND BG_OBJECTS_ (Background tiles, decorations behind players/enemies)
    for (int i = 0; i < bg_objects_.size(); i++) {
        GameObject* current_game_object = bg_objects_[i];

        current_game_object->Render(shader_);
    }

}
       
} // namespace game
