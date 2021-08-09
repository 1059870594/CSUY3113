#include "Level2.h"

#define LEVEL2_WIDTH 16
#define LEVEL2_HEIGHT 18

#define LEVEL2_ENEMY_COUNT 1
#define LEVEL2_KEY_COUNT 1

unsigned int level2_data[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0,
    3, 0, 3, 0, 3, 3, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0,
    3, 0, 3, 0, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
    3, 0, 3, 0, 3, 3, 3, 0, 3, 0, 0, 0, 3, 0, 3, 0,
    3, 0, 3, 0, 3, 3, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
    3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 3, 3, 3, 3, 0,
    3, 0, 3, 0, 3, 3, 3, 0, 3, 0, 3, 0, 0, 0, 0, 3,
    3, 0, 3, 3, 3, 3, 3, 0, 3, 0, 3, 0, 3, 3, 3, 0,
    3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 3, 3, 3, 0,
    3, 0, 3, 0, 3, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0,
    3, 0, 3, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0,
    3, 0, 3, 3, 3, 0, 0, 0, 3, 0, 3, 3, 3, 3, 3, 0,
    3, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 3, 0, 3, 0,
    3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
    3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
    3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 3, 0,
    3, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 3, 0, 3, 0,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0
};

void Level2::Initialize(int& life) {
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("choco.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 6, 7);
    // Move over all of the player and enemy code from initialization.
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0.8f, -0.9f, 0);//initialize player to the left
    state.player->movement = glm::vec3(0);
    state.player->speed = 3.0f;
    state.player->textureID = Util::LoadTexture("newCharacter.png");
    
    state.player->animRight = new int[4] {8, 9, 10, 11};
    state.player->animLeft = new int[4] {4, 5, 6, 7};
    state.player->animUp = new int[4] {12, 13, 14, 15};
    state.player->animDown = new int[4] {0, 1, 2, 3};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f;
    state.player->width = 0.6f;
    state.player->jumpPower = 6.0f;
    
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT]; //initialize enemies
    GLuint enemyTextureID = Util::LoadTexture("zombie.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = PATROLLER;
    state.enemies[0].aiState = PATROLLING;
    state.enemies[0].speed = 1;
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(6, -9, 0);
    state.enemies[0].movement = glm::vec3(0, -1, 0);
    
    state.destination = new Entity();
    GLuint targetOneTextureID = Util::LoadTexture("door2.png");
    state.destination->entityType = TILE;
    state.destination->textureID = targetOneTextureID;
    state.destination->position = glm::vec3(14, -6, 0);
    
    state.keys = new Entity[LEVEL2_KEY_COUNT];
    GLuint KeyTextureID = Util::LoadTexture("key.png");
    for(int i = 0; i < LEVEL2_KEY_COUNT; i++){
        state.keys[i].textureID = KeyTextureID;
        state.keys[i].entityType = KEY;
        state.keys[i].isActive = false;
    }
    state.keys[0].position = glm::vec3(13, -16, 0);
}

void Level2::Update(float deltaTime, int& life) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.keys, LEVEL2_KEY_COUNT, state.map, life);
    
    state.enemies[0].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, NULL, 0, state.map, life);
    
    state.keys[0].Update(deltaTime, state.player, NULL, 0, NULL, 0, state.map, life);
    
    state.destination->Update(deltaTime, NULL, NULL, 0, NULL, NULL, 0, life);
    
    if(state.player->position.x > 13.9 && state.player->position.y <= -5.9 && state.player->collidedKey) state.nextScene = 3;
}

void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.destination->Render(program);
    state.enemies[0].Render(program);
    state.keys[0].Render(program);
    state.player->Render(program);
}

