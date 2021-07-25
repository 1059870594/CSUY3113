#include "Scene.h"

class Level3 : public Scene {
public:
    void Initialize(int& life) override;
    void Update(float deltaTime, int& life) override;
    void Render(ShaderProgram *program) override;
};
