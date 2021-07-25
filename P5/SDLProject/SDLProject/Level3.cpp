#include "Level3.h"

#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 1

unsigned int level3_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 0, 3,
    3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};
void Level3::Initialize(int& life) {
    
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
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
    
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT]; //initialize enemies
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = JUMPER;
    state.enemies[0].aiState = JUMPING;
    state.enemies[0].speed = 1;
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(9, -3, 0);
    state.enemies[0].width = 0.8f;
    
    state.destination = new Entity();
    GLuint targetOneTextureID = Util::LoadTexture("targetOne.png");
    state.destination->entityType = TILE;
    state.destination->textureID = targetOneTextureID;
    state.destination->position = glm::vec3(12, -3, 0);
}

void Level3::Update(float deltaTime, int& life) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map, life);
    state.enemies[0].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map, life);
    state.destination->Update(deltaTime, NULL, NULL, 0, NULL, life);
}

void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    state.enemies[0].Render(program);
    state.destination->Render(program);
}


