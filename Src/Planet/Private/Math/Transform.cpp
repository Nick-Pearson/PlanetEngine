#include "Transform.h"

Transform::Transform() :
	location()/*, rotation()*/, scale(1.0f, 1.0f, 1.0f)
{

}

DirectX::XMMATRIX Transform::GetMatrix() const
{
	//return DirectX::XMMatrixRotationQuaternion()
	return DirectX::XMMatrixTranslation(location.x, location.y, location.z) * DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
}
