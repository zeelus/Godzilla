//
// Created by Gilbert Gwizdala on 2018-12-27.
//

#ifndef GODZILLA_BULDINGCOMPONENT_HPP
#define GODZILLA_BULDINGCOMPONENT_HPP

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <vector>


using namespace Urho3D;
using namespace std;


class BuldingComponent: public LogicComponent {

    URHO3D_OBJECT(BuldingComponent, LogicComponent)


    Node* CreateNode(const Vector3 &position);

    vector<Node*> elements;

    float size = 8.0f;

    float getSize() const;

    void DestridBulding();

    bool isDestroid = false;

    bool IsDestroid() const;

public:
    static void RegisterObject(Context* context);

    explicit BuldingComponent(Context * context);

    void CreateBuldingsBlocks(short levels);

    void Start() override;

    void HandleNodeCollision(StringHash eventType, VariantMap &eventData);
};


#endif //GODZILLA_BULDINGCOMPONENT_HPP
