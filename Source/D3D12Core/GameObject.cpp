﻿#include "pch.h"

#include "Transform.h"

#include "GameObject.h"


namespace Game
{
    GameObject::GameObject() : GameObject(_T("New GameObject"), nullptr, false) {}
    GameObject::GameObject(const std::wstring& name, Transform* parent, bool worldPositionStays)
    {
        m_Name = std::wstring(name);
        m_Transform.reset(new Transform(*this));
        if (parent)
            m_Transform->SetParent(parent, worldPositionStays);
    }

    bool GameObject::GetActive(bool isInHierarchy) const
    {
        if (!isInHierarchy) return m_Enabled;
        if (m_Enabled && (m_Transform->GetParent() != nullptr))
            return m_Transform->GetParent()->GetGameObject().GetActive();
        return m_Enabled;
    }
}