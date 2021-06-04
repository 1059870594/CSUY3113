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
glm::mat4 viewMatrix, playerMatrix, swordMatrix, projectionMatrix, sword2M;

float player_x = 0; //keep track of player_x's position
float player_rotate = 0; //keep track of player's rotation degrees
float player_direction = 1.0f;
float sword_rotate = 0;
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
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl"); //change to texture folder
    
    viewMatrix = glm::mat4(1.0f);
    playerMatrix = glm::mat4(1.0f);
    swordMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);
     
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //This object has a black background
    
    glEnable(GL_BLEND);
    //Turn on Transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    playerTextureID = LoadTexture("Geralt.png");//image credit to x-Naku-x, on DEVIANTART
    swordTextureID = LoadTexture("Fireblade.png");//image credit to Ehtan, on Pinterest
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f; //getTicks gives you the amount of ms since you initilized SDL
    float deltaTime = ticks - lastTicks; //calculate how much time gone by since last frame
    lastTicks = ticks;
    
    player_x += player_direction * deltaTime; //make sure you have this variable, update each time
    player_rotate += 90.0f * deltaTime;
    sword_rotate += 45.0f * deltaTime;
    
    if(player_x >= 5.0f){
        player_direction = -1.0f;
    }else if(player_x <= -5.0f){
        player_direction = 1.0f;
    }
    
    playerMatrix = glm::mat4(1.0f);
    playerMatrix = glm::translate(playerMatrix, glm::vec3(player_x,0.0f,0.0f));//translate x
    playerMatrix = glm::rotate(playerMatrix, glm::radians(player_rotate), glm::vec3(0.0f,0.0f,1.0f));
    
    swordMatrix = glm::mat4(1.0f);
    swordMatrix = glm::translate(swordMatrix, glm::vec3(-2.0,3.0f,0.0f));//translate x
    swordMatrix = glm::rotate(swordMatrix, glm::radians(sword_rotate), glm::vec3(0.0f,0.0f,1.0f));
}

void Render(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    program.SetModelMatrix(playerMatrix);//draw player and sword separately
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetModelMatrix(swordMatrix);
    glBindTexture(GL_TEXTURE_2D, swordTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
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
