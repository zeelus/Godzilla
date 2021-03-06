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
#include <Urho3D/Audio/SoundSource.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <sstream>


#include "GodzillaMainScene.hpp"
#include "CharacterComponent.hpp"
#include "BuldingComponent.hpp"
#include "BoxComponent.hpp"
#include "BallComponent.h"

GodzillaMainScene::GodzillaMainScene(Context *context) : Application(context) {
    CharacterComponent::RegisterObject(context);
    BuldingComponent::RegisterObject(context);
    BoxComponent::RegisterObject(context);
    BallComponent::RegisterObject(context);
}

void GodzillaMainScene::Start() {
    Application::Start();

    this->Setup();

    if(isMenu) {
        this->CreateMenuScene();
    } else {
        this->CreateGameScene();
    }


    this->SetupViewport();

}

void GodzillaMainScene::Setup() {

    this->engineParameters_["FullScreen"]      = false;
    this->engineParameters_["WindowWidth"]     = 1280;
    this->engineParameters_["WindowHeight"]    = 720;
    this->engineParameters_["WindowResizable"] = true;

    this->SubscribeToEvents();

}

void GodzillaMainScene::CreateMenuScene() {
    auto* cache = GetSubsystem<ResourceCache>();

    this->scene_ = new Scene(this->context_);
    this->scene_->CreateComponent<Octree>();

    this->SetupCamera();

    auto* skyNode = this->scene_->CreateChild("Sky");
    skyNode->SetScale(15000.0f);
    auto* skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Data/Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Data/Materials/Skybox.xml"));

    auto* terrainNode = this->scene_->CreateChild("Terrain");

    auto* terrain = terrainNode->CreateComponent<Terrain>();
    terrain->SetPatchSize(128);
    terrain->SetSpacing(Vector3(4, 0.6, 4));
    terrain->SetSmoothing(true);
    terrain->SetHeightMap(cache->GetResource<Image>("Textures/HeightMap.png"));
    terrain->SetMaterial(cache->GetResource<Material>("Materials/Terrain.xml"));
    terrain->SetCastShadows(true);
    terrain->SetOccluder(true);

    auto* waterNode_ = scene_->CreateChild("Water");
    waterNode_->SetScale(Vector3(2000.0f, 1.0f, 2000.0f));
    waterNode_->SetPosition(Vector3(350.0f, 45.0f, 350.0f));
    auto* water = waterNode_->CreateComponent<StaticModel>();
    water->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    water->SetMaterial(cache->GetResource<Material>("Materials/Water.xml"));
    water->SetViewMask(0x80000000);

    auto* soundNode = this->scene_->CreateChild("SoundNode");

    auto* soundSorce = soundNode->CreateComponent<SoundSource>();

    auto* song = cache->GetResource<Sound>("Sounds/Handpan.ogg");

    soundSorce->Play(song);

    showMenuUI();
}

void GodzillaMainScene::CreateGameScene() {

    auto* cache = GetSubsystem<ResourceCache>();

    this->scene_ = new Scene(this->context_);

    scene_->CreateComponent<DebugRenderer>();

    this->scene_->CreateComponent<Octree>();
    auto* ui = GetSubsystem<UI>();
    ui->Clear();

    this->SetupCamera();

    this->SetSoundTrack();

    auto* skyNode = this->scene_->CreateChild("Sky");
    skyNode->SetScale(15000.0f);
    auto* skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Data/Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Data/Materials/Skybox.xml"));

    auto* terrainNode = this->scene_->CreateChild("Terrain");

    auto* terrain = terrainNode->CreateComponent<Terrain>();
    terrain->SetPatchSize(128);
    terrain->SetSpacing(Vector3(4, 0.6, 4));
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
    waterNode_->SetScale(Vector3(2000.0f, 1.0f, 2000.0f));
    waterNode_->SetPosition(Vector3(350.0f, 45.0f, 350.0f));
    auto* water = waterNode_->CreateComponent<StaticModel>();
    water->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    water->SetMaterial(cache->GetResource<Material>("Materials/Water.xml"));
    water->SetViewMask(0x80000000);

    auto startPoint = Vector3(350, 0, 350);
    int buldingsOffser = 50;

    for(int i = 0; i < 5; i++ ) {
        for(int j = 0; j < 4; j++) {
            int height = Random(3, 8);
            auto buildingPos = Vector3(startPoint.x_ + buldingsOffser * j, 0, startPoint.z_ + buldingsOffser * i);
            auto buildingComoponent = this->CreateBuilding(buildingPos, height, terrain);
            this->buldingVector.push_back(buildingComoponent);
        }
    }


    timeText = ui->GetRoot()->CreateChild<Text>();
    timeText->SetText("You time: 0:00:00");
    timeText->SetFont(cache->GetResource<Font>("Fonts/PermanentMarker-Regular.ttf"), 20);
    timeText->SetColor(Color(1.0f, 0.0f, 0.0f));

    timeText->SetHorizontalAlignment(HA_LEFT);
    timeText->SetVerticalAlignment(VA_TOP);

    timer.Reset();

    isGameEnd = false;

    this->SetupCharacter();

}

void GodzillaMainScene::SetupCamera() {

    auto* zoneNode = this->scene_->CreateChild("Zone");
    auto* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-50000.0f, 50000.0f));
    zone->SetFogStart(1000.0f);
    zone->SetFogEnd(3000.0f);
    zone->SetFogColor(Color(1, 1, 1));
    zone->SetAmbientColor(Color(0.5, 0.5, 0.5));

    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->SetPosition(Vector3(0.0f, 46.0f, 0.0f));
    camera_ = cameraNode_->CreateComponent<Camera>();
    camera_->SetFarClip(1600);
    camera_->SetNearClip(0.1);
    camera_->SetFov(75);

    SoundListener* listener = cameraNode_->CreateComponent<SoundListener>();
    GetSubsystem<Audio>()->SetListener(listener);

    //camera_->SetFillMode(FillMode::FILL_WIREFRAME);

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

    this->TestEndGame();

    auto* input = this->GetSubsystem<Input>();

    if (input->GetKeyDown(KEY_ESCAPE))
        this->engine_->Exit();

    if (input->GetKeyDown(KEY_R) && isGameEnd)
        this->restartGame();

    if (input->GetKeyDown(KEY_SPACE) && isMenu) {
        auto* ui = GetSubsystem<UI>();
        ui->GetRoot()->RemoveAllChildren();
        isMenu = false;
        this->restartGame();
    }

    if(isGameEnd)
        return;

    if (characterComponent && characterNode) {

        characterComponent->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT, false);

        characterComponent->controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
        characterComponent->controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
        characterComponent->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
        characterComponent->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
        characterComponent->controls_.Set(CTRL_LEFT_MOUSE, input->GetMouseButtonDown(MOUSEB_LEFT));

        characterComponent->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
        characterComponent->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
        characterComponent->controls_.pitch_ = Clamp(characterComponent->controls_.pitch_, -80.0f, 80.0f);
        characterNode->SetRotation(Quaternion(characterComponent->controls_.yaw_ , Vector3::UP));

        SetUITimer();
    }


}

void GodzillaMainScene::SetUITimer() {
    auto time = this->timer.GetMSec(false);
    unsigned int sec = time / 1000;
    unsigned int mili = time - (sec * 1000);
    unsigned int min = sec / 60;
    sec = sec - (min * 60);
    stringstream s;
    s << "You time: " << min << ":" << sec << ":" << mili;
    string ss = s.str();
    if(timeText)
        timeText->SetText(ss.c_str());
}

void GodzillaMainScene::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData) {

    if(isGameEnd)
        return;

    //scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);

    if(isDebug) {
        float timeStep = eventData[Update::P_TIMESTEP].GetFloat();
        float MOVE_SPEED = 10.0f;
        const float MOUSE_SENSITIVITY = 0.1f;

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
                this->cameraNode_->SetDirection(Vector3::FORWARD);
                this->cameraNode_->Yaw(yaw_);
                this->cameraNode_->Pitch(pitch_);
            }
        }
    } else {

        if(!characterComponent && !characterNode) return;

        const Quaternion& rot = characterNode->GetRotation();
        Quaternion dir = rot * Quaternion(characterComponent->controls_.pitch_, Vector3::RIGHT);

        Vector3 aimPoint = characterNode->GetPosition() + rot * Vector3(0.0f, 100.0f, -60.0f);

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
    this->characterNode->SetScale(0.03);
    this->characterNode->SetPosition(Vector3(0.0f, 10.0f, 0.0f));

    auto* modelNode = this->characterNode->CreateChild("CharacterModel");
    modelNode->SetRotation(Quaternion(180.0f, Vector3::UP));
    AnimatedModel* modelObject = modelNode->CreateComponent<AnimatedModel>();
    modelObject->SetModel(cache->GetResource<Model>("Models/Godzilla/Godzilla.mdl"));
    modelObject->SetMaterial(cache->GetResource<Material>("Models/Godzilla/Materials/Material.xml"));
    modelObject->SetCastShadows(true);
    modelNode->CreateComponent<AnimationController>();

    auto* body = this->characterNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);
    body->SetMass(100.0f);
    body->SetAngularFactor(Vector3::ZERO);
    body->SetCollisionEventMode(COLLISION_ALWAYS);

    auto* shape = this->characterNode->CreateComponent<CollisionShape>();
    shape->SetCapsule(800.0f, 1800.0f, Vector3(0.0f, 900.0f, -100.0f));

    auto skeleton = modelObject->GetSkeleton();
    this->CreateCollisionShapeForBone(skeleton, String("tail_3"), 30.0f, 30.0f);
    this->CreateCollisionShapeForBone(skeleton, String("tail_4"), 20.0f, 50.0f, Vector3(0.0f, 0.0f, 10.0f));
    this->CreateCollisionShapeForBone(skeleton, String("tail_6"), 20.0f, 30.0f, Vector3(0.0f, 0.0f, 5.0f));
    this->CreateCollisionShapeForBone(skeleton, String("tail_8"), 10.0f, 40.0f, Vector3(0.0f, 10.0f, 5.0f));

    characterComponent = this->characterNode->CreateComponent<CharacterComponent>();
    characterComponent->SetAnimationState(GodzillaState::IDLE);
    characterComponent->SetScene(this->scene_);

    this->characterNode->CreateComponent<SoundSource>();

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

    auto* shape = boneNode->CreateComponent<CollisionShape>();
    shape->SetCapsule(width, height, position);

}

BuldingComponent* GodzillaMainScene::CreateBuilding(Vector3 position, short levels, Terrain *terrain) {

    Node* buldingNode = scene_->CreateChild("Bulding");

    auto* buldingComponet = buldingNode->CreateComponent<BuldingComponent>();

    auto height = terrain->GetHeight(position);

    buldingNode->SetPosition(Vector3(position.x_, height + 1.0f, position.z_));

    buldingComponet->CreateBuldingsBlocks(levels);

    auto* body = buldingNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);
    body->SetMass(0.0f);
    body->SetAngularFactor(Vector3::ZERO);

    auto* shape = buldingNode->CreateComponent<CollisionShape>();
    float size = buldingComponet->getSize();
    shape->SetBox(Vector3(3 * size, levels * size, 3 * size));
    shape->SetPosition(Vector3(0.0f, (levels * size) / 2.0f, 0.0f));


    return buldingComponet;
}

void GodzillaMainScene::SetSoundTrack() {

    auto* cache = GetSubsystem<ResourceCache>();

    auto* soundNode = this->scene_->CreateChild("SoundNode");

    auto* soundSorce = soundNode->CreateComponent<SoundSource>();

    auto* song = cache->GetResource<Sound>("Sounds/Seekdestroy.ogg");

    soundSorce->Play(song);

}

void GodzillaMainScene::TestEndGame() {

    for(auto bulidngComponent: this->buldingVector) {
        if(!bulidngComponent->isDestroid) {
            return;
        }
    }

    this->ENDGAME();
}

void GodzillaMainScene::ENDGAME() {
    if(isGameEnd || isMenu) return;
    isGameEnd = true;

    this->scene_->SetUpdateEnabled(false);
    this->showEndText();

}

void GodzillaMainScene::showEndText() {

    auto* cache = GetSubsystem<ResourceCache>();
    auto* ui = GetSubsystem<UI>();

    // Construct new Text object, set string to display and font to use
    auto* instructionText = ui->GetRoot()->CreateChild<Text>();
    instructionText->SetText("Game Over");
    instructionText->SetFont(cache->GetResource<Font>("Fonts/PermanentMarker-Regular.ttf"), 35);
    instructionText->SetColor(Color(1.0f, 0.0f, 0.0f));

    // Position the text relative to the screen center
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);

    auto* instructionText2 = ui->GetRoot()->CreateChild<Text>();
    String textCopy = this->timeText->GetText();
    instructionText2->SetText(textCopy);
    instructionText2->SetFont(cache->GetResource<Font>("Fonts/PermanentMarker-Regular.ttf"), 25);
    instructionText2->SetColor(Color(1.0f, 0.0f, 0.0f));

    instructionText2->SetHorizontalAlignment(HA_CENTER);
    instructionText2->SetVerticalAlignment(VA_CENTER);
    instructionText2->SetPosition(0, ui->GetRoot()->GetHeight() / 6);

    auto* instructionText3 = ui->GetRoot()->CreateChild<Text>();
    instructionText3->SetText("Press r to restart, ESC to exit.");
    instructionText3->SetFont(cache->GetResource<Font>("Fonts/PermanentMarker-Regular.ttf"), 20);
    instructionText3->SetColor(Color(1.0f, 0.0f, 0.0f));

    instructionText3->SetHorizontalAlignment(HA_CENTER);
    instructionText3->SetVerticalAlignment(VA_CENTER);
    instructionText3->SetPosition(0, ui->GetRoot()->GetHeight() / 3);
}

void GodzillaMainScene::restartGame() {

    if(characterNode)
        characterNode->Remove();
    scene_->Remove();
    if(timeText)
        timeText->Remove();
    timeText = nullptr;
    Start();

}

void GodzillaMainScene::showMenuUI() {
    auto* cache = GetSubsystem<ResourceCache>();
    auto* ui = GetSubsystem<UI>();

    auto* instructionText = ui->GetRoot()->CreateChild<Text>();
    instructionText->SetText("Godzilla - City Destroyer");
    instructionText->SetFont(cache->GetResource<Font>("Fonts/PermanentMarker-Regular.ttf"), 35);
    instructionText->SetColor(Color(44 / 255.0f, 140 / 255.0f, 54 / 255.0f));

    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);

    auto* instructionText2 = ui->GetRoot()->CreateChild<Text>();
    instructionText2->SetText("Press SPACE key to play.");
    instructionText2->SetFont(cache->GetResource<Font>("Fonts/PermanentMarker-Regular.ttf"), 20);
    instructionText2->SetColor(Color(44 / 255.0f, 140 / 255.0f, 54 / 255.0f));

    instructionText2->SetHorizontalAlignment(HA_CENTER);
    instructionText2->SetVerticalAlignment(VA_CENTER);
    instructionText2->SetPosition(0, ui->GetRoot()->GetHeight() / 3);
}


