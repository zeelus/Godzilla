//
// Created by Gilbert Gwizdala on 2018-12-27.
//

#include "BuldingComponent.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/IO/MemoryBuffer.h>

#include "CharacterComponent.hpp"


using namespace Urho3D;

BuldingComponent::BuldingComponent(Context *context) : LogicComponent(context) {

}

void BuldingComponent::RegisterObject(Context *context) {

    context->RegisterFactory<BuldingComponent>();

}

void BuldingComponent::CreateBuldingsBlocks(short levels) {
    for(int i = 0; i < levels; i++) {
        auto* boxC = this->CreateNode(Vector3(0.0f, (size * i) + (size / 2.0f), 0.0f));
        auto* box1 = this->CreateNode(Vector3(size, (size * i) + (size / 2.0f), size));
        auto* box2 = this->CreateNode(Vector3(size, (size * i) + (size / 2.0f), 0.0f));
        auto* box3 = this->CreateNode(Vector3(0.0f, (size * i) + (size / 2.0f), size));
        auto* box4 = this->CreateNode(Vector3(-size, (size * i) + (size / 2.0f), -size));
        auto* box5 = this->CreateNode(Vector3(-size, (size * i) + (size / 2.0f), 0.0f));
        auto* box6 = this->CreateNode(Vector3(0.0f, (size * i) + (size / 2.0f), -size));
        auto* box7 = this->CreateNode(Vector3(size, (size * i) + (size / 2.0f), -size));
        auto* box8 = this->CreateNode(Vector3(-size, (size * i) + (size / 2.0f), size));

        elements.push_back(boxC);
        elements.push_back(box1);
        elements.push_back(box2);
        elements.push_back(box3);
        elements.push_back(box4);
        elements.push_back(box5);
        elements.push_back(box6);
        elements.push_back(box7);
        elements.push_back(box8);
    }


}

Node *BuldingComponent::CreateNode(const Vector3 &position) {
    auto* cache = GetSubsystem<ResourceCache>();

    Node* boxNode = node_->CreateChild("SmallBox");
    boxNode->SetPosition(position);
    boxNode->SetScale(size);
    auto* boxObject = boxNode->CreateComponent<StaticModel>();
    boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    boxObject->SetMaterial(cache->GetResource<Material>("Materials/StoneSmall.xml"));
    boxObject->SetCastShadows(true);

    return boxNode;
}

void BuldingComponent::Start() {
    LogicComponent::Start();

    SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(BuldingComponent, HandleNodeCollision));
}

void BuldingComponent::HandleNodeCollision(StringHash eventType, VariantMap &eventData) {

    using namespace NodeCollision;

    Node* cNode = reinterpret_cast<Node*>(eventData[P_OTHERNODE].GetPtr());

    auto* characterComponent = cNode->GetComponent<CharacterComponent>();

    if(characterComponent != nullptr) {
        this->DestridBulding();
    }
}

float BuldingComponent::getSize() const {
    return size;
}

void BuldingComponent::DestridBulding() {

    isDestroid = true;

    this->GetComponent<CollisionShape>()->Remove();
    this->GetComponent<RigidBody>()->Remove();

    for(auto* element : elements) {
        auto* body = element->CreateComponent<RigidBody>();
        body->SetMass(0.50f);
        body->SetFriction(0.75f);
        auto* shape = element->CreateComponent<CollisionShape>();
        shape->SetBox(Vector3::ONE);
    }
}

bool BuldingComponent::IsDestroid() const {
    return isDestroid;
}



