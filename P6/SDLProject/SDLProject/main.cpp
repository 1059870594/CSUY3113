#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"



#include "Entity.h"
#include "Map.h"
#include "Util.h"

#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Mix_Music *music;
Mix_Chunk *bounce;

Scene *currentScene;
Scene *sceneList[4];

GLuint fontTexture3ID;
bool win;
glm::vec3 textPosition;

int life = 3;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize(life);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Maze Adventure!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    //program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    program.Load("shaders/vertex_lit.glsl", "shaders/fragment_lit.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("voxel-revolution-by-kevin-macleod-from-filmmusic-io.mp3");
        //Voxel Revolution by Kevin MacLeod
        //Link: https://incompetech.filmmusic.io/song/7017-voxel-revolution
        //License: https://filmmusic.io/standard-license
    Mix_PlayMusic(music, -1);
    
    bounce = Mix_LoadWAV("446100__justinvoke__bounce.wav"); //credit to justInvoke on Freesound
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    fontTexture3ID = Util::LoadTexture("font1.png");
    
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]); //switch to level1
}

void ProcessInput() {
    
    if(currentScene != sceneList[0]){
        currentScene->state.player->movement = glm::vec3(0);
    }
        
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
                        break;
                        
                    case SDLK_RETURN:
                        if(currentScene == sceneList[0]){
                            currentScene->state.nextScene = 1;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if(currentScene != sceneList[0]){
        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }
        
        if (keys[SDL_SCANCODE_UP]) {
            currentScene->state.player->movement.y = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animUp;
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            currentScene->state.player->movement.y = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animDown;
        }
        
        if (glm::length(currentScene->state.player->movement) > 1.0f) {
            currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
        }
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
        currentScene->Update(FIXED_TIMESTEP, life);
        if(currentScene != sceneList[0]) program.SetLightPosition(currentScene->state.player->position);
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    
    if(currentScene != sceneList[0]){
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, -currentScene->state.player->position.y, 0));
   
    }
    
    if(currentScene != sceneList[0] && currentScene->state.player->keySound){
        Mix_PlayChannel(-1, bounce, 0);
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);
    
    if(currentScene != sceneList[0]){
        textPosition.x = currentScene->state.player->position.x - 4.6f;
        textPosition.y = currentScene->state.player->position.y + 3.4f;
        textPosition.z = 0;
        Util::DrawText(&program, fontTexture3ID, "lives: " + std::to_string(life), 0.5f, -0.25f, textPosition);
    }

    if(life == 0){
        currentScene->state.enemies[0].speed = 0;
        currentScene->state.player->speed = 0;
        Util::DrawText(&program, fontTexture3ID, "You lose", 0.5f, -0.25f, glm::vec3(textPosition.x, textPosition.y - 0.5, 0));
    }

    if(currentScene == sceneList[3] && currentScene->state.player->position.x > 13.9 && currentScene->state.player->position.y <= -5.9 && currentScene->state.player->collidedKey){
        win = true;
    }
    
    if(win){
        currentScene->state.enemies[0].speed = 0;
        currentScene->state.player->speed = 0;
        Util::DrawText(&program, fontTexture3ID, "You Win", 0.5f, -0.25f, glm::vec3(textPosition.x, textPosition.y - 0.5, 0));
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
        
        if(currentScene -> state.nextScene >= 0) SwitchToScene(sceneList[currentScene -> state.nextScene]);
        
        Render();
    }
    
    Shutdown();
    return 0;
}
