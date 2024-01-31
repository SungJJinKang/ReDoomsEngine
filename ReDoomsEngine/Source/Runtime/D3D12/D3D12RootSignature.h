#pragma once
#include "CommonInclude.h"
#include "D3D12Include.h"
#include "D3D12Shader.h"
#include "D3D12Enums.h"

class FD3D12ShaderTemplate;

// ref : https://learn.microsoft.com/en-us/windows/win32/direct3d12/creating-a-root-signature, UE5 FD3D12RootSignatureDesc::FD3D12RootSignatureDesc
class FD3D12RootSignature
{
public:

	static constexpr int32_t MaxRootParameters = 32;	// Arbitrary max, increase as needed.

	uint32_t RootParameterCount;
	uint32_t BindingSpace;

	ComPtr<ID3DBlob> RootSignatureBlob;
	ComPtr<ID3D12RootSignature> RootSignature;

	CD3DX12_ROOT_PARAMETER1 TableSlots[MaxRootParameters];
	CD3DX12_DESCRIPTOR_RANGE1 DescriptorRanges[MaxRootParameters];
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootDesc;

	struct ShaderStage
	{
		// TODO: Make these arrays and index into them by type instead of individual variables.
		uint8_t MaxCBVCount = 0;
		uint8_t MaxSRVCount = 0;
		uint8_t MaxSamplerCount = 0;
		uint8_t MaxUAVCount = 0;
		uint16_t CBVRegisterMask;
		bool bVisible = false;
	};

	eastl::array<uint8_t, EShaderFrequency::NumShaderFrequency> SRVBindSlot;
	eastl::array<uint8_t, EShaderFrequency::NumShaderFrequency> CBVBindSlot;
	eastl::array<uint8_t, EShaderFrequency::NumShaderFrequency> RootCBVBindSlot;
	eastl::array<uint8_t, EShaderFrequency::NumShaderFrequency> SamplerBindSlot;
	uint8_t UAVBindSlot;

	eastl::array<ShaderStage, EShaderFrequency::NumShaderFrequency> Stage;

	static FD3D12RootSignature CreateRootSignature(const FBoundShaderSet& InBoundShaderSet);
};

class FD3D12RootSignatureManager : public EA::StdC::Singleton<FD3D12RootSignatureManager>
{
public:

	void Init();
	eastl::shared_ptr<FD3D12RootSignature> GetOrCreateRootSignature(const FBoundShaderSet& InBoundShaderSet);

private:

	eastl::hash_map<FShaderHash, eastl::shared_ptr<FD3D12RootSignature>> RootSignatureMap;
};
