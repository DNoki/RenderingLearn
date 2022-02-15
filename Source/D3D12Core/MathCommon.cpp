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

const float Math::Deg2Rad = 0.01745329251994329576923690768489f;
const float Math::Epsilon = std::numeric_limits<float>::epsilon();
const float Math::PI = 3.14159265358979323846264338327950288f;
const float Math::Rad2Deg = 57.295779513082320876798154814105f;


const Matrix4x4 Matrix4x4::Identity = Matrix4x4();

void Matrix4x4::SetTRS(const Vector3& p, const Vector3& r, const Vector3& s) noexcept
{
    using namespace DirectX;
    *this = XMMatrixScalingFromVector(s) * XMMatrixRotationRollPitchYawFromVector(r) * XMMatrixTranslationFromVector(p);
}