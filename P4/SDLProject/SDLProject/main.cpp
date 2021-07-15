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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>

#include "Entity.h"
#define PLATFORM_COUNT 38
#define ENEMY_COUNT 3

GLuint fontTextureID;

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemies;
    Entity *destination;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position){
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
                offset + (-0.5f * size), 0.5f * size,
                offset + (-0.5f * size), -0.5f * size,
                offset + (0.5f * size), 0.5f * size,
                offset + (0.5f * size), -0.5f * size,
                offset + (0.5f * size), 0.5f * size,
                offset + (-0.5f * size), -0.5f * size,
        });
            texCoords.insert(texCoords.end(), {
                u, v,
                u, v + height,
                u + width, v,
                u + width, v + height,
                u + width, v,
                u, v + height,
            });
        } // end of for loop
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    fontTextureID = LoadTexture("font1.png");
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4, -1, 0);//initialize player to the left
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("newCharacter.png"); //credit to Revolvee
    
    state.player->animRight = new int[4] {8, 9, 10, 11};
    state.player->animLeft = new int[4] {4, 5, 6, 7};
    //state.player->animUp = new int[4] {2, 6, 10, 14};
    //state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f;
    state.player->width = 0.6f;
    state.player->jumpPower = 6.0f;
    
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("newTile.png"); //credit to Kenney
    
    for(int i = 0; i < PLATFORM_COUNT; i++){//initiliaize all the platforms using for loop
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
    }
    
    for(int i = 0; i < 11; i++){//initiliaize all the platforms using for loop
        state.platforms[i].position = glm::vec3(-5 + i, -3.25f, 0);
    }
    
    for(int i = 11; i < 21; i++){//initiliaize all the platforms using for loop
        state.platforms[i].position = glm::vec3(-3 + i - 11, -2.25f, 0);
    }
    
    for(int i = 21; i < 27; i++){//initiliaize all the platforms using for loop
        state.platforms[i].position = glm::vec3(i - 21, -1.25f, 0);
    }
    
    state.platforms[22].position = glm::vec3(1, -1.75f, 0);
    state.platforms[23].position = glm::vec3(2, -1.75f, 0);
    state.platforms[24].position = glm::vec3(3, -1.75f, 0);
    state.platforms[25].position = glm::vec3(4, -1.75f, 0);
    
    state.platforms[27].position = glm::vec3(0.5, -0.75f, 0);
    state.platforms[28].position = glm::vec3(4.5, -0.75f, 0);
    
    for(int i = 29; i < 34; i++){//initiliaize all the platforms using for loop
        state.platforms[i].position = glm::vec3(-5 + i - 29, 1.75f, 0);
    }
    
    for(int i = 34; i < 38; i++){//initiliaize all the platforms using for loop
        state.platforms[i].position = glm::vec3(2 + i - 35, 1.5f, 0);
    }
    
    state.platforms[37].position = glm::vec3(5.0, 0.25f, 0);
    
    
    for(size_t i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0, NULL, NULL, 0, NULL, 0);
    }
    
    state.enemies = new Entity[ENEMY_COUNT]; //initialize enemies
    GLuint enemyTextureID = LoadTexture("zombie_idle.png"); //credit to Kenney
    
    
    for(int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].speed = 0.2;
    }
    
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = WALKING;
    state.enemies[0].position = glm::vec3(-1, -1.25f, 0);
    
    state.enemies[1].aiType = PATROLLER;
    state.enemies[1].aiState = PATROLLING;
    state.enemies[1].movement.x = -1;
    state.enemies[1].position = glm::vec3(3.5f, -0.75f, 0);
    
    state.enemies[2].aiType = JUMPER;
    state.enemies[2].aiState = JUMPING;
    state.enemies[2].speed = 0;
    state.enemies[2].position = glm::vec3(2, 2.5f, 0);
    
    state.destination = new Entity();
    state.destination -> entityType = DESTINATION;
    state.destination -> textureID = LoadTexture("laserBlueBurst.png"); //credit to Kenney
    state.destination -> position = glm::vec3(-4.5f, 2.75f, 0);
    state.destination -> Update(0, NULL, NULL, 0, NULL, 0);
}

void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        if(state.player->collidedBottom){
                            state.player->jump = true;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
   
    while (deltaTime >= FIXED_TIMESTEP) {//tell player and enmeies to update
       // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
        
        for(int i = 0; i < ENEMY_COUNT; i++){
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
        }
       
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}

int enemyOvercomed = 0;

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for(size_t i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    
    for(size_t i = 0; i < ENEMY_COUNT; i++){//render enemies
        state.enemies[i].Render(&program);
    }
    
    state.destination -> Render(&program);
    
    state.player->Render(&program);
    
    if(state.player -> position.x <= -4.3f && state.player -> position.y >= 2.65f){
        for(int i = 0; i < ENEMY_COUNT; i++){
            if(state.enemies[i].isCounted == false && state.enemies[i].isActive == false){
                state.enemies[i].isCounted = true; //avoid overcounting
                enemyOvercomed++;
            }
        }
        if(enemyOvercomed == ENEMY_COUNT){
            DrawText(&program, fontTextureID, "You Win!", 0.5f, -0.25f, glm::vec3(-4.75f, 3.3, 0));
        }else{
            state.player -> speed = 0;
            DrawText(&program, fontTextureID, "You Lose!", 0.5f, -0.25f, glm::vec3(-4.75f, 3.3, 0));
        }
    }else if(state.player -> collidedELeft || state.player -> collidedERight){
        state.player -> speed = 0;
        DrawText(&program, fontTextureID, "You Lose!", 0.5f, -0.25f, glm::vec3(-4.75f, 3.3, 0));
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
