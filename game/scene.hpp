#ifndef SCENE_HPP
#define SCENE_HPP

class Scene
{
public:
    virtual ~Scene(){}

    virtual void Update(Uint32 deltaTimeMS) = 0;

    virtual void Render(float partialUpdatePercentage) = 0;
};

#endif // SCENE_HPP
