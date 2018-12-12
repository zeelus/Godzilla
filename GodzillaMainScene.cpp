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

    this->cameraNode_->SetPosition(Vector3(13, 30, 30));
    this->cameraNode_->SetDirection(Vector3::FORWARD);
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
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();
    // Movement speed as world units per second
    float MOVE_SPEED=10.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    // camera movement
    Input* input = GetSubsystem<Input>();
    if(input->GetQualifierDown(Qualifier(1)))  // 1 is shift, 2 is ctrl, 4 is alt
        MOVE_SPEED*=10;
    if(input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3(0, 0, 1)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3(0, 0, -1)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3(-1, 0, 0)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3( 1, 0, 0)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown(KEY_ESCAPE))
        this->engine_->Exit();

    if(!GetSubsystem<Input>()->IsMouseVisible())
    {
        IntVector2 mouseMove=input->GetMouseMove();
        if( mouseMove.x_ > -2000000000 && mouseMove.y_ > -2000000000 )
        {
            static float yaw_=0;
            static float pitch_=0;
            yaw_ += MOUSE_SENSITIVITY*mouseMove.x_;
            pitch_ += MOUSE_SENSITIVITY*mouseMove.y_;
            pitch_ = Clamp(pitch_, -90.0f, 90.0f);
            // Reset rotation and set yaw and pitch again
            this->cameraNode_->SetDirection(Vector3::FORWARD);
            this->cameraNode_->Yaw(yaw_);
            this->cameraNode_->Pitch(pitch_);
        }
    }
}

void GodzillaMainScene::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData) {
    //scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
}

void GodzillaMainScene::SetupCharacter() {
    auto* cache = GetSubsystem<ResourceCache>();
    this->characterNode = this->scene_->CreateChild("CharacterNode");
    this->characterNode->SetScale(0.05);

    AnimatedModel* modelObject = this->characterNode->CreateComponent<AnimatedModel>();
    modelObject->SetModel(cache->GetResource<Model>("Models/Godzilla/Godzilla.mdl"));
    modelObject->SetMaterial(cache->GetResource<Material>("Models/Godzilla/Materials/Material.xml"));
    modelObject->SetCastShadows(true);
    this->characterNode->CreateComponent<AnimationController>();

    auto* characterComponent = this->characterNode->CreateComponent<CharacterComponent>();

    characterComponent->SetAnimationState(GodzillaState::IDLE);

}
