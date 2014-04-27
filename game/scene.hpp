#ifndef SCENE_HPP
#define SCENE_HPP

class RenderContext;
union SDL_Event;

class Scene
{
public:
    virtual ~Scene(){}

    virtual bool HandleEvent(const SDL_Event& event) = 0;

    virtual void Update(unsigned int deltaTimeMS) = 0;

    virtual void Render(RenderContext& renderContext, float partialUpdatePercentage) = 0;
};

#endif // SCENE_HPP
