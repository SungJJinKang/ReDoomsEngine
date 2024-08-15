#pragma once
#include "d3d12.h"
#include "CommonInclude.h"

inline bool operator==(const D3D12_VERTEX_BUFFER_VIEW& A,const D3D12_VERTEX_BUFFER_VIEW& B)
{
	return A.BufferLocation == B.BufferLocation
		&& A.SizeInBytes == B.SizeInBytes
		&& A.StrideInBytes == B.StrideInBytes;
}

inline bool operator!=(const D3D12_VERTEX_BUFFER_VIEW& A,const D3D12_VERTEX_BUFFER_VIEW& B)
{
	return !(A == B);
}

inline bool operator==(const D3D12_INDEX_BUFFER_VIEW& A,const D3D12_INDEX_BUFFER_VIEW& B)
{
	return A.BufferLocation == B.BufferLocation
		&& A.SizeInBytes == B.SizeInBytes
		&& A.Format == B.Format;
}

inline bool operator!=(const D3D12_INDEX_BUFFER_VIEW& A,const D3D12_INDEX_BUFFER_VIEW& B)
{
	return !(A == B);
}

namespace eastl
{
	template <> struct hash<D3D12_VERTEX_BUFFER_VIEW>
	{
		size_t operator()(D3D12_VERTEX_BUFFER_VIEW val) const { return CityHash64(reinterpret_cast<const char*>(&val), sizeof(val)); }
	};

	template <> struct hash<D3D12_INDEX_BUFFER_VIEW>
	{
		size_t operator()(D3D12_INDEX_BUFFER_VIEW val) const { return CityHash64(reinterpret_cast<const char*>(&val), sizeof(val)); }
	};
}