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
    Util::DrawText(program, fontTextureID, "Platformer!", 0.5f, -0.25f, glm::vec3(-4.75f, 3.3, 0));
    Util::DrawText(program, fontTextureID, "Press Enter to Start!", 0.5f, -0.25f, glm::vec3(-4.75f, 0, 0));
}

