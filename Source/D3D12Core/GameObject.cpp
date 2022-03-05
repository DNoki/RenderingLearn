#include "pch.h"

#include "Transform.h"

#include "GameObject.h"


namespace Game
{
    GameObject::GameObject() : GameObject(_T("New GameObject"), nullptr, false) {}
    GameObject::GameObject(const std::wstring& name, Transform* parent, bool worldPositionStays) : m_Components()
    {
        m_Name = std::wstring(name);
        m_Enabled = true;
        m_Transform.reset(new Transform(*this));
    }

    bool GameObject::GetActive(bool isInHierarchy) const
    {
        if (!isInHierarchy) return m_Enabled;
        if (m_Enabled && (m_Transform->GetParent() != nullptr))
            return m_Transform->GetParent()->GetGameObject().GetActive();
        return m_Enabled;
    }
}