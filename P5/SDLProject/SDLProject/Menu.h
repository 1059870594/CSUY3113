#include "Scene.h"

class Menu : public Scene {
public:
    void Initialize(int& life) override;
    void Update(float deltaTime, int& life) override;
    void Render(ShaderProgram *program) override;
};
