#include "D3D12RootSignature.h"
#include "D3D12Shader.h"

D3D12_VERSIONED_ROOT_SIGNATURE_DESC FD3D12RootSignature::CreateRootSignatureDesc(const FD3D12ShaderTemplate* const InD3D12Shader)
{
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc;
	MEM_ZERO(RootSignatureDesc);

	return D3D12_VERSIONED_ROOT_SIGNATURE_DESC();
}

void FD3D12RootSignatureManager::Init()
{
}

void FD3D12RootSignatureManager::GetOrCreateRootSignature(const FD3D12ShaderTemplate* const InD3D12Shader)
{
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc = FD3D12RootSignature::CreateRootSignatureDesc(InD3D12Shader);

	
}

