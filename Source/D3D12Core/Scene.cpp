#include "pch.h"

#include "AppMain.h"
#include "CommandList.h"
#include "GraphicsCore.h"
#include "GraphicsCommon.h"
#include "GameTime.h"
#include "Input.h"

#include "TextureLoader.h"

#include "PipelineState.h"
#include "GraphicsBuffer.h"
#include "Texture2D.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"

#include "Scene.h"

using namespace std;
using namespace Graphics;
using namespace Application;

namespace Game
{
    GameObject& Scene::AddGameObject(std::unique_ptr<GameObject>&& gameObj)
    {
        return AddGameObject(gameObj);
    }
    GameObject& Scene::AddGameObject(std::unique_ptr<GameObject>& gameObj)
    {
        auto result = gameObj.get();
        m_HierarchyObject.push_back(move(gameObj));
        return *result;
    }
    GameObject* Scene::FindGameObject(const std::string& name) const
    {
        for (auto& obj : m_HierarchyObject)
        {
            if (obj->GetTransform().GetParent() || !obj->GetActive()) continue;
            if (obj->m_Name == name)
                return obj.get();
        }
        return nullptr;
    }

    void Scene::ExecuteScene()
    {
        ExecuteUpdate();
        ExecuteRender();
    }
    void Scene::CloseScene()
    {
        m_HierarchyObject.clear();
    }

}