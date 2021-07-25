#include "Level1.h"

#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

GLuint targetOneTextureID;

/*
unsigned int level1_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};
 */

unsigned int level1_data[] =
{
    11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    11, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
    11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    11, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};


void Level1::Initialize(int& life) {
    
    state.nextScene = -1; //stays at this scene
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    GLuint mapTextureID = Util::LoadTexture("cakeSet.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 7, 7);
    // Move over all of the player and enemy code from initialization.
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, 0, 0);//initialize player to the left
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0);
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f;
    state.player->width = 0.6f;
    state.player->jumpPower = 6.0f;
    
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT]; //initialize enemies
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = WALKING;
    state.enemies[0].speed = 1;
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(2, -5, 0);
    
    state.destination = new Entity();
    targetOneTextureID = Util::LoadTexture("targetOne.png");
    state.destination->entityType = TILE;
    state.destination->textureID = targetOneTextureID;
    state.destination->position = glm::vec3(12, -4, 0);
}

void Level1::Update(float deltaTime, int& life) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map, life);
    
    for(int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map, life);
    }
    
    state.destination->Update(deltaTime, NULL, NULL, 0, NULL, life);
    
    if(state.player->position.x > 12 && state.player->position.y < -4) state.nextScene = 2; //condition to change scene
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    
    for(size_t i = 0; i < LEVEL1_ENEMY_COUNT; i++){//render enemies
        state.enemies[i].Render(program);
    }
    
    state.destination->Render(program);
    
}
