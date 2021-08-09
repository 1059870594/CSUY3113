#include "Menu.h"

GLuint fontTextureID;

void Menu::Initialize(int& life) {
    
    state.nextScene = -1; //stays at this scene
    
    fontTextureID = Util::LoadTexture("font1.png");
    
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void Menu::Update(float deltaTime, int& life) {
    return;
}

void Menu::Render(ShaderProgram *program) {
    Util::DrawText(program, fontTextureID, "Maze Adventure!", 0.5f, -0.25f, glm::vec3(-4.75f, 3.3f, 0));
    Util::DrawText(program, fontTextureID, "Press Enter to Start!", 0.5f, -0.25f, glm::vec3(-4.75f, 2.3f, 0));
    Util::DrawText(program, fontTextureID, "Rules: ", 0.5f, -0.25f, glm::vec3(-4.75f, 1.3f, 0));
    Util::DrawText(program, fontTextureID, "1: Collect a key to pass each level", 0.5f, -0.25f, glm::vec3(-4.75f, 0.3f, 0));
    Util::DrawText(program, fontTextureID, "2: Key will show up when you are nearby", 0.5f, -0.25f, glm::vec3(-4.75f, -0.7f, 0));
    Util::DrawText(program, fontTextureID, "3: Avoid AI, you get 3 lives", 0.5f, -0.25f, glm::vec3(-4.75f, -1.7f, 0));
}

