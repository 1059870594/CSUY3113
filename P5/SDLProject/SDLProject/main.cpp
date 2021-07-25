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

Scene *currentScene;
//Scene *sceneList[2]; //two scenes
Scene *sceneList[4];

GLuint fontTexture3ID;

int life = 3;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    //currentScene->Initialize();
    currentScene->Initialize(life);
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
                        if(currentScene != sceneList[0]){
                            if(currentScene->state.player->collidedBottom){
                                currentScene->state.player->jump = true;
                            }
                        }
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
        //currentScene->Update(FIXED_TIMESTEP);
        currentScene->Update(FIXED_TIMESTEP, life);
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    
    if(currentScene != sceneList[0]){
        
        /*
        if (currentScene->state.player->position.x > 5) { //only move the camera if the player's position greater than 5 ; left boundary
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
            std::cout << currentScene -> state.player->position.x << std::endl;
        } else {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }
         */
        
        
        if (currentScene->state.player->position.x >= 5 && currentScene->state.player->position.x <= 8) { //only move the camera if the player's position greater than 5 ; left boundary
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        } else if(currentScene->state.player->position.x < 5){
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        } else if(currentScene->state.player->position.x > 8){
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-8, 3.75, 0));
        }
        
        
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);
    
    Util::DrawText(&program, fontTexture3ID, "lives: " + std::to_string(life), 0.5f, -0.25f, glm::vec3(8, -4.5f, 0));
    if(life == 0){
        Util::DrawText(&program, fontTexture3ID, "You lose", 0.5f, -0.25f, glm::vec3(8, -4.5f, 0));
    }
    if(currentScene == sceneList[3] && currentScene->state.player->position.x > 11.5  && fabs(currentScene->state.player->position.y + 3) <= 0.5f){
        Util::DrawText(&program, fontTexture3ID, "You Win", 0.5f, -0.25f, glm::vec3(8, -4.5f, 0));
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
