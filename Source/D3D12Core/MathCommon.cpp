#include "pch.h"

#include "MathCommon.h"

// --------------------------------------------------------------------------
/*
    行主序与列主序
        若将内存中存储的矩阵看做一维列表
            0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
        行主序
            存储顺序
                0       1       2       3
                4       5       6       7
                8       9       10      11
                12(x)   13(y)   14(z)   15
            乘法顺序：右乘(v * M)

        列主序
            存储顺序
                0   1   2   3(x)
                4   5   6   7(y)
                8   9   10  11(z)
                12  13  14  15
            乘法顺序：左乘(M * v)

    D3D 中的矩阵存储
        测试 DirectXMath 为行主序
        在 HLSL 中默认为列主序，程序应向 HLSL 输入行主序矩阵，根据编译模式（\Zpc=列主序、\Zpr=行主序）解释矩阵
        例：输入矩阵为 1 0 0 0 0 1 0 0 0 0 1 0 x y z 1（行）
            /Zpc       1 0 0 x 0 1 0 y 0 0 1 z 0 0 0 1（列）应使用左乘
            /Zpr       1 0 0 0 0 1 0 0 0 0 1 0 x y z 1（行）应使用右乘
*/
// --------------------------------------------------------------------------

using namespace std;

const Quaternion Quaternion::Identity = Quaternion();

Vector3 Quaternion::GetEulerAngles() const noexcept
{
    // https://zhuanlan.zhihu.com/p/45404840?from=groupmessage
    using namespace DirectX;
    const Quaternion& q = *this;
    float pitch, yaw, roll;
#if 0
    // GLM 标准 XYZ ?
    // z
    roll = (Math::Atan2(2.0f * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z));
    // x
    pitch;
    {
        float _y = 2.0f * (q.y * q.z + q.w * q.x);
        float _x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

        if (!(Math::Abs(_x - _y) > Math::Epsilon)) //avoid atan2(0,0) - handle singularity - Matiis
            pitch = 2.0f * Math::Atan2(q.x, q.w);
        else pitch = Math::Atan2(_y, _x);
    }
    // y
    yaw = Math::Asin(Math::Clamp(-2.0f * (q.x * q.z - q.w * q.y), -1.0f, 1.0f));
#endif
#if 0
    // 旋转矩阵转欧拉角  YXZ 顺规
    Matrix4x4 rotMatrix = XMMatrixRotationQuaternion(*this);
    pitch = Math::Asin(-rotMatrix._32);
    if (Math::Approximately(Math::Abs(rotMatrix._32), 1.0f, 0.000001f))
    {
        // sin(x) 值为±1，解决 Gimbal Lock
        if (pitch < 0) // 俯仰角为 -90°
        {
            // yaw + roll = Math::Atan2(-rotMatrix._21, rotMatrix._11);
            roll = 0.0f;
            yaw = Math::Atan2(-rotMatrix._21, rotMatrix._11);
        }
        else // 俯仰角为 90°
        {
            // yaw - roll = Math::Atan2(rotMatrix._21, rotMatrix._11);
            float value = Math::Atan2(rotMatrix._21, rotMatrix._11);
            roll = 0.0f;
            yaw = value;
        }
    }
    else
    {
        roll = Math::Atan2(rotMatrix._12, rotMatrix._22);
        yaw = Math::Atan2(rotMatrix._31, rotMatrix._33);
    }
#endif
    // 四元数转欧拉角  YXZ 顺规
    float sinPitch = Math::Clamp(2.0f * (q.w * q.x - q.y * q.z), -1.0f, 1.0f);
    pitch = Math::Asin(sinPitch);
    if (Math::Approximately(Math::Abs(sinPitch), 1.0f, 0.000001f))
    {
        // sin(x) 值为±1，解决 Gimbal Lock
        roll = 0.0f;
        yaw = 2.0f * Math::Atan2(q.y, q.w);
    }
    else
    {
        roll = Math::Atan2(2.0f * (q.y * q.x + q.w * q.z), q.w * q.w + q.y * q.y - q.x * q.x - q.z * q.z);
        yaw = Math::Atan2(2.0f * (q.x * q.z + q.y * q.w), q.w * q.w - q.y * q.y - q.x * q.x + q.z * q.z);
    }

    return Vector3(pitch, yaw, roll) * Math::Rad2Deg;
}


const Matrix4x4 Matrix4x4::Identity = Matrix4x4();


bool Matrix4x4::GetTRS(OUT Vector3& t, OUT Quaternion& r, OUT Vector3& s) const noexcept
{
    using namespace DirectX;
    XMVECTOR _t, _r, _s;
    bool result = XMMatrixDecompose(&_t, &_r, &_s, *this);
    if (result)
    {
        t = _t; r = _r; s = _s;
    }
    return result;
}
