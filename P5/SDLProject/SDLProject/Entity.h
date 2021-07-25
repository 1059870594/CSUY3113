#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"

/*
enum EntityType {PLAYER, PLATFORM, ENEMY};
enum AIType { WALKER, WAITANDGO };
enum AIState { IDLE, WALKING, ATTACKING };
*/

enum EntityType {PLAYER, ENEMY, TILE};
enum AIType { WALKER, PATROLLER, JUMPER };
enum AIState { WALKING, PATROLLING, JUMPING };

class Entity {
public:
    EntityType entityType;
    AIType aiType; //give type and state
    AIState aiState;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float width = 1;
    float height = 1;
    bool jump = false;
    float jumpPower = 0;
    float speed;
    
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;

    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    bool isActive = true;
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    /*
    bool collidedETop = false;
    bool collidedEBottom = false;
    bool collidedELeft = false;
    bool collidedERight = false;
    EntityType lastCollison;
    */
    Entity();
    
    bool CheckCollison(Entity *other);
    void CheckCollisonsY(Entity *objects, int objectCount, int& life);
    void CheckCollisonsX(Entity *objects, int objectCount, int& life);
    void CheckCollisionsX(Map *map); //check if colliding with map
    void CheckCollisionsY(Map *map);
    
    void Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map *map, int& life);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity* player, Map *map);
    void AIWalker(Entity* player);
    void AIPatroller(Entity* player, Map *map);
    void AIJumper(Entity* player);
};
