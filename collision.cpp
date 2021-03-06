
#include "collision.h"

namespace game {

    bool distanceCheck(GameObject* o1, GameObject* o2, float distance)
    {

        if (glm::length(o1->GetPosition() - o2->GetPosition()) < distance) {
            return true;
        }

        return false;
    }

    bool RayCircleCollision(GameObject* r, GameObject* c, float delta_time) {

        float px = r->GetPosition()[0];
        float py = r->GetPosition()[1];

        float vx = r->GetVelocity()[0];
        float vy = r->GetVelocity()[1];

        float cx = c->GetPosition()[0];
        float cy = c->GetPosition()[1];

        float radius = c->GetRadius();

        //std::printf("input:\n");
        //std::printf("Px: %f, Py: %f\n", px, py);
        //std::printf("Vx: %f, Vy: %f\n", vx, vy);
        //std::printf("Cx: %f, Cy: %f\n", cx, cy);
        //std::printf("Radius: %f\n", radius);

        px = px - cx;
        py = py - cy;

        //std::printf("\najusted Px and Py to acount for C:\n");
        //std::printf("Px: %f, Py: %f\n", px, py);
        //std::printf("Vx: %f, Vy: %f\n", vx, vy);
        //std::printf("Cx: %f, Cy: %f\n", cx, cy);

        float slope = vy / vx;
        //std::printf("\nslope: %f\n", slope);

        float b = px * slope - py;
        //std::printf("\nb: %f\n", b);

        float discriminant = (slope * b * 2) * (slope * b * 2) - 4 * (slope * slope + 1) * (b * b - radius * radius);
        //std::printf("\ndiscriminant: %f\n", discriminant);

        if (discriminant < 0) {
            //std::printf("No intersection");
        }
        else if (discriminant == 0) {
            //std::printf("1 intersection");
        }
        else {
            //std::printf("2 intersections");

            float intersection_1 = (-(slope * b * 2) + std::sqrt(discriminant)) / (2 * (slope * slope + 1));
            float intersection_2 = (-(slope * b * 2) - std::sqrt(discriminant)) / (2 * (slope * slope + 1));

            //std::printf("\nintersection 1: %f\n", intersection_1);
            //std::printf("\nintersection 2: %f\n", intersection_2);

            intersection_1 = (intersection_1 + py) / slope;
            intersection_2 = (intersection_2 + py) / slope;

            //std::printf("\nintersection 1: %f\n", intersection_1);
            //std::printf("\nintersection 2: %f\n", intersection_2);

            if (vy > 0) {
                if ((intersection_1 < delta_time * std::sqrt(vx * vx + vy * vy)) && (intersection_1 > 0)) {
                    //std::printf("colitions detected\n");
                    return true;
                }
                if ((intersection_2 < delta_time * std::sqrt(vx * vx + vy * vy)) && (intersection_2 > 0)) {
                    //std::printf("colitions detected\n");
                    return true;
                }
            }
            else {
                if ((intersection_1 > -delta_time * std::sqrt(vx * vx + vy * vy)) && (intersection_1 < 0)) {
                    //std::printf("colitions detected\n");
                    return true;
                }
                if ((intersection_2 > -delta_time * std::sqrt(vx * vx + vy * vy)) && (intersection_2 < 0)) {
                    //std::printf("colitions detected\n");
                    return true;
                }
            }

        }

        return false;
    }

    bool CircleCircleCollision(GameObject* c1, GameObject* c2) {

        if (glm::length(c1->GetPosition() - c2->GetPosition()) < c1->GetRadius() + c2->GetRadius()) {
            return true;
        }

        return false;
    }

    void CheckAllCollisions(std::vector<GameObject*>& gameObjects, float delta_time) {

        //printf("check all collisions");

        //loop over each game object
        for (int i = 0; i < gameObjects.size(); i++) {

            // Get the current game object
            GameObject* current_game_object = gameObjects[i];

            // Check for collision with other game objects
            for (int j = i + 1; j < gameObjects.size(); j++) {

                // Get the other game object
                GameObject* other_game_object = gameObjects[j];

                //check type of collision
                int collision_type = CheckCollisionType(current_game_object->GetTag(), other_game_object->GetTag());

                //check for collition if there is a type
                bool collision = false;
                if (collision_type == 1) {
                    collision = CircleCircleCollision(current_game_object, other_game_object);
                }
                else if (collision_type == 2) {
                    collision = RayCircleCollision(current_game_object, other_game_object, delta_time);
                    //printf("checked for ray circle colision\n");
                }
                else if (collision_type == 3) {
                    collision = RayCircleCollision(other_game_object, current_game_object, delta_time);
                }

                if (collision) {

                    CollisionResponce(current_game_object, other_game_object, gameObjects, delta_time);

                }


            }
        }

        return;

    }

    int CheckCollisionType(std::string tag1, std::string tag2) {

        //checking for any combination of objects that might collide
        if (tag1 == "player" && tag2 == "plane") {
            return 1;
        }
        else if (tag1 == "player" && tag2 == "plane2") {
            return 1;
        }
        else if (tag1 == "plane" && tag2 == "bullet_p") {
            return 1;
        }
        else if (tag1 == "plane2" && tag2 == "bullet_p") {
            return 1;
        }
        else if (tag1 == "plane3" && tag2 == "bullet_p") {
            return 1;
        }
        else if (tag1 == "plane4" && tag2 == "bullet_p") {
            return 1;
        }
        else if (tag1 == "planeboss" && tag2 == "bullet_p") {
            return 1;
        }
        else if (tag1 == "bullet_p" && tag2 == "plane") {
            return 1;
        }
        else if (tag1 == "bullet_p" && tag2 == "plane2") {
            return 1;
        }
        else if (tag1 == "bullet_p" && tag2 == "plane3") {
            return 1;
        }
        else if (tag1 == "bullet_p" && tag2 == "plane4") {
            return 1;
        }
        else if (tag1 == "bullet_p" && tag2 == "planeboss") {
            return 1;
        }
        else if (tag1 == "player" && tag2 == "health") {
            return 1;
        }
        else if (tag1 == "player" && tag2 == "shield") {
            return 1;
        }
        else if (tag1 == "player" && tag2 == "bullet_e") {
            return 1;
        }

      

    }

    void CollisionResponce(GameObject* current_game_object, GameObject* other_game_object, std::vector<GameObject*>& gameObjects, float delta_time) {

        std::string tag1 = current_game_object->GetTag();
        std::string tag2 = other_game_object->GetTag();

        //checking for combinations of objects and performing appropriate responce 
        //if the object needs to be despawned it is moved far of the screan so it is removed in the update function when
        //it is checked if it is out of bounds
        if (tag1 == "player" && (tag2 == "plane" || tag2 == "plane2" || tag2 == "plane3" || tag2 == "plane4")) {
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);
            player->subtractHealth(1);
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
        }
        else if ((tag2 == "plane" || tag2 == "plane2" || tag2 == "plane3" || tag2 == "plane4") && tag2 == "player") {
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(other_game_object);
            player->subtractHealth(1);
            current_game_object->SetPosition(glm::vec3(100, 0, 0));
        }
        else if (tag1 == "bullet_p" && tag2 == "plane") {
            //printf("collision between bullet and plane\n");
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
            current_game_object->SetPosition(glm::vec3(-100, 0, 0));

        }
        else if (tag1 == "bullet_p" && tag2 == "plane2") {
            //printf("collision between bullet and plane\n");
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
            current_game_object->SetPosition(glm::vec3(-100, 0, 0));

        }
        else if (tag1 == "bullet_p" && tag2 == "plane3") {
            //printf("collision between bullet and plane\n");
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
            current_game_object->SetPosition(glm::vec3(-100, 0, 0));

        }
        else if (tag1 == "bullet_p" && tag2 == "plane4") {
            //printf("collision between bullet and plane\n");
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
            current_game_object->SetPosition(glm::vec3(-100, 0, 0));

        }
        else if (tag1 == "bullet_p" && tag2 == "planeboss") {
            //printf("collision between bullet and plane\n");

            current_game_object->SetPosition(glm::vec3(-100, 0, 0));
            other_game_object->subtractHealth(1);

            if (other_game_object->getHealth() <= 0) {
                other_game_object->SetPosition(glm::vec3(100, 0, 0));
            }

        }
        else if (tag1 == "plane" && tag2 == "bullet_p") {
            //printf("collision between plane and bullet\n");
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
            current_game_object->SetPosition(glm::vec3(-100, 0, 0));
        }
        else if (tag1 == "plane2" && tag2 == "bullet_p") {
            //printf("collision between plane and bullet\n");
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
            current_game_object->SetPosition(glm::vec3(-100, 0, 0));
        }
        else if (tag1 == "plane3" && tag2 == "bullet_p") {
            //printf("collision between plane and bullet\n");
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
            current_game_object->SetPosition(glm::vec3(-100, 0, 0));
        }
        else if (tag1 == "plane4" && tag2 == "bullet_p") {
            //printf("collision between plane and bullet\n");
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
            current_game_object->SetPosition(glm::vec3(-100, 0, 0));
        }
        else if (tag1 == "planeboss" && tag2 == "bullet_p") {
            //printf("collision between plane and bullet\n");

            other_game_object->SetPosition(glm::vec3(-100, 0, 0));
            current_game_object->subtractHealth(1);

            if (current_game_object->getHealth() <= 0) {
                current_game_object->SetPosition(glm::vec3(100, 0, 0));
            }
        }
        else if (tag1 == "player" && tag2 == "health") {
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);
            player->addHealth(1);
            other_game_object->SetPosition(glm::vec3(100,0,0));
        }
        else if (tag1 == "player" && tag2 == "shield") {
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);
            player->addShieldTimer(5);
            
            for (int i = 0; i < player->child_.size(); i++) {
                player->child_[i]->SetScale(0.2f);
            }

            other_game_object->SetPosition(glm::vec3(100, 0, 0));
        }
        else if (tag1 == "player" && tag2 == "bullet_e") {
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);
            player->subtractHealth(1);
            other_game_object->SetPosition(glm::vec3(100, 0, 0));
        }

    }

}