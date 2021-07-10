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
                    //std::cout << collidedLeft;
                }
            }
        }
}

void Entity::AIWalker(Entity* player){ //needs an input to know the relative position of player
    if(player->position.x < position.x){
        movement = glm::vec3(-1, 0 , 0);  //walks to left depends on where player is
    }else{
        movement = glm::vec3(1, 0 , 0); //walks to right
    }
}

void Entity::AIPatroller(Entity* player, Entity* platforms, int platformCount){ //needs an input to know the relative position of player
    movement = glm::vec3(-1, 0 , 0);
    CheckCollisonsX(platforms, platformCount);
    //std::cout << collidedLeft;
    if(collidedLeft){
        movement = glm::vec3(1, 0 , 0);  //walks to left depends on where player is
    }else if(collidedRight){
        movement = glm::vec3(-1, 0 , 0); //walks to right
    }else{
        movement = glm::vec3(-1, 0 , 0);
    }
}

void Entity::AIJumper(Entity* player){ //needs an input to know the relative position of player
    if(player->position.x < position.x){
        movement = glm::vec3(-1, 0 , 0);  //walks to left depends on where player is
    }else{
        movement = glm::vec3(1, 0 , 0); //walks to right
    }
}

void Entity::AI(Entity* player, Entity* platforms, int platformCount){
    switch(aiType){ //call different types actions
        case WALKER:
            AIWalker(player);
            break;
        
        case PATROLLER:
            AIPatroller(player, platforms, platformCount);
            break;
            
        case JUMPER:
            AIJumper(player);
            break;
    }
}

void Entity::Update(float deltaTime, Entity* player, Entity *platforms, int platformCount, Entity *enemies, int enemyCount)
{
    if(isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if(entityType == ENEMY){ //if its an enemy, use AI to process
        //AI(player);
        AI(player, platforms, platformCount);
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
    
    position.y += velocity.y * deltaTime;       // Move on Y
    CheckCollisonsY(platforms, platformCount);  // Fix if needed
    
    position.x += velocity.x * deltaTime;       // Move on X
    CheckCollisonsX(platforms, platformCount);  // Fix if needed
    
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
