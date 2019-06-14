#include "Transform.h"
#include "../PlanetLogging.h"

Transform::Transform() :
	location()/*, rotation()*/, scale(1.0f, 1.0f, 1.0f)
{

}

DirectX::XMMATRIX Transform::GetMatrix() const
{
	Quaternion rotNorm = rotation;
	rotNorm.Normalise();


	return DirectX::XMMatrixRotationQuaternion(rotNorm.value) *
		DirectX::XMMatrixTranslation(location.x, location.y, location.z) *
		DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
}
