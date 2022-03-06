﻿#include "pch.h"


#include "Transform.h"


namespace Game
{
    Vector3 Transform::GetPosition(bool isWorld) const
    {
        if (m_Parent == nullptr || !isWorld)
            return LocalPosition;
        else
            return m_Parent->GetLocalToWorldMatrix() * Vector4(LocalPosition, 1.0f);
    }
    void Transform::SetPosition(const Vector3& pos, bool isWorld)
    {
        if (m_Parent == nullptr || !isWorld)
            LocalPosition = pos;
        else
            LocalPosition = m_Parent->GetLocalToWorldMatrix().Inverse() * Vector4(pos, 1.0f);
    }
    Quaternion Transform::GetRotation(bool isWorld) const
    {
        return Quaternion::CreateFromEulerAngles(GetEulerAngles(isWorld));
    }
    void Transform::SetRotation(const Quaternion& rot, bool isWorld)
    {
        SetEulerAngles(rot.GetEulerAngles(), isWorld);
    }
    Vector3 Transform::GetEulerAngles(bool isWorld) const
    {
        if (m_Parent == nullptr || !isWorld)
            return LocalEulerAngles;
        else
            return (m_Parent->GetRotation() * Quaternion(LocalEulerAngles)).GetEulerAngles();
    }
    void Transform::SetEulerAngles(const Vector3& e, bool isWorld)
    {
        auto eee = e * Math::Rad2Deg;
        if (m_Parent == nullptr || !isWorld)
            LocalEulerAngles = e;
        else
            LocalEulerAngles = (m_Parent->GetRotation().Inverse() * Quaternion(e)).GetEulerAngles();
    }
    Vector3 Transform::GetLossyScale() const
    {
        if (m_Parent == nullptr)
            return LocalScale;
        else
            return m_Parent->GetLossyScale() * LocalScale;
    }
    void Transform::SetLossyScale(const Vector3& s)
    {
        if (m_Parent == nullptr)
            LocalScale = s;
        else
            LocalScale = s / m_Parent->GetLossyScale();
    }

    void Transform::SetParent(Transform* parent, bool worldPositionStays)
    {
        if (!parent || m_Parent == parent) return;
        if (m_Parent)
        {
            auto it = std::find(m_Parent->m_Childs.begin(), m_Parent->m_Childs.end(), this);
            if (it != m_Parent->m_Childs.end())
                m_Parent->m_Childs.erase(it);
        }
        parent->m_Childs.push_back(this);

        if (!worldPositionStays)
            m_Parent = parent;
        else
        {
            auto pos = GetPosition();
            auto rotation = GetRotation();
            auto s = GetLossyScale();

            m_Parent = parent;
            SetPosition(pos);
            SetRotation(rotation);
            SetLossyScale(s);
        }
    }
}