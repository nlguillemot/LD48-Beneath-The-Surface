#ifndef WORLDSCENE_HPP
#define WORLDSCENE_HPP

#include "scene.hpp"
#include "billboard.hpp"

#include "rendercontext.hpp"

#include <GLmesh.hpp>
#include <vector>

struct LookAtCamera
{
    glm::vec3 EyePosition;
    glm::vec3 TargetPosition;
    glm::vec3 UpVector;
};

struct PerspectiveParams
{
    float FovY;
    float Aspect;
    float Near;
    float Far;
};

struct Player
{
    size_t BillboardID;
};

enum class MoundState
{
    Untouched,
    Uncovered,
    Flagged
};

struct Mound
{
    size_t BillboardID;

    MoundState State;
};

class WorldScene : public Scene
{
    std::unique_ptr<GLplus::Program> mModelProgram;

    std::unique_ptr<GLmesh::StaticMesh> mpWorldMesh;

    std::shared_ptr<GLplus::Texture2D> mpPlayerTexture;
    std::shared_ptr<GLplus::Texture2D> mpMoundTexture;

    std::vector<std::unique_ptr<Billboard>> mBillboards;

    Player mPlayer;
    std::vector<Mound> mMounds;

    Viewport mViewport;
    LookAtCamera mCamera;
    PerspectiveParams mPerspective;

    bool mCameraRotating = false;

    void ResetMounds();

public:
    WorldScene();

    bool HandleEvent(const SDL_Event& event) override;
    void Update(unsigned int deltaTimeMS) override;
    void Render(RenderContext& renderContext, float partialUpdatePercentage) override;
};

#endif // WORLDSCENE_HPP
