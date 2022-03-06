#include "pch.h"

#include "GameObject.h"

#include "Component.h"


namespace Game
{
    Component::Component(GameObject& obj) :m_GameObject(obj), m_Transform(obj.GetTransform())
    {
    }
}