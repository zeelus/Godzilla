//
// Created by Gilbert Gwizdala on 2019-01-21.
//

#include "BoxComponent.hpp"
#include "GodzillaMainScene.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Physics/PhysicsEvents.h>

BoxComponent::BoxComponent(Context *context) : LogicComponent(context) {

}


void BoxComponent::RegisterObject(Context* context) {
    context->RegisterFactory<BoxComponent>();
}

void BoxComponent::Start() {
    LogicComponent::Start();

    this->lifeTime = 0.0f;
    this->isDestroided = false;
    this->ShowFire();

    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(BoxComponent, HandleNodeCollision));
}

void BoxComponent::FixedUpdate(float timeStep) {
    this->lifeTime += timeStep;

    if(!isDestroided && onGround) {
        this->DestroyedBox();
        this->lifeTime = 0.0;
    }


    if(isDestroided && (this->lifeTime >= BOX_FIRE_LIFE_TIME)) {
        this->node_->Remove();
    }
}

void BoxComponent::DestroyedBox() {

    this->isDestroided = true;

    this->GetComponent<RigidBody>()->Remove();
    this->GetComponent<CollisionShape>()->Remove();
    this->GetComponent<StaticModel>()->Remove();
}

void BoxComponent::ShowFire() {
    auto* cache = GetSubsystem<ResourceCache>();
    auto* emitter = this->node_->CreateComponent<ParticleEmitter>();
    auto* effect = cache->GetResource<ParticleEffect>("Particle/torch_fire.xml");
    emitter->SetEffect(effect);
}

void BoxComponent::HandleNodeCollision(StringHash eventType, VariantMap &eventData) {

    using namespace NodeCollision;

    onGround = false;

    auto contactNode = dynamic_cast<Node*>(eventData[P_OTHERNODE].GetPtr());

    if(contactNode) {
        auto terrain = contactNode->GetComponent<Terrain>();
        if(terrain) onGround = true;
    }

}