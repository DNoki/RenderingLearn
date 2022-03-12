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
        m_TransformNode.m_Data = this;
    };

    Vector3 Transform::GetPosition(bool isWorld) const
    {
        auto* parent = GetParent();
        if (parent == nullptr || !isWorld)
            return m_LocalPosition;
        else
            return parent->GetLocalToWorldMatrix() * Vector4(m_LocalPosition, 1.0f);
    }
    void Transform::SetPosition(const Vector3& pos, bool isWorld)
    {
        auto* parent = GetParent();
        if (parent == nullptr || !isWorld)
            m_LocalPosition = pos;
        else
            m_LocalPosition = parent->GetLocalToWorldMatrix().Inverse() * Vector4(pos, 1.0f);
    }
    Quaternion Transform::GetRotation(bool isWorld) const
    {
        auto* parent = GetParent();
        if (parent == nullptr || !isWorld)
            return m_LocalRotation;
        else
            return parent->GetRotation() * m_LocalRotation;
    }
    void Transform::SetRotation(const Quaternion& rot, bool isWorld)
    {
        auto* parent = GetParent();
        if (parent == nullptr || !isWorld)
            m_LocalRotation = rot;
        else
            m_LocalRotation = parent->GetRotation().Inverse() * rot;
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
        auto* parent = GetParent();
        if (parent == nullptr)
            return m_LocalScale;
        else
            return parent->GetLossyScale() * m_LocalScale;
    }
    void Transform::SetLossyScale(const Vector3& s)
    {
        auto* parent = GetParent();
        if (parent == nullptr)
            m_LocalScale = s;
        else
            m_LocalScale = s / parent->GetLossyScale();
    }

    void Transform::SetParent(Transform* parent, bool worldPositionStays)
    {
        if (m_TransformNode.m_Parent)
        {
            // 父对象已存在
            m_TransformNode.m_Parent->RemoveChild(&m_TransformNode);
        }
        if (!worldPositionStays)
            parent->m_TransformNode.AddChild(&m_TransformNode);
        else
        {
            auto pos = GetPosition();
            auto rotation = GetRotation();
            auto s = GetLossyScale();

            parent->m_TransformNode.AddChild(&m_TransformNode);
            SetPosition(pos);
            SetRotation(rotation);
            SetLossyScale(s);
        }
    }
}