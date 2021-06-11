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

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, paddleOneMatrix, paddleTwoMatrix, ballMatrix, projectionMatrix;

glm::vec3 paddleOne_position = glm::vec3(0,0,0);
glm::vec3 paddleOne_movement = glm::vec3(0,0,0);
float paddleOne_speed = 1.0f;

glm::vec3 paddleTwo_position = glm::vec3(0,0,0);
glm::vec3 paddleTwo_movement = glm::vec3(0,0,0);
float paddleTwo_speed = 1.0f;

float ball_x, ball_y; //keep track of the ball's position

GLuint playerTextureID, swordTextureID;

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

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Geralt and His Sword!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl"); //change to texture folder
    
    viewMatrix = glm::mat4(1.0f);
    paddleOneMatrix = glm::mat4(1.0f);
    paddleTwoMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);
     
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //This object has a black background
    
}

void ProcessInput() {
    paddleOne_movement = glm::vec3(0);
    paddleTwo_movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_LEFT:
                        break;
                    case SDLK_RIGHT:
                        break;
                    case SDLK_SPACE:
                        break;
                }
                break;
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    if(keys[SDL_SCANCODE_W]){
        paddleOne_movement.y += 1.0f;
    }
    else if(keys[SDL_SCANCODE_S]){
        paddleOne_movement.y += -1.0f;
    }
    
    if(keys[SDL_SCANCODE_UP]){
        paddleTwo_movement.y += 1.0f;
    }
    else if(keys[SDL_SCANCODE_DOWN]){
        paddleTwo_movement.y += -1.0f;
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f; //getTicks gives you the amount of ms since you initilized SDL
    float deltaTime = ticks - lastTicks; //calculate how much time gone by since last frame
    lastTicks = ticks;
    
    paddleOne_position += paddleOne_speed * paddleOne_movement * deltaTime; //make sure you have this variable, update each time
    paddleTwo_position += paddleTwo_speed * paddleTwo_movement * deltaTime; //it seems that this postion refers to the center of rectangle
    ball_x += 1.0f * deltaTime;
    ball_y += 1.0f * deltaTime;
    
    if(paddleOne_position.y > 3.00f){ //the edge of the rectangle touches the window at 3, -3; if set to > 3.75f, half of the renctangle would disappear
        paddleOne_position.y = 3.00f;
    }
    if(paddleOne_position.y < -3.00f){
        paddleOne_position.y = -3.00f;
    }
    
    if(paddleTwo_position.y > 3.00f){
        paddleTwo_position.y = 3.00f;
    }
    if(paddleTwo_position.y < -3.00f){
        paddleTwo_position.y = -3.00f;
    }
    
    paddleOneMatrix = glm::mat4(1.0f);
    paddleOneMatrix = glm::translate(paddleOneMatrix, paddleOne_position);
    
    paddleTwoMatrix = glm::mat4(1.0f);
    paddleTwoMatrix = glm::translate(paddleTwoMatrix, paddleTwo_position);
    
    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::translate(ballMatrix, glm::vec3(ball_x, ball_y, 0.0f));
}

void Render(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    //float vertices[]  = {0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f};
    
    //glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    program.SetModelMatrix(paddleOneMatrix);
    glRectf(-5.0f,0.75f,-4.75f,-0.75f);
    
    program.SetModelMatrix(paddleTwoMatrix);
    glRectf(4.75f,0.75f,5.0f,-0.75f);
    
    program.SetModelMatrix(ballMatrix);
    glRectf(-0.2f, 0.2f, 0.2f, -0.2f);
    
    glDisableVertexAttribArray(program.positionAttribute);
    
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
