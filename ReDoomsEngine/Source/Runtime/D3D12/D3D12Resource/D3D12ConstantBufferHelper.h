#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

template <typename VariableType>
struct FD3D12ConstantBufferMemberVariableHelper
{
	static constexpr bool CanBeMemberOfConstantBuffer = false;
	static constexpr size_t Alignment = alignof(VariableType);
	using AlignedType = eastl::aligned_storage_t<sizeof(VariableType), Alignment>;
};

template <>
struct FD3D12ConstantBufferMemberVariableHelper<bool>
{
	static constexpr bool CanBeMemberOfConstantBuffer = true;
	static constexpr size_t Alignment = 4;

	using AlignedType = eastl::aligned_storage_t<sizeof(bool), Alignment>;
};

template <>
struct FD3D12ConstantBufferMemberVariableHelper<int32_t>
{
	static constexpr bool CanBeMemberOfConstantBuffer = true;
	static constexpr size_t Alignment = 4;

	using AlignedType = eastl::aligned_storage_t<sizeof(int32_t), Alignment>;
};

template <>
struct FD3D12ConstantBufferMemberVariableHelper<uint32_t>
{
	static constexpr bool CanBeMemberOfConstantBuffer = true;
	static constexpr size_t Alignment = 4;

	using AlignedType = eastl::aligned_storage_t<sizeof(uint32_t), Alignment>;
};

template <>
struct FD3D12ConstantBufferMemberVariableHelper<float>
{
	static constexpr bool CanBeMemberOfConstantBuffer = true;
	static constexpr size_t Alignment = 4;

	using AlignedType = eastl::aligned_storage_t<sizeof(float), Alignment>;
};

template <>
struct FD3D12ConstantBufferMemberVariableHelper<double>
{
	static constexpr bool CanBeMemberOfConstantBuffer = true;
	static constexpr size_t Alignment = 8;

	using AlignedType = eastl::aligned_storage_t<sizeof(double), Alignment>;
};

template <>
struct FD3D12ConstantBufferMemberVariableHelper<Vector2>
{
	static constexpr bool CanBeMemberOfConstantBuffer = true;
	static constexpr size_t Alignment = 8;

	using AlignedType = eastl::aligned_storage_t<sizeof(Vector2), Alignment>;
};

template <>
struct FD3D12ConstantBufferMemberVariableHelper<Vector3>
{
	static constexpr bool CanBeMemberOfConstantBuffer = true;
	static constexpr size_t Alignment = 16;

	using AlignedType = eastl::aligned_storage_t<sizeof(Vector3), Alignment>;
};

template <>
struct FD3D12ConstantBufferMemberVariableHelper<Vector4>
{
	static constexpr bool CanBeMemberOfConstantBuffer = true;
	static constexpr size_t Alignment = 16;

	using AlignedType = eastl::aligned_storage_t<sizeof(Vector4), Alignment>;
};

template <>
struct FD3D12ConstantBufferMemberVariableHelper<Matrix>
{
	static constexpr bool CanBeMemberOfConstantBuffer = true;
	static constexpr size_t Alignment = 16;

	using AlignedType = eastl::aligned_storage_t<sizeof(Matrix), Alignment>;
};

#define START_DEFINE_CONSTANT_BUFFER(ConstantBufferTypeName)

#define ADD_CONSTANT_BUFFER_MEMBER_VARIABLE(TypeName, VariableName)

#define END_DEFINE_CONSTANT_BUFFER