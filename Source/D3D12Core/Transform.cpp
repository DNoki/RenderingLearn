#include "pch.h"

#include "GameObject.h"

#include "Transform.h"


namespace Game
{
    Transform::Transform(GameObject& obj) : Component(obj)
    {
        m_TransformBuffer.reset(new ConstansBuffer<TransformBuffer>());
        m_TransformBuffer->PlacedCreate();
        m_TransformBuffer->SetName(Application::Format(L"%s (TransformBuffer)", obj.m_Name));
    };

    Vector3 Transform::GetPosition(bool isWorld) const
    {
        if (m_Parent == nullptr || !isWorld)
            return m_LocalPosition;
        else
            return m_Parent->GetLocalToWorldMatrix() * Vector4(m_LocalPosition, 1.0f);
    }
    void Transform::SetPosition(const Vector3& pos, bool isWorld)
    {
        if (m_Parent == nullptr || !isWorld)
            m_LocalPosition = pos;
        else
            m_LocalPosition = m_Parent->GetLocalToWorldMatrix().Inverse() * Vector4(pos, 1.0f);
    }
    Quaternion Transform::GetRotation(bool isWorld) const
    {
        if (m_Parent == nullptr || !isWorld)
            return m_LocalRotation;
        else
            return m_Parent->GetRotation() * m_LocalRotation;
    }
    void Transform::SetRotation(const Quaternion& rot, bool isWorld)
    {
        if (m_Parent == nullptr || !isWorld)
            m_LocalRotation = rot;
        else
            m_LocalRotation = m_Parent->GetRotation().Inverse() * rot;
    }
    Vector3 Transform::GetEulerAngles(bool isWorld) const
    {
        return GetRotation(isWorld).GetEulerAngles();
    }
    void Transform::SetEulerAngles(const Vector3& e, bool isWorld)
    {
        SetRotation(Quaternion::CreateFromEulerAngles(e), isWorld);
    }
    Vector3 Transform::GetLossyScale() const
    {
        if (m_Parent == nullptr)
            return m_LocalScale;
        else
            return m_Parent->GetLossyScale() * m_LocalScale;
    }
    void Transform::SetLossyScale(const Vector3& s)
    {
        if (m_Parent == nullptr)
            m_LocalScale = s;
        else
            m_LocalScale = s / m_Parent->GetLossyScale();
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