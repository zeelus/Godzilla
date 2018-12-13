//
// Created by Gilbert Gwizdala on 12/12/2018.
//

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/AnimationController.h>

#include "CharacterComponent.hpp"

using namespace Urho3D;

void CharacterComponent::RegisterObject(Context *context) {

    context->RegisterFactory<CharacterComponent>();

}

void CharacterComponent::SetAnimationState(GodzillaState state) {

    auto* animationController = this->GetComponent<AnimationController>();
    switch(state) {
        case GodzillaState::IDLE :
            animationController->Play("Models/Godzilla/Animations/GodzillaIdle.ani", 0, true);
            break;
        case GodzillaState::RUN :
            animationController->Play("Models/Godzilla/Animations/GodzillaRun.ani", 0, true);
            break;
    }
}
