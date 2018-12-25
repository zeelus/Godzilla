//
// Created by Gilbert Gwizdala on 12/12/2018.
//

#ifndef GODZILLA_CHARACTERCOMPONENT_HPP
#define GODZILLA_CHARACTERCOMPONENT_HPP

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Input/Controls.h>

using namespace Urho3D;

enum class GodzillaState {
    IDLE,
    RUN
};

class CharacterComponent: public LogicComponent {

public:

    static void RegisterObject(Context* context);

    CharacterComponent(Context* context): LogicComponent(context) {};

    void SetAnimationState(GodzillaState state);

    Controls controls_;
};


#endif //GODZILLA_CHARACTERCOMPONENT_HPP
