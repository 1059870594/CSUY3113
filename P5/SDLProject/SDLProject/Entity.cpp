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
    if(xdist < 0 && ydist < 0){
        //lastCollison = other->entityType;
        //other->lastCollison = this->entityType;
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
               collidedETop = true;
               life -= 1;
               if(life >= 1){
                   position = glm::vec3(4, -1, 0);
               }
           }else if (velocity.y < 0) {
               position.y += penetrationY;
               velocity.y = 0;
               collidedEBottom = true;
               objects[i].isActive = false;
           }
       }
       if(life < 1) life = 0;
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
               collidedERight = true;
               life -= 1;
               if(life >= 1){
                   position = glm::vec3(4, -1, 0);
               }
           }
           else if (velocity.x < 0) {
               position.x += penetrationX;
               velocity.x = 0;
               collidedELeft = true;
               life -= 1;
               if(life >= 1){
                   position = glm::vec3(4, -1, 0);
               }
           }
       }
       if(life < 1) life = 0;
   }
}

void Entity::CheckCollisionsY(Map *map){
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map *map){
   // Probes for tiles
   glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
   glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
   
    float penetration_x = 0;
   float penetration_y = 0;
   
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
       position.x += penetration_x;
       velocity.x = 0;
       collidedLeft = true;
    }
   if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
       position.x -= penetration_x;
       velocity.x = 0;
       collidedRight = true;
   }
}

/*
void Entity::AIWalker(){
    movement = glm::vec3(-1, 0, 0); //make this AI move to left
}

void Entity::AIWaitAndGo(Entity* player){ //needs an input to know the relative position of player
    switch(aiState){
        
        case IDLE: //just check distance
            if(glm::distance(position, player->position) < 3.0f){ //if less than a certain distance, the enemey walks
                aiState = WALKING;
            }
            break;
        
        case WALKING:
            if(player->position.x < position.x){
                movement = glm::vec3(-1, 0 , 0);  //walks to left depends on where player is
            }else{
                movement = glm::vec3(1, 0 , 0); //walks to right
            }
            break;
        
        case ATTACKING:
            break;
    }
}

void Entity::AI(Entity* player){
    switch(aiType){ //call different types actions
        case WALKER:
            AIWalker();
            break;
        
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
    }
}
 */

void Entity::AIWalker(Entity* player){
    if(player->position.x < position.x){
        movement = glm::vec3(-1, 0 , 0);  //walks to left depends on where player is
    }else{
        movement = glm::vec3(1, 0 , 0); //walks to right
    }
}

void Entity::AI(Entity* player, Map *map){
    switch(aiType){ //call different types actions
        case WALKER:
            AIWalker(player);
            break;
        
        case PATROLLER:
            //AIPatroller(player, map);
            break;
            
        case JUMPER:
            //AIJumper(player);
            break;
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map *map)
{
    if(isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    collidedETop = false;
    collidedEBottom = false;
    collidedELeft = false;
    collidedERight = false;
    
    if(entityType == ENEMY){ //if its an enemy, use AI to process
        //AI(player);
        AI(player, map);
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
    
    if(jump){
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(map);
    
    
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(map);
    
    /*
    if(entityType == PLAYER) CheckCollisonsY(objects, objectCount);
    if(entityType == PLAYER) CheckCollisonsX(objects, objectCount);
    */
    
    if(entityType == PLAYER){//check if player runs into the moving enemy
        CheckCollisonsY(objects, objectCount);
        CheckCollisonsX(objects, objectCount);
    }else if(entityType == ENEMY){ //check if enemey collided with a not moving playr
        /*
        if(lastCollison == PLAYER){ //this does not work, if the players runs into the moving enemy, his life minus twice
            lastCollison = TILE; //reset the collison type
            if(player->life >= 1){
                player->life -= 1;
                if(player->life > 0){
                    player->position = glm::vec3(4, -1, 0);
                }
            }
        }
         */
        if(CheckCollison(player)){ //this one works
            if(player->life >= 1){
                player->life -= 1;
                if(player->life > 0){
                    player->position = glm::vec3(4, -1, 0);
                }
            }
        };
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
