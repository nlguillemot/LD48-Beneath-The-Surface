#include "worldscene.hpp"

#include "rendercontext.hpp"
#include "geometry.hpp"

#include <SDL2plus.hpp>

#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <algorithm>
#include <stdexcept>

WorldScene::WorldScene()
{
    mpModelProgram.reset(new GLplus::Program(GLplus::Program::FromFiles("world.vs","world.fs")));
    mpDebugProgram.reset(new GLplus::Program(GLplus::Program::FromFiles("debug.vs","debug.fs")));

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

    for (int number = 0; number < 9; number++)
    {
        mMoundNumberTextures.emplace_back(new GLplus::Texture2D());
        GLplus::ScopedTexture2DBinding scopedTextureBind(*mMoundNumberTextures.back());
        GLplus::Texture2DBinding& textureBind = scopedTextureBind.GetBinding();

        std::string filename = "mound" + std::to_string(number) + ".png";
        textureBind.LoadImage(filename.c_str(), GLplus::Texture2D::InvertY);
    }

    // Add player
    mBillboards.emplace_back(new Billboard());
    mPlayer.BillboardID = mBillboards.size() - 1;
    std::unique_ptr<Billboard>& playerSprite = mBillboards.back();
    playerSprite->SetTexture(mpPlayerTexture);
    playerSprite->SetDimensions(glm::vec2(playerAspect, 1.0f) * 2.0f);
    playerSprite->SetCenterPosition(glm::vec3(0.0f, playerSprite->GetDimensions().y / 2.0f, 0.0f));

    // Add mounds
    mMoundsPerRow = 10;
    for (int i = 0; i < mMoundsPerRow; i++)
    {
        for (int j = 0; j < mMoundsPerRow; j++)
        {
            mBillboards.emplace_back(new Billboard());
            std::unique_ptr<Billboard>& moundSprite = mBillboards.back();
            moundSprite->SetTexture(mpMoundTexture);
            moundSprite->SetDimensions(glm::vec2(moundAspect, 1.0f) * 0.7f);
            glm::vec3 uncenteredPosition = glm::vec3(i * 1.0f, moundSprite->GetDimensions().y / 2.0f, j * 1.0f);
            moundSprite->SetCenterPosition(uncenteredPosition
                                           - glm::vec3(mMoundsPerRow / 2.0f, 0.0f, mMoundsPerRow / 2.0f)
                                           + glm::vec3(0.5f, 0.0f, 0.5f));
            mMounds.emplace_back();
            Mound& mound = mMounds.back();
            mound.BillboardID = mBillboards.size() - 1;
            mound.State = MoundState::Untouched;
        }
    }

    ResetMounds();

    mViewport.TopLeft = glm::ivec2(0);
    mViewport.Size = glm::ivec2(1,1); // temporary until first render

    mCamera.EyePosition = glm::vec3(7.0f, 10.0f, 7.0f);
    mCamera.TargetPosition = mBillboards[mPlayer.BillboardID]->GetCenterPosition();
    mCamera.UpVector = glm::vec3(0.0f,1.0f,0.0f);

    mPerspective.FovY = 70.0f;
    mPerspective.Aspect = mViewport.Size.x / mViewport.Size.y; // temporary until first render
    mPerspective.Near = 0.1f;
    mPerspective.Far = 1000.0f;

    UpdateWorldView();
    UpdateProjection();
}

void print(const char* name, glm::vec3 v)
{
    printf("%s: { %f, %f, %f }\n", name, v.x, v.y, v.z);
    fflush(stdout);
}

void WorldScene::ResetMounds()
{
    // reset state of mounds
    for (Mound& mound : mMounds)
    {
        mound.State = MoundState::Untouched;
        mound.IsMine = false;
        mBillboards[mound.BillboardID]->SetTexture(mpMoundTexture);
    }

    // allocate mines
    int numMines = 20;

    std::random_device randomDevice;
    std::default_random_engine randomEngine(randomDevice());
    std::uniform_int_distribution<int> uniformDist(0, mMounds.size() - 1);

    for (int minesLeft = numMines; minesLeft > 0 && numMines - minesLeft < mMounds.size(); minesLeft--)
    {
        int chosen;
retry:
        chosen = uniformDist(randomEngine);
        if (mMounds[chosen].IsMine)
        {
            goto retry;
        }

        mMounds[chosen].IsMine = true;
    }
}

void WorldScene::ClickMound(size_t moundIndex)
{
    if (mMounds.at(moundIndex).IsMine)
    {
        printf("You died!\n"); fflush(stdout);
        ResetMounds();
    }
    else
    {
        std::vector<size_t> toUnCover = ZeroClosure(moundIndex);
        toUnCover.push_back(moundIndex);
        for (size_t index : toUnCover)
        {
            printf("Uncovered %zu\n", index);
            mMounds[index].State = MoundState::Uncovered;
            mBillboards[mMounds[index].BillboardID]->SetTexture(mMoundNumberTextures[0]);
        }
        fflush(stdout);
    }
}

std::vector<size_t> WorldScene::ZeroClosure(size_t moundIndex)
{
    std::vector<size_t> closure;

    std::vector<size_t> toCheck{moundIndex};
    while (!toCheck.empty())
    {
        if (std::find(closure.begin(), closure.end(), toCheck.back()) != closure.end())
        {
            toCheck.pop_back();
        }
        else if (mMounds[toCheck.back()].IsMine)
        {
            toCheck.pop_back();
        }
        else
        {
            std::vector<size_t> surroundings = SurroundingMounds(toCheck.back());
            int numSurroundingMines = 0;
            for (size_t surrounding : surroundings)
            {
                if (mMounds[surrounding].IsMine)
                {
                    numSurroundingMines++;
                }
            }

            if (numSurroundingMines == 0)
            {
                closure.push_back(toCheck.back());
                toCheck.pop_back();
                toCheck.insert(toCheck.end(), surroundings.begin(), surroundings.end());
            }
            else
            {
                toCheck.pop_back();
            }
        }
    }

    return closure;
}

std::vector<size_t> WorldScene::SurroundingMounds(size_t moundIndex)
{
    if (moundIndex >= mMounds.size()) throw std::out_of_range("moundIndex");

    std::vector<size_t> surroundingMounds;

    int cy = moundIndex / mMoundsPerRow;
    int cx = moundIndex - cy * mMoundsPerRow;

    for (int dy = -1; dy <= 1; dy++)
    {
        int y = cy + dy;
        for (int dx = -1; dx <= 1; dx++)
        {
            if (!(dx == 0 && dy == 0))
            {
                int x = cx + dx;
                if (x >= 0 && x < mMoundsPerRow && y >= 0 && y < mMoundsPerRow)
                {
                    surroundingMounds.push_back(y * mMoundsPerRow + x);
                }
            }
        }
    }

    return surroundingMounds;
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
            int windowWidth, windowHeight;
            SDL_GetWindowSize(clickedWindow, &windowWidth, &windowHeight);

            glm::vec3 rayStart = glm::unProject(
                        glm::vec3((float) event.button.x, (float) windowHeight - event.button.y, 0.0f),
                        mWorldViewMatrix, mProjectionMatrix,
                        glm::vec4(0.0f, 0.0f, (float) windowWidth, (float) windowHeight));

            glm::vec3 rayEnd = glm::unProject(
                        glm::vec3((float) event.button.x, (float) windowHeight - event.button.y, 1.0f),
                        mWorldViewMatrix, mProjectionMatrix,
                        glm::vec4(0.0f, 0.0f, (float) windowWidth, (float) windowHeight));

            Mound* closestMound = nullptr;
            float closest_t = INFINITY;
            for (Mound& mound : mMounds)
            {
                std::unique_ptr<Billboard>& sprite = mBillboards[mound.BillboardID];
                glm::vec3 bottomLeft, across, up;
                sprite->GetPlane(bottomLeft, across, up);

                float t;
                if (RayParallelogramIntersect(rayStart, rayEnd - rayStart, bottomLeft, across, up, t))
                {
                    if (t < closest_t)
                    {
                        closestMound = &mound;
                        closest_t = t;
                    }
                }
            }

            if (closestMound && closestMound->State == MoundState::Untouched)
            {
                ClickMound(closestMound - mMounds.data());
            }

            return true;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            mCameraRotating = true;
            return true;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
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
    else if (event.type == SDL_MOUSEWHEEL)
    {
        if (event.wheel.y != 0)
        {
            glm::vec3 toEye = mCamera.EyePosition - mCamera.TargetPosition;
            toEye *= event.wheel.y > 0 ? 0.9f : 1.11f;
            mCamera.EyePosition = mCamera.TargetPosition + toEye;
            return true;
        }
    }

    return false;
}

void WorldScene::Update(unsigned int deltaTimeMS)
{
}

void WorldScene::UpdateWorldView()
{
    mWorldViewMatrix = glm::lookAt(mCamera.EyePosition, mCamera.TargetPosition, mCamera.UpVector);
}

void WorldScene::UpdateProjection()
{
    mProjectionMatrix = glm::perspective(mPerspective.FovY, mPerspective.Aspect, mPerspective.Near, mPerspective.Far);
}

void WorldScene::Render(RenderContext& renderContext, float partialUpdatePercentage)
{
    mViewport = renderContext.CurrentViewport;
    mPerspective.Aspect = (float) mViewport.Size.x / mViewport.Size.y;

    UpdateWorldView();
    UpdateProjection();

    {
        GLplus::ScopedProgramBinding scopedProgramBinding(*mpModelProgram);
        GLplus::ProgramBinding& programBinding = scopedProgramBinding.GetBinding();
        programBinding.UploadMatrix4("projection", GL_FALSE, &mProjectionMatrix[0][0]);
        programBinding.UploadMatrix4("modelview", GL_FALSE, &mWorldViewMatrix[0][0]);

        glEnable(GL_DEPTH_TEST);
        GLplus::CheckGLErrors();

        mpWorldMesh->Render(*mpModelProgram);

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
            pBillboard->Render(*mpModelProgram);
        }
    }

    {
        GLplus::ScopedProgramBinding scopedProgramBinding(*mpDebugProgram);
        GLplus::ProgramBinding& programBinding = scopedProgramBinding.GetBinding();
        programBinding.UploadMatrix4("projection", GL_FALSE, &mProjectionMatrix[0][0]);
        programBinding.UploadMatrix4("modelview", GL_FALSE, &mWorldViewMatrix[0][0]);

        mDebugDraw.SetLineWidth(2.0f);
        mDebugDraw.Render(*mpDebugProgram);
    }
}
