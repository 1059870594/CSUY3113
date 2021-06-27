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
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#define PLATFORM_COUNT 25
#define SAFEPLATFORM_COUNT 2

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *safePlatforms;
    Entity *font;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

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
    displayWindow = SDL_CreateWindow("Lunar Explorer!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
   
    // Initialize Game Objects
    
    // Initialize Player
    
    fontTextureID = LoadTexture("font1.png");
    
    state.player = new Entity();
    state.player->position = glm::vec3(3.0f, 3.55f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-0.02f,0);
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("spaceship4.png");
    state.player->entityType = player;
    
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile040.png");
    
    for(int i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].entityType = platforms;
    }
    
    float x_coord = -4.5f;
    for(int i = 0; i < 3; i++){
        state.platforms[i].position = glm::vec3(x_coord, -3.25f, 0);
        x_coord += 1.0f;
    }
    
    x_coord += 2.0f;
    for(int i = 3; i < 8; i++){
        state.platforms[i].position = glm::vec3(x_coord, -3.25f, 0);
        x_coord += 1.0f;
    }

    float y1_coord = -2.25f;
    for(int j = 8; j < 15; j++){
        state.platforms[j].position = glm::vec3(-4.5f, y1_coord, 0);
        y1_coord += 1.0f;
    }
    
    float y2_coord = -2.25f;
    for(int j = 15; j < 22; j++){
        state.platforms[j].position = glm::vec3(4.5f, y2_coord, 0);
        y2_coord += 1.0f;
    }
    
    float x2_coord = -2.5f;
    for(int i = 22; i < 25; i++){
        state.platforms[i].position = glm::vec3(x2_coord, 0.25f, 0);
        x2_coord += 1.0f;
    }
    
    state.safePlatforms = new Entity[SAFEPLATFORM_COUNT];
    GLuint safePlatformTextureID = LoadTexture("platformPack_tile038.png");
    state.safePlatforms[0].textureID = safePlatformTextureID;
    state.safePlatforms[0].position = glm::vec3(-1.5f, -3.25f, 0);
    state.safePlatforms[1].textureID = safePlatformTextureID;
    state.safePlatforms[1].position = glm::vec3(-0.5f, -3.25f, 0);
    
    for(size_t i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Update(0, NULL, NULL,0, 0);
    }
    
    for(size_t i = 0; i < SAFEPLATFORM_COUNT; i++){
        state.safePlatforms[i].Update(0, NULL, NULL,0, 0);
        state.safePlatforms[i].entityType = safePlatforms;
    }
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
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->acceleration.x -= 1.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->acceleration.x += 1.0f;
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
   
    while (deltaTime >= FIXED_TIMESTEP) {
       // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.platforms, state.safePlatforms, PLATFORM_COUNT, SAFEPLATFORM_COUNT); //check player hits platform
       deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for(size_t i = 0; i < PLATFORM_COUNT; i++){
        state.platforms[i].Render(&program);
    }
    
    for(size_t i = 0; i < safePlatforms; i++){
        state.safePlatforms[i].Render(&program);
    }
    
    state.player->Render(&program);
    
    if(state.player->lastCollison == safePlatforms){
        DrawText(&program, fontTextureID, "Mission Successful!", 0.5f, -0.25f, glm::vec3(-4.75f, 3.3, 0));
    }else if(state.player->lastCollison == platforms){
        DrawText(&program, fontTextureID, "Mission Failed!", 0.5f, -0.25f, glm::vec3(-4.75f, 3.3, 0));
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
