//
// Created by Gilbert Gwizdala on 09/12/2018.
//

#ifndef GODZILLA_GODZILLAMAINSCENE_HPP
#define GODZILLA_GODZILLAMAINSCENE_HPP

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Skeleton.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/UI/Text.h>
#include "CharacterComponent.hpp"
#include <vector>

#include "BuldingComponent.hpp"

const float CAMERA_MIN_DIST = 20.0f;
const float CAMERA_INITIAL_DIST = 50.0f;
const float CAMERA_MAX_DIST = 1000.0f;
const float YAW_SENSITIVITY = 0.1f;

using namespace Urho3D;

class GodzillaMainScene: public Application {

public:

    GodzillaMainScene(Context * context);

    virtual void Start();

private:

    SharedPtr<Scene> scene_;
    Camera* camera_;
    Node* cameraNode_;
    Node* characterNode;
    Text* timeText;
    CharacterComponent * characterComponent;
    Timer timer;
    bool isMenu = true;

    std::vector<BuldingComponent*> buldingVector;

    bool isGameEnd = false;

    bool isDebug = false;

    void Setup();

    void CreateGameScene();

    void CreateMenuScene();

    void SetupViewport();

    void SetupCamera();

    void SubscribeToEvents();

    void SetupCharacter();

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    void CreateTestBox();

    void CreateCollisionShapeForBone(Skeleton& skeleton, String name, float width, float height, Vector3 position = Vector3::ZERO);

    BuldingComponent* CreateBuilding(Vector3 position, short levels, Terrain* terrain);

    void SetSoundTrack();

    void TestEndGame();

    void ENDGAME();

    void showEndText();

    void restartGame();

    void SetUITimer();

    void showMenuUI();
};


#endif //GODZILLA_GODZILLAMAINSCENE_HPP
