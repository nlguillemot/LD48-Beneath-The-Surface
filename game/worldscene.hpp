#ifndef WORLDSCENE_HPP
#define WORLDSCENE_HPP

#include "scene.hpp"
#include "billboard.hpp"

#include "rendercontext.hpp"
#include "debugdraw.hpp"

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

    bool IsMine;
};

class WorldScene : public Scene
{
    std::unique_ptr<GLplus::Program> mpModelProgram;
    std::unique_ptr<GLplus::Program> mpDebugProgram;

    std::unique_ptr<GLmesh::StaticMesh> mpWorldMesh;

    std::shared_ptr<GLplus::Texture2D> mpPlayerTexture;

    std::shared_ptr<GLplus::Texture2D> mpMoundTexture;
    std::vector<std::shared_ptr<GLplus::Texture2D>> mMoundNumberTextures;

    std::vector<std::unique_ptr<Billboard>> mBillboards;

    DebugDraw mDebugDraw;

    Player mPlayer;

    int mMoundsPerRow;
    std::vector<Mound> mMounds;

    Viewport mViewport;
    LookAtCamera mCamera;
    PerspectiveParams mPerspective;

    glm::mat4 mWorldViewMatrix;
    glm::mat4 mProjectionMatrix;

    bool mCameraRotating = false;

    void ResetMounds();

    void ClickMound(size_t moundIndex);
    std::vector<size_t> ZeroClosure(size_t moundIndex);
    std::vector<size_t> SurroundingMounds(size_t moundIndex);

    void UpdateWorldView();
    void UpdateProjection();

public:
    WorldScene();

    bool HandleEvent(const SDL_Event& event) override;
    void Update(unsigned int deltaTimeMS) override;
    void Render(RenderContext& renderContext, float partialUpdatePercentage) override;
};

#endif // WORLDSCENE_HPP
