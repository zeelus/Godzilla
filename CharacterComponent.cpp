//
// Created by Gilbert Gwizdala on 12/12/2018.
//

#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>

#include "BallComponent.h"

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

    this->TryPlaySound(timeStep);

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

void CharacterComponent::TryPlaySound(float timeStep) {
    this->lastSoundPlay += timeStep;

    if(this->lastSoundPlay >= GODZILLA_SOUND_INTERWAL) {
        this->lastSoundPlay = 0.0f;
        auto* cache = GetSubsystem<ResourceCache>();
        auto* soundSorce = this->GetComponent<SoundSource>();
        if(soundSorce) {
            auto* sound = cache->GetResource<Sound>("Sounds/Godzilla.wav");
            soundSorce->Play(sound);
        }
    }
}

void CharacterComponent::Update(float timeStep) {
    this->lastEnergiiShot += timeStep;
    if(controls_.IsDown(CTRL_LEFT_MOUSE) && this->lastEnergiiShot >= 1.0) {
        this->createEnergiBall();
        this->lastEnergiiShot = 0.0f;
    }
}

void CharacterComponent::createEnergiBall() const {

    auto* cache = GetSubsystem<ResourceCache>();

    auto* ballNode = this->scene->CreateChild("BALL");

    auto* staticModel = ballNode->CreateComponent<StaticModel>();
    auto* model = cache->GetResource<Model>("Models/planet.mdl");

    staticModel->SetModel(model);

    auto thisPosition = this->node_->GetPosition();
    auto thisTransform = this->node_->GetWorldTransform();
    auto thisRostation = this->node_->GetRotation();

    ballNode->SetScale(Vector3(3.0f, 3.0f, 3.0f));
    ballNode->SetPosition(Vector3(thisPosition.x_, thisPosition.y_+ 50.0f, thisPosition.z_+ 30.0f));
    ballNode->RotateAround(thisPosition, thisRostation, TS_WORLD);


    auto* body = ballNode->CreateComponent<RigidBody>();
    body->SetMass(1.0f);
    Vector3 shot = thisRostation * Vector3::FORWARD * 400.0f;
    shot += Vector3::UP * 10.0f;
    body->ApplyImpulse(shot);

    auto* shape = ballNode->CreateComponent<CollisionShape>();
    shape->SetCapsule(1.0f, 1.0f);

    auto* emitter = ballNode->CreateComponent<ParticleEmitter>();
    auto* effect = cache->GetResource<ParticleEffect>("Particle/Energii.xml");
    emitter->SetEffect(effect);

    ballNode->CreateComponent<BallComponent>();

}

void CharacterComponent::SetScene(Scene *scene) {
    CharacterComponent::scene = scene;
}


