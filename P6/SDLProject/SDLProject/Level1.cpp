#include "Level1.h"

#define LEVEL1_WIDTH 16
#define LEVEL1_HEIGHT 18

#define LEVEL1_ENEMY_COUNT 1
#define KEY_COUNT 1

GLuint targetOneTextureID;

unsigned int level1_data[] =
{
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0,
    9, 0, 9, 0, 9, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0,
    9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0,
    9, 0, 0, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0,
    9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0,
    9, 0, 9, 0, 0, 0, 9, 0, 9, 0, 9, 0, 9, 9, 9, 0,
    9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 0, 0, 0, 9,
    9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 9, 9, 0,
    9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 0, 0, 9, 9, 9, 0,
    9, 0, 9, 0, 9, 0, 0, 0, 9, 9, 9, 9, 9, 9, 9, 0,
    9, 0, 9, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0,
    9, 0, 9, 9, 9, 0, 0, 0, 9, 9, 9, 9, 9, 9, 9, 0,
    9, 0, 0, 0, 9, 9, 9, 0, 9, 0, 0, 0, 9, 0, 9, 0,
    9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0,
    9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0,
    9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 9, 0, 0, 0, 9, 0,
    9, 0, 9, 0, 0, 0, 9, 0, 0, 0, 9, 0, 9, 0, 9, 0,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0
};


void Level1::Initialize(int& life) {
    
    state.nextScene = -1; //stays at this scene
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    GLuint mapTextureID = Util::LoadTexture("cake.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 6, 7);
    // Move over all of the player and enemy code from initialization.
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0.8, -16.1, 0);//initialize player to the left
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,0,0);
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
    
    state.player->height = 0.8;
    state.player->width = 0.6f;
    state.player->jumpPower = 6.0f;
    
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT]; //initialize enemies
    GLuint enemyTextureID = Util::LoadTexture("zombie.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = PATROLLER;
    state.enemies[0].aiState = PATROLLING;
    state.enemies[0].speed = 1;
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(6, -9, 0);
    state.enemies[0].movement = glm::vec3(0, -1, 0);
    
    state.destination = new Entity();
    targetOneTextureID = Util::LoadTexture("door2.png");
    state.destination->entityType = TILE;
    state.destination->textureID = targetOneTextureID;
    state.destination->position = glm::vec3(14, -6, 0);
    
    state.keys = new Entity[KEY_COUNT];
    GLuint CoinTextureID = Util::LoadTexture("key.png");
    for(int i = 0; i < KEY_COUNT; i++){
        state.keys[i].textureID = CoinTextureID;
        state.keys[i].entityType = KEY;
        state.keys[i].isActive = false;
    }
    state.keys[0].position = glm::vec3(13, -12, 0);
}

void Level1::Update(float deltaTime, int& life) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.keys, KEY_COUNT, state.map, life);
    
    for(int i = 0; i < LEVEL1_ENEMY_COUNT; i++){
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, NULL, 0, state.map, life);
    }
    
    state.destination->Update(deltaTime, NULL, NULL, 0, NULL, 0, NULL, life);
    
    for(int i = 0; i < KEY_COUNT; i++){
        state.keys[i].Update(deltaTime, state.player, NULL, 0, NULL, 0, state.map, life);
    }
    
    if(state.player->position.x > 13.9 && state.player->position.y <= -5.9 && state.player->collidedKey) state.nextScene = 2; //condition to change scene
}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    
    
    for(size_t i = 0; i < LEVEL1_ENEMY_COUNT; i++){//render enemies
        state.enemies[i].Render(program);
    }
    
    state.destination->Render(program);
    
    state.player->Render(program);
    
    for(size_t i = 0; i < KEY_COUNT; i++){//render enemies
        state.keys[i].Render(program);
    }
}
