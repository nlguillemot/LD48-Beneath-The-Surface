#ifndef SCENE_HPP
#define SCENE_HPP

class RenderContext;

class Scene
{
public:
    virtual ~Scene(){}

    virtual void Update(unsigned int deltaTimeMS) = 0;

    virtual void Render(RenderContext& renderContext, float partialUpdatePercentage) = 0;
};

#endif // SCENE_HPP
