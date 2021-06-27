#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    acceleration = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollison(Entity *other){
    
    if(isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    if(xdist < 0 && ydist < 0) {
        lastCollison = other->entityType;
        return true;
    }
    return false;
}

void Entity::CheckCollisonsY(Entity *objects, int objectCount){
   for (int i = 0; i < objectCount; i++){
       Entity *object = &objects[i];
       if (CheckCollison(object)){
           float ydist = fabs(position.y - object->position.y);
           float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
           if (velocity.y > 0) {
               position.y -= penetrationY;
               velocity.y = 0;
               collidedTop = true;
           }else if (velocity.y < 0) {
               position.y += penetrationY;
               velocity.y = 0;
               collidedBottom = true;
           }
       }
   }
}

void Entity::CheckCollisonsX(Entity *objects, int objectCount){//x direction collison
   for (int i = 0; i < objectCount; i++){
       Entity *object = &objects[i];
       if (CheckCollison(object)){
           float xdist = fabs(position.x - object->position.x);
           float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
           if (velocity.x > 0) {
               position.x -= penetrationX;
               velocity.x = 0;
               collidedRight = true;
           }
           else if (velocity.x < 0) {
               position.x += penetrationX;
               velocity.x = 0;
               collidedLeft = true;
           }
       }
   }
}

void Entity::Update(float deltaTime, Entity *platforms, Entity *safePlatforms, int platformCount, int safePlatformCount)
{
    if(isActive == false) return;
    
    if(collidedLeft || collidedRight || collidedBottom) return;
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;       // Move on Y
    CheckCollisonsY(platforms, platformCount);  // Fix if needed
    CheckCollisonsY(safePlatforms, safePlatformCount);
    
    position.x += velocity.x * deltaTime;       // Move on X
    CheckCollisonsX(platforms, platformCount);  // Fix if needed
    CheckCollisonsX(safePlatforms, safePlatformCount);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::Render(ShaderProgram *program) {
    
    if(isActive == false) return;
    
    program->SetModelMatrix(modelMatrix);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
