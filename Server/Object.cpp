#include "Object.h"

Object::Object() : id(-1), pos{0.f, 0.f, 0.f}, rotation(DirectX::XMFLOAT3(0.f, 0.f, 0.f)), type(Robot)
{
}