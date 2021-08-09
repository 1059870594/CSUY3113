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
    
    if(other == this) return false;
    
    if(isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    if(xdist < 0 && ydist < 0) return true;
    
    return false;
}

void Entity::CheckCollisons(Entity *objects, int objectCount, int& life){
   for (int i = 0; i < objectCount; i++){
       Entity *object = &objects[i];
       if (CheckCollison(object)){
           if(object->entityType == KEY){
               object->isActive = false;
               collidedKey = true;
               keySound = true;
           }else{
               life -= 1;
               if(life >= 1){
                   position = glm::vec3(5, -8, 0);
               }
               if(life < 1) life = 0;
           }
       }
   }
}

void Entity::CheckCollisionsY(Map *map){
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(top, &penetration_x, &penetration_y)) {
        position.y -= penetration_y;
        //velocity.y = 0;
        collidedTop = true;
    }
    
    if (map->IsSolid(bottom, &penetration_x, &penetration_y)) {
        position.y += penetration_y;
        //velocity.y = 0;
        collidedBottom = true;
    }

}

void Entity::CheckCollisionsX(Map *map){
   // Probes for tiles
   glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
   glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
   
    float penetration_x = 0;
   float penetration_y = 0;
   
    if (map->IsSolid(left, &penetration_x, &penetration_y)) {
       position.x += penetration_x;
       velocity.x = 0;
       collidedLeft = true;
    }
   if (map->IsSolid(right, &penetration_x, &penetration_y)) {
       position.x -= penetration_x;
       velocity.x = 0;
       collidedRight = true;
   }
}

void Entity::AIPatroller(){
    if(position.y  >= -9){
        movement = glm::vec3(0, -1, 0);
    }else if(position.y <= -11){
        movement = glm::vec3(0, 1, 0);
    }
}

void Entity::AI(Entity* player){
    switch(aiType){ //call different types actions
        case PATROLLER:
            AIPatroller();
            break;
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Entity* objects2, int object2Count, Map *map, int& life)
{
    if(entityType == KEY){
        if(glm::distance(position, player->position) < 2.5f && keyCount == 0){
            isActive = true;
            keyCount += 1;
        }
    }
    
    if(isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    keySound = false;
    
    if(entityType == ENEMY){ //if its an enemy, use AI to process
        AI(player);
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    position.y += movement.y * deltaTime * speed;
    if(entityType != TILE) CheckCollisionsY(map);
    
    position.x += movement.x * deltaTime * speed;
    if(entityType != TILE) CheckCollisionsX(map);
    
    if(entityType == PLAYER){//check if player runs into the moving enemy
        CheckCollisons(objects, objectCount, life);
        CheckCollisons(objects2, object2Count, life);
    }else if(entityType == ENEMY){ //check if enemey collided with a not moving playr
        if(CheckCollison(player)){ //this one works
            if(life >= 1){
                life -= 1;
                if(life > 0){
                    player->position = glm::vec3(5, -8, 0);
                }
            }
        }
    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    
    if(isActive == false) return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
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
