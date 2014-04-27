#include "worldscene.hpp"

#include "rendercontext.hpp"

#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>

WorldScene::WorldScene()
{
    std::vector<tinyobj::shape_t> worldShapes;
    tinyobj::LoadObj(worldShapes, "floor.obj");
    if (worldShapes.empty())
    {
        throw std::runtime_error("floor model not available");
    }

    mpWorldMesh.reset(new GLmesh::StaticMesh());
    mpWorldMesh->LoadShape(worldShapes[0]);

    mpPlayerTexture.reset(new GLplus::Texture2D());
    {
        GLplus::ScopedTexture2DBinding scopedTextureBind(*mpPlayerTexture);
        scopedTextureBind.GetBinding().LoadImage("player.png", GLplus::Texture2D::InvertY);
    }

    mBillboards.emplace_back(new Billboard());
    std::unique_ptr<Billboard>& player = mBillboards.back();
    player->SetTexture(mpPlayerTexture);
    player->SetCenterPosition(glm::vec3(0.0f,1.0f,0.0f));
    player->SetDimensions(glm::vec2(2.0f,2.0f));

    mModelProgram.reset(new GLplus::Program(GLplus::Program::FromFiles("world.vs","world.fs")));

    mCamera.EyePosition = glm::vec3(3.0f);
    mCamera.TargetPosition = glm::vec3(0.0f);
    mCamera.UpVector = glm::vec3(0.0f,1.0f,0.0f);
}

void WorldScene::Update(unsigned int deltaTimeMS)
{

}

void WorldScene::Render(RenderContext& renderContext, float partialUpdatePercentage)
{
    float aspect = renderContext.GetAspectRatio();
    glm::mat4 projection = glm::perspective(70.0f, aspect, 0.1f, 1000.0f);
    glm::mat4 worldview = glm::lookAt(mCamera.EyePosition, mCamera.TargetPosition, mCamera.UpVector);

    GLplus::ScopedProgramBinding scopedProgramBinding(*mModelProgram);
    GLplus::ProgramBinding& programBinding = scopedProgramBinding.GetBinding();
    programBinding.UploadMatrix4("projection", GL_FALSE, &projection[0][0]);
    programBinding.UploadMatrix4("modelview", GL_FALSE, &worldview[0][0]);

    glEnable(GL_DEPTH_TEST);

    mpWorldMesh->Render(*mModelProgram);

    for (const std::unique_ptr<Billboard>& pBillboard : mBillboards)
    {
        pBillboard->SetCameraPosition(mCamera.EyePosition);
        pBillboard->SetCameraViewDirection(mCamera.TargetPosition - mCamera.EyePosition);
        pBillboard->SetCameraUp(mCamera.UpVector);
        pBillboard->Render(*mModelProgram);
    }
}
