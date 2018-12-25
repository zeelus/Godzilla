//
// Created by Gilbert Gwizdala on 09/12/2018.
//

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/Image.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Skeleton.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/Input.h>


#include "GodzillaMainScene.hpp"

#include "CharacterComponent.hpp"

GodzillaMainScene::GodzillaMainScene(Context *context) : Application(context) {
    CharacterComponent::RegisterObject(context);
}

void GodzillaMainScene::Start() {
    Application::Start();

    this->Setup();

    this->CreateScene();

    this->SetupCharacter();

    this->SetupViewport();
}

void GodzillaMainScene::Setup() {

    this->engineParameters_["FullScreen"]      = false;
    this->engineParameters_["WindowWidth"]     = 1280;
    this->engineParameters_["WindowHeight"]    = 720;
    this->engineParameters_["WindowResizable"] = true;

    this->SubscribeToEvents();

}

void GodzillaMainScene::CreateScene() {
    auto* cache = GetSubsystem<ResourceCache>();

    this->scene_ = new Scene(this->context_);

    scene_->CreateComponent<DebugRenderer>();

    this->scene_->CreateComponent<Octree>();

    this->SetupCamera();

    auto* skyNode = this->scene_->CreateChild("Sky");
    skyNode->SetScale(1500.0f);
    auto* skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Data/Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Data/Materials/Skybox.xml"));

    auto* terrainNode = this->scene_->CreateChild("Terrain");

    auto* terrain = terrainNode->CreateComponent<Terrain>();
    terrain->SetPatchSize(128);
    terrain->SetSpacing(Vector3(2, 0.5, 2));
    terrain->SetSmoothing(true);
    terrain->SetHeightMap(cache->GetResource<Image>("Textures/HeightMap.png"));
    terrain->SetMaterial(cache->GetResource<Material>("Materials/Terrain.xml"));
    terrain->SetCastShadows(true);
    terrain->SetOccluder(true);

    auto* terrainBody = terrainNode->CreateComponent<RigidBody>();
    terrainBody->SetCollisionLayer(2);

    auto* terrainShape = terrainNode->CreateComponent<CollisionShape>();
    terrainShape->SetTerrain();

    auto* waterNode_ = scene_->CreateChild("Water");
    waterNode_->SetScale(Vector3(2048.0f, 1.0f, 2048.0f));
    waterNode_->SetPosition(Vector3(0.0f, 40.0f, 0.0f));
    auto* water = waterNode_->CreateComponent<StaticModel>();
    water->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    water->SetMaterial(cache->GetResource<Material>("Materials/Water.xml"));
    water->SetViewMask(0x80000000);

}

void GodzillaMainScene::SetupCamera() {

    auto* zoneNode = this->scene_->CreateChild("Zone");
    auto* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-50000.0f, 50000.0f));
    zone->SetFogStart(500.0f);
    zone->SetFogEnd(600.0f);
    zone->SetFogColor(Color(1, 1, 1));
    zone->SetAmbientColor(Color(0.5, 0.5, 0.5));

    cameraNode_ = scene_->CreateChild("Camera");
    camera_ = cameraNode_->CreateComponent<Camera>();
    camera_->SetFarClip(600);
    camera_->SetNearClip(0.1);
    camera_->SetFov(75);

    //camera_->SetFillMode(FillMode::FILL_WIREFRAME);

    //this->cameraNode_->SetPosition(Vector3(13, 30, 30));
    //this->cameraNode_->SetDirection(Vector3::FORWARD);
}

void GodzillaMainScene::SetupViewport()  {
    auto* cache = GetSubsystem<ResourceCache>();
    auto* renderer = GetSubsystem<Renderer>();

    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    auto* effectRenderPath = viewport->GetRenderPath();

    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA2.xml"));
}

void  GodzillaMainScene::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(GodzillaMainScene, HandleUpdate));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(GodzillaMainScene, HandlePostRenderUpdate));
}

void GodzillaMainScene::HandleUpdate(StringHash eventType, VariantMap& eventData) {

    auto* input = this->GetSubsystem<Input>();

    if (characterComponent) {

        characterComponent->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT, false);

        characterComponent->controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
        characterComponent->controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
        characterComponent->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
        characterComponent->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));


        characterComponent->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
        characterComponent->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
        characterComponent->controls_.pitch_ = Clamp(characterComponent->controls_.pitch_, -80.0f, 80.0f);
        characterNode->SetRotation(Quaternion(characterComponent->controls_.yaw_ , Vector3::UP));
    }


}

void GodzillaMainScene::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData) {
    scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);

    if(isDebug) {
        float timeStep = eventData[Update::P_TIMESTEP].GetFloat();
        // Movement speed as world units per second
        float MOVE_SPEED = 10.0f;
        // Mouse sensitivity as degrees per pixel
        const float MOUSE_SENSITIVITY = 0.1f;

        // camera movement
        Input *input = GetSubsystem<Input>();
        if (input->GetQualifierDown(Qualifier(1)))  // 1 is shift, 2 is ctrl, 4 is alt
            MOVE_SPEED *= 10;
        if (input->GetKeyDown(KEY_W))
            cameraNode_->Translate(Vector3(0, 0, 1) * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_S))
            cameraNode_->Translate(Vector3(0, 0, -1) * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_A))
            cameraNode_->Translate(Vector3(-1, 0, 0) * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_D))
            cameraNode_->Translate(Vector3(1, 0, 0) * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_ESCAPE))
            this->engine_->Exit();
        if (input->GetMouseButtonPress(MOUSEB_LEFT))
            this->CreateTestBox();

        if (!GetSubsystem<Input>()->IsMouseVisible()) {
            IntVector2 mouseMove = input->GetMouseMove();
            if (mouseMove.x_ > -2000000000 && mouseMove.y_ > -2000000000) {
                static float yaw_ = 0;
                static float pitch_ = 0;
                yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
                pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
                pitch_ = Clamp(pitch_, -90.0f, 90.0f);
                // Reset rotation and set yaw and pitch again
                this->cameraNode_->SetDirection(Vector3::FORWARD);
                this->cameraNode_->Yaw(yaw_);
                this->cameraNode_->Pitch(pitch_);
            }
        }
    } else {

        const Quaternion& rot = characterNode->GetRotation();
        Quaternion dir = rot * Quaternion(characterComponent->controls_.pitch_, Vector3::RIGHT);

        Vector3 aimPoint = characterNode->GetPosition() + rot * Vector3(0.0f, 80.0f, 0.0f);

        Vector3 rayDir = dir * Vector3::BACK;
        float rayDistance = CAMERA_INITIAL_DIST;
        PhysicsRaycastResult result;
        scene_->GetComponent<PhysicsWorld>()->RaycastSingle(result, Ray(aimPoint, rayDir), rayDistance, 2);
        if (result.body_)
            rayDistance = Min(rayDistance, result.distance_);
        rayDistance = Clamp(rayDistance, CAMERA_MIN_DIST, CAMERA_MAX_DIST);

        cameraNode_->SetPosition(aimPoint + rayDir * rayDistance);
        cameraNode_->SetRotation(dir);
    }
}

void GodzillaMainScene::SetupCharacter() {
    auto* cache = GetSubsystem<ResourceCache>();
    this->characterNode = this->scene_->CreateChild("CharacterNode");
    this->characterNode->SetScale(0.05);

    auto* modelNode = this->characterNode->CreateChild("CharacterModel");
    modelNode->SetRotation(Quaternion(180.0f, Vector3::UP));
    AnimatedModel* modelObject = modelNode->CreateComponent<AnimatedModel>();
    modelObject->SetModel(cache->GetResource<Model>("Models/Godzilla/Godzilla.mdl"));
    modelObject->SetMaterial(cache->GetResource<Material>("Models/Godzilla/Materials/Material.xml"));
    modelObject->SetCastShadows(true);
    modelNode->CreateComponent<AnimationController>();

    this->characterComponent = modelNode->CreateComponent<CharacterComponent>();

    auto* body = this->characterNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);
    body->SetMass(100.0f);
    body->SetAngularFactor(Vector3::ZERO);
    body->SetCollisionEventMode(COLLISION_ALWAYS);
    body->SetFriction(100.0f);

    auto* shape = this->characterNode->CreateComponent<CollisionShape>();
    shape->SetCapsule(800.0f, 1800.0f, Vector3(0.0f, 900.0f, -100.0f));

    auto skeleton = modelObject->GetSkeleton();
    this->CreateCollisionShapeForBone(skeleton, String("tail_3"), 30.0f, 30.0f);
    this->CreateCollisionShapeForBone(skeleton, String("tail_4"), 20.0f, 50.0f, Vector3(0.0f, 0.0f, 10.0f));
    this->CreateCollisionShapeForBone(skeleton, String("tail_6"), 20.0f, 30.0f, Vector3(0.0f, 0.0f, 5.0f));
    this->CreateCollisionShapeForBone(skeleton, String("tail_8"), 10.0f, 40.0f, Vector3(0.0f, 10.0f, 5.0f));

    characterComponent->SetAnimationState(GodzillaState::RUN);

}

void GodzillaMainScene::CreateTestBox() {
    auto* cache = GetSubsystem<ResourceCache>();

    // Create a smaller box at camera position
    Node* boxNode = scene_->CreateChild("SmallBox");
    boxNode->SetPosition(cameraNode_->GetPosition());
    boxNode->SetRotation(cameraNode_->GetRotation());
    boxNode->SetScale(2.5f);
    auto* boxObject = boxNode->CreateComponent<StaticModel>();
    boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    boxObject->SetMaterial(cache->GetResource<Material>("Materials/StoneSmall.xml"));
    boxObject->SetCastShadows(true);

    // Create physics components, use a smaller mass also
    auto* body = boxNode->CreateComponent<RigidBody>();
    body->SetMass(2.50f);
    body->SetFriction(0.75f);
    auto* shape = boxNode->CreateComponent<CollisionShape>();
    shape->SetBox(Vector3::ONE);

    const float OBJECT_VELOCITY = 30.0f;

    body->SetLinearVelocity(cameraNode_->GetRotation() * Vector3(0.0f, 0.25f, 1.0f) * OBJECT_VELOCITY);
}

void GodzillaMainScene::CreateCollisionShapeForBone(Skeleton& skeleton, String name, float width, float height,
                                                    Vector3 position) {
    auto boneNode = skeleton.GetBone(name)->node_;

    if (!boneNode) {
        return;
    }

    auto* body = boneNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);
    body->SetMass(2.0f);
    body->SetAngularFactor(Vector3::ZERO);
    body->SetCollisionEventMode(COLLISION_ALWAYS);
    body->SetFriction(100.0f);

    auto* shape = boneNode->CreateComponent<CollisionShape>();
    shape->SetCapsule(width, height, position);

}