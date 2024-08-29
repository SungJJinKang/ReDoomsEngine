#pragma once

#include "D3D12Include.h"

struct FD3D12ViewDesc
{
	EShaderParameterResourceType ShaderParameterResourceType;
};

struct FD3D12SRVDesc : public FD3D12ViewDesc
{
	uint64 FirstElement = 0;
	uint32 NumElements = UINT32_MAX;
	uint32 StructureByteStride = 0;

	eastl::optional<D3D12_SHADER_RESOURCE_VIEW_DESC> Desc{};
	
};

inline bool operator==(const D3D12_SHADER_RESOURCE_VIEW_DESC& lhs, const D3D12_SHADER_RESOURCE_VIEW_DESC& rhs)
{
	return EA::StdC::Memcmp(&lhs, &rhs, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC)) == 0;
}

inline bool operator==(const FD3D12SRVDesc& lhs, const FD3D12SRVDesc& rhs)
{
	return (lhs.ShaderParameterResourceType == rhs.ShaderParameterResourceType) && (lhs.FirstElement == rhs.FirstElement) &&
		(lhs.NumElements == rhs.NumElements) && (lhs.StructureByteStride == rhs.StructureByteStride) && (lhs.Desc == rhs.Desc);
}

namespace eastl
{
	template <> struct hash<D3D12_SHADER_RESOURCE_VIEW_DESC>
	{
		size_t operator()(D3D12_SHADER_RESOURCE_VIEW_DESC val) const
		{
			return CityHash64(reinterpret_cast<const char*>(&val), sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		}
	};

	template <> struct hash<FD3D12SRVDesc>
	{
		size_t operator()(FD3D12SRVDesc val) const 
		{
			return eastl::hash<eastl::optional<D3D12_SHADER_RESOURCE_VIEW_DESC>>()(val.Desc);
		}
	};
}