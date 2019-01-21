//
// Created by Gilbert Gwizdala on 12/12/2018.
//

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/IO/MemoryBuffer.h>

#include "CharacterComponent.hpp"

using namespace Urho3D;

void CharacterComponent::RegisterObject(Context *context) {

    context->RegisterFactory<CharacterComponent>();

}

void CharacterComponent::Start() {
    LogicComponent::Start();

    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(CharacterComponent, HandleNodeCollision));
    body = GetComponent<RigidBody>();
}

void CharacterComponent::SetAnimationState(GodzillaState state) {

    auto* animationController = this->node_->GetChild("CharacterModel")->GetComponent<AnimationController>();
    animationController->StopAll();
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

    moveDir += Vector3::DOWN;

    //if(onGround)
    body->ApplyImpulse(rot * moveDir * MOVE_FORCE);

    Vector3 brakeForce = -planeVelocity * BRAKE_FORCE;
    body->ApplyImpulse(brakeForce);

    moveDir -= Vector3::DOWN;

    if (!(moveDir.Equals(Vector3::ZERO))) {
        this->SetAnimationState(GodzillaState::RUN);
    } else {
        this->SetAnimationState(GodzillaState::IDLE);
    }

    onGround = false;

}

void CharacterComponent::HandleNodeCollision(StringHash eventType, VariantMap &eventData) {

    using namespace NodeCollision;

    MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    while (!contacts.IsEof())
    {
        Vector3 contactPosition = contacts.ReadVector3();
        Vector3 contactNormal = contacts.ReadVector3();
        /*float contactDistance = */contacts.ReadFloat();
        /*float contactImpulse = */contacts.ReadFloat();

        if (contactPosition.y_ < (node_->GetPosition().y_ + 1.0f))
        {
            float level = contactNormal.y_;
            if (level > 0.75) {
                onGround = true;
            }
        }
    }
}

CharacterComponent::CharacterComponent(Context *context) : LogicComponent(context) {

}


