//
// Created by Gilbert Gwizdala on 12/12/2018.
//

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Input/Controls.h>

#include "CharacterComponent.hpp"

using namespace Urho3D;

void CharacterComponent::RegisterObject(Context *context) {

    context->RegisterFactory<CharacterComponent>();

}

void CharacterComponent::Start() {
    LogicComponent::Start();

    body = GetComponent<RigidBody>();
}

void CharacterComponent::SetAnimationState(GodzillaState state) {

    auto* animationController = this->node_->GetChild("CharacterModel")->GetComponent<AnimationController>();
    switch(state) {
        case GodzillaState::IDLE :
            animationController->Play("Models/Godzilla/Animations/GodzillaIdle.ani", 0, true);
            break;
        case GodzillaState::RUN :
            animationController->Play("Models/Godzilla/Animations/GodzillaRun.ani", 0, true);
            break;
    }
}

void CharacterComponent::FixedUpdate(float timeStep) {
    LogicComponent::FixedUpdate(timeStep);

    const Quaternion& rot = node_->GetRotation();
    Vector3 moveDir = Vector3::ZERO;
    const Vector3& velocity = body->GetLinearVelocity();

    Vector3 planeVelocity(velocity.x_, 0.0f, velocity.z_);

    if (controls_.IsDown(CTRL_FORWARD))
        moveDir += Vector3::FORWARD;
    if (controls_.IsDown(CTRL_BACK))
        moveDir += Vector3::BACK;
    if (controls_.IsDown(CTRL_LEFT))
        moveDir += Vector3::LEFT;
    if (controls_.IsDown(CTRL_RIGHT))
        moveDir += Vector3::RIGHT;

    if (moveDir.LengthSquared() > 0.0f)
        moveDir.Normalize();

    body->ApplyImpulse(rot * moveDir * MOVE_FORCE);

}


