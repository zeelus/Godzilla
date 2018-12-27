//
// Created by Gilbert Gwizdala on 12/12/2018.
//

#ifndef GODZILLA_CHARACTERCOMPONENT_HPP
#define GODZILLA_CHARACTERCOMPONENT_HPP

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/RigidBody.h>


const unsigned CTRL_FORWARD = 1;
const unsigned CTRL_BACK = 2;
const unsigned CTRL_LEFT = 4;
const unsigned CTRL_RIGHT = 8;

const float MOVE_FORCE = 150.0f;
const float BRAKE_FORCE = 2.0f;

using namespace Urho3D;

enum class GodzillaState {
    IDLE,
    RUN
};

class CharacterComponent: public LogicComponent {

    URHO3D_OBJECT(CharacterComponent, LogicComponent)

    RigidBody *body;

    bool onGround = true;

public:

    static void RegisterObject(Context* context);

    explicit CharacterComponent(Context* context);

    void SetAnimationState(GodzillaState state);

    Controls controls_;

    void FixedUpdate(float timeStep) override;

    void Start() override;

    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
};


#endif //GODZILLA_CHARACTERCOMPONENT_HPP
