#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"

class FD3D12Shader;

// ref : https://learn.microsoft.com/en-us/windows/win32/direct3d12/creating-a-root-signature, UE5 FD3D12RootSignatureDesc::FD3D12RootSignatureDesc
class FD3D12RootSignature
{
public:

	static D3D12_VERSIONED_ROOT_SIGNATURE_DESC CreateRootSignatureDesc(const FD3D12Shader* const InD3D12Shader);
};

class FD3D12RootSignatureManager : public EA::StdC::Singleton<FD3D12RootSignatureManager>
{
public:

	void Init();
	void CreateAndAddNewRootSignature(const FD3D12Shader* const InD3D12Shader);

private:

	eastl::vector_map<const FD3D12Shader*, eastl::unique_ptr<FD3D12RootSignature>> RootSignatureMap;
};
