#ifndef WORLDSCENE_HPP
#define WORLDSCENE_HPP

#include "scene.hpp"
#include "billboard.hpp"

#include <GLmesh.hpp>
#include <vector>

struct LookAtCamera
{
    glm::vec3 EyePosition;
    glm::vec3 TargetPosition;
    glm::vec3 UpVector;
};

class WorldScene : public Scene
{
    std::unique_ptr<GLmesh::StaticMesh> mpWorldMesh;

    std::shared_ptr<GLplus::Texture2D> mpPlayerTexture;

    std::vector<std::unique_ptr<Billboard>> mBillboards;

    std::unique_ptr<GLplus::Program> mModelProgram;

    LookAtCamera mCamera;

public:
    WorldScene();

    void Update(unsigned int deltaTimeMS) override;
    void Render(RenderContext& renderContext, float partialUpdatePercentage) override;
};

#endif // WORLDSCENE_HPP
