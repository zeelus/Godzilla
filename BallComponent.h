//
// Created by Gilbert Gwizdala on 2019-01-28.
//

#ifndef GODZILLA_BALLCOMPONENT_H
#define GODZILLA_BALLCOMPONENT_H

#include <Urho3D/Scene/LogicComponent.h>

using namespace Urho3D;

class BallComponent: public LogicComponent {
    URHO3D_OBJECT(BallComponent, LogicComponent)

    bool isDestroided;
    bool onGround = false;

public:
    explicit BallComponent(Context * context);

    static void RegisterObject(Context* context);

    virtual void Start() override;

    virtual void FixedUpdate(float timeStep)  override;

    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
};


#endif //GODZILLA_BALLCOMPONENT_H
