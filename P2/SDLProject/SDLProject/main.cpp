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
float paddleOne_speed = 0.0f;

glm::vec3 paddleTwo_position = glm::vec3(0,0,0);
glm::vec3 paddleTwo_movement = glm::vec3(0,0,0);
float paddleTwo_speed = 0.0f;

glm::vec3 ball_position = glm::vec3(0,0,0);
float ball_x_direction = 0.0f;
float ball_y_direction = 0.0f;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Geralt and His Sword!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    paddleOneMatrix = glm::mat4(1.0f);
    paddleTwoMatrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);
     
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); 
    
}

int space_press_time = 0;

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
                        space_press_time += 1;
                        if(space_press_time == 1){
                            paddleOne_speed = 2.0f;
                            paddleTwo_speed = 2.0f;
                            ball_x_direction = 1.0f;
                            ball_y_direction = 1.0f;
                        }else{//hit space more than once ends the game
                            ball_x_direction = 0.0f;
                            ball_y_direction = 0.0f;
                            paddleOne_speed = 0.0f;
                            paddleTwo_speed = 0.0f;
                        }
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

void paddle_work_correctly(){
    if(paddleOne_position.y > 3.00f){ //the edge of the rectangle touches the window at 3, -3; if set to > 3.75f, half of the renctangle would disappear
        paddleOne_position.y = 3.00f;
    }
    else if(paddleOne_position.y < -3.00f){
        paddleOne_position.y = -3.00f;
    }
    
    if(paddleTwo_position.y > 3.00f){
        paddleTwo_position.y = 3.00f;
    }
    else if(paddleTwo_position.y < -3.00f){
        paddleTwo_position.y = -3.00f;
    }
}

void ball_work_correctly(){
    if(ball_position.x >= 4.8f or ball_position.x <= -4.8f){//game over
        ball_y_direction = 0.0f;
        ball_x_direction = 0.0f;
        paddleOne_speed = 0.0f;
        paddleTwo_speed = 0.0f;
    }else{//games continues
        if(ball_position.y >= 3.55f){//hit the top from the left or right, ball needs to bounce down
            ball_position.y = 3.55f;
            ball_y_direction = -1.0f;
        }
        else if(ball_position.y <= -3.55f){//hit the bot from the left or right, ball needs to bounce up
            ball_position.y = -3.55f;
            ball_y_direction = 1.0f;
        }
        //need to cap the y position of the ball within -3.55 to 3.55
        
        float xdist_right = fabs(4.875f - ball_position.x) - ((0.4+0.25)/2.0f);
        float ydist_right = fabs(paddleTwo_position.y - ball_position.y) - ((0.4+1.5)/2.0f);
        float xdist_left = fabs(-4.875f - ball_position.x) - ((0.4+0.25)/2.0f);
        float ydist_left = fabs(paddleOne_position.y - ball_position.y) - ((0.4+1.5)/2.0f);
        
        //4.875f is the actual x position of paddleOne
        //std::cout << "x: " << paddleOne_position.x << std::endl; why these x position = 0?
        
        if(ball_x_direction > 0.0f && (xdist_right < 0 && ydist_right < 0)){//hit right paddle
            ball_x_direction = -1.0f;
        }
        else if(ball_x_direction < 0.0f && (xdist_left < 0 && ydist_left < 0)){//hit left paddle
            ball_x_direction = 1.0f;
        }
    }
}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f; //getTicks gives you the amount of ms since you initilized SDL
    float deltaTime = ticks - lastTicks; //calculate how much time gone by since last frame
    lastTicks = ticks;
    
    paddleOne_position += paddleOne_speed * paddleOne_movement * deltaTime;
    paddleTwo_position += paddleTwo_speed * paddleTwo_movement * deltaTime; //it seems that this postion refers to the center of rectangle
    ball_position.x += ball_x_direction * 2.0f * deltaTime;
    ball_position.y += ball_y_direction * 2.0f * deltaTime;
    
    paddle_work_correctly();
    ball_work_correctly();
    
    paddleOneMatrix = glm::mat4(1.0f);
    paddleOneMatrix = glm::translate(paddleOneMatrix, paddleOne_position);
    
    paddleTwoMatrix = glm::mat4(1.0f);
    paddleTwoMatrix = glm::translate(paddleTwoMatrix, paddleTwo_position);
    
    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::translate(ballMatrix, ball_position);
}

void Render(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnableVertexAttribArray(program.positionAttribute);
    
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
