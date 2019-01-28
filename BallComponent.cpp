//
// Created by Gilbert Gwizdala on 2019-01-28.
//

#include "BallComponent.h"

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>

void BallComponent::RegisterObject(Context *context) {
    context->RegisterFactory<BallComponent>();
}

BallComponent::BallComponent(Context *context) : LogicComponent(context) {

}

void BallComponent::Start() {
    LogicComponent::Start();
    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(BallComponent, HandleNodeCollision));
}

void BallComponent::FixedUpdate(float timeStep) {
    LogicComponent::FixedUpdate(timeStep);

    if(!isDestroided && onGround) {
        this->node_->Remove();
    }
}

void BallComponent::HandleNodeCollision(StringHash eventType, VariantMap &eventData) {
    using namespace NodeCollision;

    onGround = false;

    auto contactNode = dynamic_cast<Node*>(eventData[P_OTHERNODE].GetPtr());

    if(contactNode) {
        auto terrain = contactNode->GetComponent<Terrain>();
        if(terrain) onGround = true;
    }
}
