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

    state = "game";

    // Setup the player object (position, texture, vertex count)
    // Note that, in this specific implementation, the player object should always be the first object in the game object vector 
    game_objects_.push_back(new PlayerGameObject(glm::vec3(0.0f, 0.0f, 0.0f), tex_[0], size_, "player", tex_[15]));
    game_objects_[0]->SetROF(0.4);

    GameObject* orbit = new GameObject(glm::vec3(0.5f, 0.0f, 0.0f), tex_[21], size_, "orbit");
    orbit->SetScale(0.0f);
    game_objects_[0]->child_.push_back(orbit);
    orbit = new GameObject(glm::vec3(-0.5f, 0.0f, 0.0f), tex_[21], size_, "orbit");
    orbit->SetScale(0.0f);
    game_objects_[0]->child_.push_back(orbit);
    orbit = new GameObject(glm::vec3(0.0f, 0.5f, 0.0f), tex_[21], size_, "orbit");
    orbit->SetScale(0.0f);
    game_objects_[0]->child_.push_back(orbit);
    orbit = new GameObject(glm::vec3(0.0f, -0.5f, 0.0f), tex_[21], size_, "orbit");
    orbit->SetScale(0.0f);
    game_objects_[0]->child_.push_back(orbit);

    GameObject* heart = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), tex_[14], size_, "heart");
    heart->SetScale(1);
    game_objects_.push_back(heart);

    //spawn some powerups
    //game_objects_.push_back(new GameObject(glm::vec3(-1.0f, 7.0f, 0.0f), tex_[6], size_, "health"));
    //game_objects_.push_back(new GameObject(glm::vec3(1.0f, 8.0f, 0.0f), tex_[7], size_, "shield"));

    // Setup hud
    GameObject* hud = new GameObject(glm::vec3(0.0001f, 0.0f, 0.0f), tex_[22], size_, "title");
    hud->SetScale(5.0f);
    fg_objects_.push_back(hud);
    hud = new GameObject(glm::vec3(0.0f, -1.0f, 0.0f), tex_[16], size_, "hud_bar");
    hud->SetScale(5.0f);
    fg_objects_.push_back(hud);
    hud = new GameObject(glm::vec3(0.0f, -2.0f, 0.0f), tex_[17], size_, "hud_arrow");
    hud->SetScale(0.5f);
    fg_objects_.push_back(hud);
    hud = new GameObject(glm::vec3(0.0f, 1.0f, 0.0f), tex_[25], size_, "title_win");
    hud->SetScale(0.0f);
    fg_objects_.push_back(hud);
    hud = new GameObject(glm::vec3(0.0f, 1.0f, 0.0f), tex_[26], size_, "title_lose");
    hud->SetScale(0.0f);
    fg_objects_.push_back(hud);
    hud = new GameObject(glm::vec3(-2.0f, 6.0f, 0.0f), tex_[27], size_, "indicator1");
    hud->SetScale(0.5f);
    fg_objects_.push_back(hud);
    hud = new GameObject(glm::vec3(-2.0f, 6.0f, 0.0f), tex_[28], size_, "indicator2");
    hud->SetScale(0.5f);
    fg_objects_.push_back(hud);
    
    int texnumber = 3;

    // Setup background
    for (int i = 0; i < 100; i++) {

        if (i < 16) {
            texnumber = 18;
        }else if (i < 30) {
            texnumber = 19;
        }else{
            texnumber = 20;
        }

        GameObject* background = new GameObject(glm::vec3(0.0f, i * 10, 0.0f), tex_[texnumber], size_, "ground");
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

        if (state == "win" || state == "lose") {
            camera_zoom = glm::translate(camera_zoom, -glm::vec3(0, fg_objects_[0]->GetPosition()[1] + 0.8, 0));
        }
        else {
            camera_zoom = glm::translate(camera_zoom, -glm::vec3(0, player->GetPosition()[1] + 2.0f, 0));
        }

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
    SetTexture(tex_[3], (resources_directory_g+std::string("/textures/bg1.png")).c_str());
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
    SetTexture(tex_[15], (resources_directory_g + std::string("/textures/playerShield.png")).c_str());
    SetTexture(tex_[16], (resources_directory_g + std::string("/textures/progressbar.png")).c_str());
    SetTexture(tex_[17], (resources_directory_g + std::string("/textures/progressbar_arrow.png")).c_str());
    SetTexture(tex_[18], (resources_directory_g + std::string("/textures/bg1.png")).c_str());
    SetTexture(tex_[19], (resources_directory_g + std::string("/textures/bg2.png")).c_str());
    SetTexture(tex_[20], (resources_directory_g + std::string("/textures/bg3.png")).c_str());
    SetTexture(tex_[21], (resources_directory_g + std::string("/textures/shield_cosmetic.png")).c_str());
    SetTexture(tex_[22], (resources_directory_g + std::string("/textures/title.png")).c_str());
    SetTexture(tex_[23], (resources_directory_g + std::string("/textures/bullet_green.png")).c_str());
    SetTexture(tex_[24], (resources_directory_g + std::string("/textures/bullet_orange.png")).c_str());
    SetTexture(tex_[25], (resources_directory_g + std::string("/textures/title_win.png")).c_str());
    SetTexture(tex_[26], (resources_directory_g + std::string("/textures/title_lose.png")).c_str());
    SetTexture(tex_[27], (resources_directory_g + std::string("/textures/indicator_1.png")).c_str());
    SetTexture(tex_[28], (resources_directory_g + std::string("/textures/indicator_2.png")).c_str());
    

    glBindTexture(GL_TEXTURE_2D, tex_[0]);
}


void Game::Controls(void)
{
    // Get player game object
    PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
    glm::vec3 curpos = player->GetPosition();
    glm::vec3 curvel = player->GetVelocity();
    glm::vec3 newPos;

    // debug tools
    if (glfwGetKey(window_, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
        player->SetPosition(glm::vec3(0.0f, player->GetPosition()[1] - 1, 0.0f));
        printf("[?] Moving player backwards...\n");
    }
    if (glfwGetKey(window_, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
        player->SetPosition(glm::vec3(0.0f, player->GetPosition()[1] + 1, 0.0f));
        printf("[?] Moving player forwards...\n");
    }
    if (glfwGetKey(window_, GLFW_KEY_BACKSLASH) == GLFW_PRESS) {
        player->addShieldTimer(60);
        printf("[?] Giving player 60 seconds of invincibility...\n");
    }

    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        printf("[?] Closing game...\n");
        glfwSetWindowShouldClose(window_, true);
    }

    // if the player won or lost, skip the rest of these inputs
    // basically, ignore player input
    if (state == "win" || state == "lose") {
        return;
    }

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
        player->SetVelocity(glm::vec3(2.0f, player->GetVelocity()[1], 0.0f));

        if ((player->GetPosition()[0] + 2.0f) > 4.5) {
            player->SetVelocity(curvel);
        }
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        player->SetVelocity(glm::vec3(-2.0f, player->GetVelocity()[1], 0.0f));

        if ((player->GetPosition()[0] - 2.0f) < -4.5) {
            player->SetVelocity(curvel);
        }
    }
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {

        if (player->GetWeaponType() == 1) {
            SpawnBullet(player, 16);
        }
        else if (player->GetWeaponType() == 2) {
            double time = player->GetTime();
            player->SetAngle(player->GetAngle() + 30);
            SpawnBullet(player, 8);
            player->SetTime(time);
            player->SetAngle(player->GetAngle() -60);
            SpawnBullet(player, 8);
            player->SetTime(time);
            player->SetAngle(player->GetAngle() + 30);
            SpawnBullet(player, 8);
        }

    }
    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
        player->setWeaponType(2);
        //switch wepond mode
    }
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {

        player->setWeaponType(1);
        //switch wepond mode
    }
}

void Game::SpawnEnemies() {

    if (glfwGetTime() > enemySpawnTimer_) {
        enemySpawnTimer_ += 2;

        // if the player is fighting the boss or has won or lost the game, no other enemies should spawn
        if (state == "boss" || state == "win" || state == "lose") {
            return;
        }

        // if the player enters the "boss area", prep the boss fight and change the game state to "boss"
        if (game_objects_[0]->GetPosition()[1] > 440 && state == "game") {
            printf("[!] SPAWNED THE BOSS\n");
            state = "boss";
            GameObject* enemy = new GameObject(glm::vec3(0.0f, game_objects_[0]->GetPosition()[1] + 5.0f, 0.0f), tex_[11], size_, "planeboss");
            enemy->SetAngle(180);
            enemy->SetROF(0.5);
            enemy->SetScale(2.0f);
            enemy->addHealth(10);
            game_objects_.push_back(enemy);
        }

        int randomNum = rand() % 100 + 1;
        float x = rand() % 5 - 1.5;
        //x = 2.5;

        if (randomNum > 50) {
            GameObject* enemy = new GameObject(glm::vec3(x, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[8], size_, "plane");
            enemy->SetAngle(180);
            game_objects_.push_back(enemy);

            printf("[!] SPAWNED A NEW ENEMY PLANE\n");
        }
        else if(randomNum > 25){
            GameObject* enemy = new GameObject(glm::vec3(x, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[9], size_, "plane2");
            game_objects_.push_back(enemy);

            printf("[!] SPAWNED A NEW ENEMY PLANE2 (SPINNER)\n");
        }
        else if(randomNum > 15) {
            GameObject* enemy = new GameObject(glm::vec3(0.0f, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[10], size_, "plane3");
            enemy->SetVelocity(glm::vec3(0,0.06,0));
            enemy->SetAngle(180);
            game_objects_.push_back(enemy);

            printf("[!] SPAWNED A NEW ENEMY PLANE3 (SIDE STEPPER)\n");
        }
        else if (randomNum > 5) {
            GameObject* enemy = new GameObject(glm::vec3(x, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[10], size_, "plane4");
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

        // if the player has won or lost the game, no more powerups should spawn
        if (state == "win" || state == "lose") {
            return;
        }

        if ((rand() % 100 + 1) > 50) {
            game_objects_.push_back(new GameObject(glm::vec3(rand() % 5 - 1.5, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[6], size_, "health"));
            printf("[!] SPAWNED A NEW HEALTH PICKUP\n");
        }
        else {
            game_objects_.push_back(new GameObject(glm::vec3(rand() % 5 - 1.5, game_objects_[0]->GetPosition()[1] + 8.0f, 0.0f), tex_[7], size_, "shield"));
            printf("[!] SPAWNED A NEW SHIELD PUCKUP\n");
        }


    }

}

void Game::SpawnBullet(GameObject* plane, int speed) {

    std::string bulletTag;
    int textureNumber;

    //checkign what type of bullet to add
    if (plane->GetTag() == "player") {
        bulletTag = "bullet_p";
        PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
        if (player->GetWeaponType() == 1) {
            textureNumber = 4;
        }
        else {
            textureNumber = 23;
        }
        
    }
    else if (plane->GetTag() == "plane" || plane->GetTag() == "plane2" || plane->GetTag() == "plane3" || plane->GetTag() == "plane4" || plane->GetTag() == "planeboss") {
        bulletTag = "bullet_e";
        textureNumber = 24;
    }

    if (plane->GetTime() < glfwGetTime()) {
        GameObject* bullet = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), tex_[textureNumber], size_, bulletTag);
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

    // Enemies + powerups will not spawn if the player hasn't "started" the game by moving forward a bit

    if (game_objects_[0]->GetPosition()[1] > 10) {
        SpawnEnemies();
        SpawnPowerups();
    }
    else if (game_objects_[0]->GetPosition()[1] <= 10) {
        enemySpawnTimer_ = glfwGetTime();
        powerupSpawnTimer_ = glfwGetTime();
    }

    // Main iteration

    // [1] FOREGROUND FG_OBJECTS_ (Heads up display)

    for (int i = 0; i < fg_objects_.size(); i++) {
        GameObject* current_game_object = fg_objects_[i];

        // if the player is in gameplay, the hud will follow them
        // if the player won or lost, the hud stops (and hides) for the camera to focus on
        if (state != "win" && state != "lose") {
            current_game_object->SetPosition(glm::vec3(current_game_object->GetPosition()[0], game_objects_[0]->GetPosition()[1], 0.0f));
        }

        if (current_game_object->GetTag() == "title") {
            // if the game has "started", make the title slide off screen and then die
            if (game_objects_[0]->GetPosition()[1] > 5) {
                current_game_object->SetPosition(glm::vec3(current_game_object->GetPosition()[0] * 1.1, current_game_object->GetPosition()[1] + 3, 0.0f));
                if (CheckOutOfBounds(current_game_object)) {
                    printf("[X] Removed title object\n");
                    fg_objects_.erase(fg_objects_.begin() + i);
                }
            }
            else {
                current_game_object->SetPosition(glm::vec3(current_game_object->GetPosition()[0], current_game_object->GetPosition()[1] + 3, 0.0f));
            }

        }
        if (current_game_object->GetTag() == "hud_bar") {
            if (state == "win") {
                continue;
            }
            if (state != "lose") {
                current_game_object->SetPosition(glm::vec3(current_game_object->GetPosition()[0], current_game_object->GetPosition()[1] + 0.5, 0.0f));
            }

        }
        if (current_game_object->GetTag() == "hud_arrow") {
            if (state == "win") {
                continue;
            }
            if (state != "lose") {
                current_game_object->SetPosition(glm::vec3((game_objects_[0]->GetPosition()[1] / 100) - 2.2, current_game_object->GetPosition()[1] - 1.2, 0.0f));
                if (current_game_object->GetPosition()[0] > 2.2) {
                    current_game_object->SetPosition(glm::vec3(2.2, current_game_object->GetPosition()[1], 0.0f));
                }
            }
        }
        if (current_game_object->GetTag() == "title_win" && state == "win") {
            current_game_object->SetScale(5.0f);
        }
        if (current_game_object->GetTag() == "title_lose" && state == "lose") {
            current_game_object->SetScale(5.0f);
        }
        if (current_game_object->GetTag() == "indicator1") {
            current_game_object->SetPosition(glm::vec3(-2.6f, current_game_object->GetPosition()[1] + 5.5f, 0.0f));

            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
            if (player->GetWeaponType() == 1) {
                current_game_object->SetScale(0.5f);
            }
            else if (player->GetWeaponType() == 2) {
                current_game_object->SetScale(0.0f);
            }
        }
        if (current_game_object->GetTag() == "indicator2") {
            current_game_object->SetPosition(glm::vec3(-2.6f, current_game_object->GetPosition()[1] + 5.5f, 0.0f));

            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
            if (player->GetWeaponType() == 1) {
                current_game_object->SetScale(0.0f);
            }
            else if (player->GetWeaponType() == 2) {
                current_game_object->SetScale(0.5f);
            }
        }

        current_game_object->Render(shader_);
    }

    // [2] MIDDLEGROUND GAME_OBJECTS_ (Player objects, enemies, powerups, etc etc)
    for (int i = 0; i < game_objects_.size(); i++) {

        // Get the current game object
        GameObject* current_game_object = game_objects_[i];

        if (CheckOutOfBounds(current_game_object)) {
            printf("[X] Removed OOB object\n");
            if (current_game_object->GetTag() == "planeboss") {
                state = "win";
            }
            game_objects_.erase(game_objects_.begin() + i);
        }

        if (current_game_object->GetTag() == "player") {
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);

            if (state == "win") {
                player->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
            }
            if (player->GetHealth() <= 0) {
                state = "lose";
                player->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                player->SetScale(0.0f);
            }
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
            //rotateing the enemy 90 degrees each time a bullet is spawned
            //this happens 4 times so it will bring the player back to where they started
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
            //std::printf("plane 3 x:%f\n", cos(glfwGetTime()) * 2.0);
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
        else if (current_game_object->GetTag() == "planeboss") {
            current_game_object->SetPosition(glm::vec3(cos(glfwGetTime()) * 2.0, current_game_object->GetPosition()[1], 0));
            current_game_object->SetVelocity(glm::vec3(0.0f, game_objects_[0]->GetVelocity()[1], 0.0f));
            SpawnBullet(current_game_object, 2);
        }

        if (current_game_object->GetTag() == "heart") {
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
            current_game_object->SetTex(tex_[11 + player->GetHealth()]);
            float x = player->GetPosition()[1];
            current_game_object->SetPosition(glm::vec3(2.5, 5.7 + x, 0));
        }

        // Switch weapon
        //if (current_game_object->GetTag() == "bullet" & !shoot) {
            //current_game_object->SetTex(tex_[3+type_weapon]);
        //}
        
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
