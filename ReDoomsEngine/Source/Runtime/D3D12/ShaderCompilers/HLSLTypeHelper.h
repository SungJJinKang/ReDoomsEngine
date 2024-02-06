#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "DirectXShaderCompiler/external/DirectX-Headers/include/directx/d3d12shader.h"

bool operator==(const std::type_info& TypeInfo, const D3D12_SHADER_TYPE_DESC& TypeDesc)
{
#define TYPE_INFO_TYPE_DESC_EQUAL(TYPE, SHADER_VARIABLE_CLASS, SHADER_VARIABLE_TYPE) \
	if (typeid(TYPE) == TypeInfo) \
	{ \
		return TypeDesc.Class == SHADER_VARIABLE_CLASS && TypeDesc.Type == SHADER_VARIABLE_TYPE; \
	}

	TYPE_INFO_TYPE_DESC_EQUAL(bool, D3D_SHADER_VARIABLE_CLASS::D3D_SVC_SCALAR, D3D_SHADER_VARIABLE_TYPE::D3D_SVT_BOOL);
	TYPE_INFO_TYPE_DESC_EQUAL(float, D3D_SHADER_VARIABLE_CLASS::D3D_SVC_SCALAR, D3D_SHADER_VARIABLE_TYPE::D3D_SVT_FLOAT);
	TYPE_INFO_TYPE_DESC_EQUAL(XMVECTOR, D3D_SHADER_VARIABLE_CLASS::D3D_SVC_VECTOR, D3D_SHADER_VARIABLE_TYPE::D3D_SVT_FLOAT);

	EA_ASSERT(false); // unsupported type
	return false;
}