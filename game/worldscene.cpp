#include "worldscene.hpp"

#include "rendercontext.hpp"
#include "geometry.hpp"

#include <SDL2plus.hpp>

#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>

WorldScene::WorldScene()
{
    mModelProgram.reset(new GLplus::Program(GLplus::Program::FromFiles("world.vs","world.fs")));

    std::vector<tinyobj::shape_t> worldShapes;
    tinyobj::LoadObj(worldShapes, "floor.obj");
    if (worldShapes.empty())
    {
        throw std::runtime_error("floor model not available");
    }

    mpWorldMesh.reset(new GLmesh::StaticMesh());
    mpWorldMesh->LoadShape(worldShapes[0]);

    mpPlayerTexture.reset(new GLplus::Texture2D());
    float playerAspect;
    {
        GLplus::ScopedTexture2DBinding scopedTextureBind(*mpPlayerTexture);
        GLplus::Texture2DBinding& textureBind = scopedTextureBind.GetBinding();

        textureBind.LoadImage("player.png", GLplus::Texture2D::InvertY);
        playerAspect = (float) textureBind.GetWidth() / textureBind.GetHeight();
    }

    mpMoundTexture.reset(new GLplus::Texture2D());
    float moundAspect;
    {
        GLplus::ScopedTexture2DBinding scopedTextureBind(*mpMoundTexture);
        GLplus::Texture2DBinding& textureBind = scopedTextureBind.GetBinding();

        textureBind.LoadImage("mound.png", GLplus::Texture2D::InvertY);
        moundAspect = (float) textureBind.GetWidth() / textureBind.GetHeight();
    }

    // Add player
    mBillboards.emplace_back(new Billboard());
    mPlayer.BillboardID = mBillboards.size() - 1;
    std::unique_ptr<Billboard>& playerSprite = mBillboards.back();
    playerSprite->SetTexture(mpPlayerTexture);
    playerSprite->SetDimensions(glm::vec2(playerAspect, 1.0f) * 2.0f);
    playerSprite->SetCenterPosition(glm::vec3(0.0f, playerSprite->GetDimensions().y / 2.0f, 0.0f));

    // Add mounds
    int moundsPerRank = 10;
    for (int i = 0; i < moundsPerRank; i++)
    {
        for (int j = 0; j < moundsPerRank; j++)
        {
            mBillboards.emplace_back(new Billboard());
            std::unique_ptr<Billboard>& moundSprite = mBillboards.back();
            moundSprite->SetTexture(mpMoundTexture);
            moundSprite->SetDimensions(glm::vec2(moundAspect, 1.0f) * 0.7f);
            glm::vec3 uncenteredPosition = glm::vec3(i * 1.0f, moundSprite->GetDimensions().y / 2.0f, j * 1.0f);
            moundSprite->SetCenterPosition(uncenteredPosition
                                           - glm::vec3(moundsPerRank / 2.0f, 0.0f, moundsPerRank / 2.0f)
                                           + glm::vec3(0.5f, 0.0f, 0.5f));
            mMounds.emplace_back();
            Mound& mound = mMounds.back();
            mound.BillboardID = mBillboards.size() - 1;
            mound.State = MoundState::Untouched;
        }
    }

    mViewport.TopLeft = glm::ivec2(0);
    mViewport.Size = glm::ivec2(1,1); // temporary until first render

    mCamera.EyePosition = glm::vec3(7.0f);
    mCamera.TargetPosition = mBillboards[mPlayer.BillboardID]->GetCenterPosition();
    mCamera.UpVector = glm::vec3(0.0f,1.0f,0.0f);

    mPerspective.FovY = 70.0f;
    mPerspective.Aspect = mViewport.Size.x / mViewport.Size.y; // temporary until first render
    mPerspective.Near = 0.1f;
    mPerspective.Far = 1000.0f;
}

void print(const char* name, glm::vec3 v)
{
    printf("%s: { %f, %f, %f }\n", name, v.x, v.y, v.z);
    fflush(stdout);
}

bool WorldScene::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Window* clickedWindow = SDL_GetWindowFromID(event.button.windowID);
        if (!clickedWindow)
        {
            return false;
        }

        if (event.button.button == SDL_BUTTON_LEFT)
        {
            glm::vec3 rayOrigin = mCamera.EyePosition;
            print("rayOrigin", rayOrigin);

            glm::vec3 cameraView = glm::normalize(mCamera.TargetPosition - mCamera.EyePosition);
            glm::vec3 cameraSide = glm::cross(cameraView, glm::normalize(mCamera.UpVector));
            glm::vec3 cameraUpward = glm::cross(cameraSide, cameraView);

            print("cameraView", cameraView);
            print("cameraSide", cameraSide);
            print("cameraUpward", cameraUpward);

            int windowWidth, windowHeight;
            SDL_GetWindowSize(clickedWindow, &windowWidth, &windowHeight);
            float windowAspect = (float) windowWidth / windowHeight;

            static const float piOver180 = 0.017453293f;
            float farPlaneHalfHeight = mPerspective.Far * std::tan(mPerspective.FovY / 2.0f * piOver180);
            float farPlaneHalfWidth = windowAspect * farPlaneHalfHeight;

            glm::vec3 farPlaneMiddle = mCamera.EyePosition + cameraView * mPerspective.Far;

            glm::vec3 farPlaneIntersect = farPlaneMiddle
                    + ((float) (event.button.x - windowWidth  / 2) / windowWidth ) * farPlaneHalfWidth  * cameraSide
                    - ((float) (event.button.y - windowHeight / 2) / windowHeight) * farPlaneHalfHeight * cameraUpward;

            glm::vec3 rayDirection = glm::normalize(farPlaneIntersect - rayOrigin);

            print("farPlaneMiddle", farPlaneMiddle);
            print("farPlaneIntersect", farPlaneIntersect);
            print("rayDirection", rayDirection);

            Mound* closestMound = nullptr;
            float closest_t = INFINITY;
            for (Mound& mound : mMounds)
            {
                std::unique_ptr<Billboard>& sprite = mBillboards[mound.BillboardID];
                glm::vec3 bottomLeft, across, up;
                sprite->GetPlane(bottomLeft, across, up);

                float t;
                if (RayParallelogramIntersect(rayOrigin, rayDirection, bottomLeft, across, up, t))
                {
                    if (t < closest_t)
                    {
                        closestMound = &mound;
                        closest_t = t;
                    }
                }
            }

            if (closestMound && closestMound->State == MoundState::Uncovered)
            {
                closestMound->State = MoundState::Uncovered;
                mBillboards[closestMound->BillboardID]->SetDimensions(glm::vec2(0));
                printf("Mound uncovered!");
                fflush(stdout);
            }

            return true;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            printf("Camera rotating"); fflush(stdout);
            mCameraRotating = true;
            return true;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
            printf("Camera not rotating"); fflush(stdout);
            mCameraRotating = false;
            return true;
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        SDL_Window* clickedWindow = SDL_GetWindowFromID(event.button.windowID);
        if (!clickedWindow)
        {
            return false;
        }

        if (mCameraRotating)
        {
            int windowWidth;
            SDL_GetWindowSize(clickedWindow, &windowWidth, NULL);

            float rotationPercent = (float) event.motion.xrel / windowWidth;
            float rotationRadians = 6.283185307 * rotationPercent;

            float cs = glm::cos(rotationRadians);
            float sn = glm::sin(rotationRadians);

            glm::vec2 xz(mCamera.EyePosition.x, mCamera.EyePosition.z);
            glm::vec2 newxz;
            newxz.x = xz.x * cs - xz.y * sn;
            newxz.y = xz.x * sn + xz.y * cs;

            mCamera.EyePosition.x = newxz.x;
            mCamera.EyePosition.z = newxz.y;

            return true;
        }
    }

    return false;
}

void WorldScene::Update(unsigned int deltaTimeMS)
{
}

void WorldScene::Render(RenderContext& renderContext, float partialUpdatePercentage)
{
    mViewport = renderContext.CurrentViewport;
    mPerspective.Aspect = (float) mViewport.Size.x / mViewport.Size.y;

    glm::mat4 projection = glm::perspective(mPerspective.FovY, mPerspective.Aspect, mPerspective.Near, mPerspective.Far);
    glm::mat4 worldview = glm::lookAt(mCamera.EyePosition, mCamera.TargetPosition, mCamera.UpVector);

    GLplus::ScopedProgramBinding scopedProgramBinding(*mModelProgram);
    GLplus::ProgramBinding& programBinding = scopedProgramBinding.GetBinding();
    programBinding.UploadMatrix4("projection", GL_FALSE, &projection[0][0]);
    programBinding.UploadMatrix4("modelview", GL_FALSE, &worldview[0][0]);

    glEnable(GL_DEPTH_TEST);
    GLplus::CheckGLErrors();

    mpWorldMesh->Render(*mModelProgram);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLplus::CheckGLErrors();

    for (const std::unique_ptr<Billboard>& pBillboard : mBillboards)
    {
        if (!pBillboard)
        {
            continue;
        }

        pBillboard->SetCameraPosition(mCamera.EyePosition);
        pBillboard->SetCameraViewDirection(mCamera.TargetPosition - mCamera.EyePosition);
        pBillboard->SetCameraUp(mCamera.UpVector);
        pBillboard->Render(*mModelProgram);
    }
}
