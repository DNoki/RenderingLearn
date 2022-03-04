#include "pch.h"

#include "GameObject.h"

#include "Component.h"


namespace Game
{
    Component::Component(GameObject& obj) :m_GameObject(obj), m_Transform(obj.GetTransform())
    {
    }
    Component::Component(GameObject& obj, Transform& trans) : m_GameObject(obj), m_Transform(trans)
    {
    }
}