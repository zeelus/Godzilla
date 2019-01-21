//
// Created by Gilbert Gwizdala on 2019-01-21.
//

#ifndef GODZILLA_BOXCOMPONENT_HPP
#define GODZILLA_BOXCOMPONENT_HPP

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

static float BOX_LIFE_TIME = 10.0f;

class BoxComponent: public LogicComponent {

    URHO3D_OBJECT(BoxComponent, LogicComponent)

    float lifeTime;
    bool isDestroided;
    bool onGround = false;

public:

    explicit BoxComponent(Context * context);

    static void RegisterObject(Context* context);

    virtual void Start() override;

    virtual void FixedUpdate(float timeStep)  override;

    void DestroyedBox();

    void ShowFire();

    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

};


#endif //GODZILLA_BOXCOMPONENT_HPP
