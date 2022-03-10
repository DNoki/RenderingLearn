#pragma once

#include "Scene.h"

namespace Game
{
    class SampleScene : public Scene
    {
    public:
        SampleScene() = default;

        virtual void Initialize() override;

    private:

        virtual void ExecuteUpdate() override;

    };

}