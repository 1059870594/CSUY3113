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

enum EntityType {player, platforms, safePlatforms};

class Entity {
public:
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    float width = 1;
    float height = 1;
    float speed;
    
    EntityType entityType;
    EntityType lastCollison;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    bool isActive = true;
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    Entity();
    
    bool CheckCollison(Entity *other);
    void CheckCollisonsY(Entity *objects, int objectCount);
    void CheckCollisonsX(Entity *objects, int objectCount);
    void Update(float deltaTime, Entity *platforms, Entity *safePlatforms, int platformCount, int safePlatformCount);
    void Render(ShaderProgram *program);
};
