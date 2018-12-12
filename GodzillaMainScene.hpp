//
// Created by Gilbert Gwizdala on 09/12/2018.
//

#ifndef GODZILLA_GODZILLAMAINSCENE_HPP
#define GODZILLA_GODZILLAMAINSCENE_HPP

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Camera.h>


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

    void Setup();

    void CreateScene();

    void SetupViewport();

    void SetupCamera();

    void SubscribeToEvents();

    void SetupCharacter();

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
};


#endif //GODZILLA_GODZILLAMAINSCENE_HPP
