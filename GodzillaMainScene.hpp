//
// Created by Gilbert Gwizdala on 09/12/2018.
//

#ifndef GODZILLA_GODZILLAMAINSCENE_HPP
#define GODZILLA_GODZILLAMAINSCENE_HPP

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Skeleton.h>
#include "CharacterComponent.hpp"

const float CAMERA_MIN_DIST = 20.0f;
const float CAMERA_INITIAL_DIST = 50.0f;
const float CAMERA_MAX_DIST = 100.0f;
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
    CharacterComponent * characterComponent;

    bool isDebug = false;

    void Setup();

    void CreateScene();

    void SetupViewport();

    void SetupCamera();

    void SubscribeToEvents();

    void SetupCharacter();

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    void CreateTestBox();

    void CreateCollisionShapeForBone(Skeleton& skeleton, String name, float width, float height, Vector3 position = Vector3::ZERO);
};


#endif //GODZILLA_GODZILLAMAINSCENE_HPP
