#include "D3D12RootSignature.h"
#include "D3D12Shader.h"
#include "D3D12Device.h"

#define MAX_ROOT_CBVS 16

FD3D12RootSignature FD3D12RootSignature::CreateRootSignature(const FBoundShaderSet& InBoundShaderSet)
{
	FD3D12RootSignature RootSignature;
	MEM_ZERO(RootSignature.RootDesc);

	const D3D12_SHADER_VISIBILITY ShaderVisibilityPriorityOrder[] =
	{
		D3D12_SHADER_VISIBILITY_PIXEL,
		D3D12_SHADER_VISIBILITY_VERTEX,
	};

	// Root ConstantBuffer(RootDescriptor)는 사이즈가 커서 RootSignature 사이즈를 키워 성능 하락을 가져옴(https://learn.microsoft.com/en-us/windows/win32/direct3d12/using-descriptors-directly-in-the-root-signature)
	// -> 되도록이면 RootDescriptor Table을 사용함 
	const D3D12_ROOT_PARAMETER_TYPE RootParameterTypePriorityOrder[] = { D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, D3D12_ROOT_PARAMETER_TYPE_CBV };
	
	RootSignature.RootParameterCount = 0;
	RootSignature.BindingSpace = 0; // Default binding space for D3D 11 & 12 shaders

	const D3D12_RESOURCE_BINDING_TIER ResourceBindingTier = FD3D12Device::GetInstance()->GetResourceBindingTier();

	const D3D12_DESCRIPTOR_RANGE_FLAGS SRVDescriptorRangeFlags = (ResourceBindingTier <= D3D12_RESOURCE_BINDING_TIER_1) ?
		D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE :
		D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE | D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;

	const D3D12_DESCRIPTOR_RANGE_FLAGS CBVDescriptorRangeFlags = (ResourceBindingTier <= D3D12_RESOURCE_BINDING_TIER_2) ?
		D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE :
		D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE | D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;

	const D3D12_DESCRIPTOR_RANGE_FLAGS UAVDescriptorRangeFlags = (ResourceBindingTier <= D3D12_RESOURCE_BINDING_TIER_2) ?
		D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE :
		D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;

	const D3D12_DESCRIPTOR_RANGE_FLAGS SamplerDescriptorRangeFlags = (ResourceBindingTier <= D3D12_RESOURCE_BINDING_TIER_1) ?
		D3D12_DESCRIPTOR_RANGE_FLAG_NONE :
		D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;

	const D3D12_ROOT_DESCRIPTOR_FLAGS CBVRootDescriptorFlags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;	// We always set the data in an upload heap before calling Set*RootConstantBufferView.

	for (uint32_t RootParameterTypeIndex = 0; RootParameterTypeIndex < ARRAY_LENGTH(RootParameterTypePriorityOrder); RootParameterTypeIndex++)
	{
		const D3D12_ROOT_PARAMETER_TYPE& RootParameterType = RootParameterTypePriorityOrder[RootParameterTypeIndex];

		// ... and each shader stage visibility ...
		for (uint32_t ShaderVisibilityIndex = 0; ShaderVisibilityIndex < ARRAY_LENGTH(ShaderVisibilityPriorityOrder); ShaderVisibilityIndex++)
		{
			const D3D12_SHADER_VISIBILITY Visibility = ShaderVisibilityPriorityOrder[ShaderVisibilityIndex];
			const FD3D12ShaderTemplate* Shader = InBoundShaderSet.ShaderList[Visibility];
			
			if (Shader)
			{
				const FD3D12ShaderReflectionData& ReflectionData = Shader->GetD3D12ShaderReflection();

				switch (RootParameterType)
				{
				case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
				{
					if (ReflectionData.ShaderResourceCount > 0)
					{
						RootSignature.DescriptorRanges[RootSignature.RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, ReflectionData.ShaderResourceCount, 0u, RootSignature.BindingSpace, SRVDescriptorRangeFlags);
						RootSignature.TableSlots[RootSignature.RootParameterCount].InitAsDescriptorTable(1, &RootSignature.DescriptorRanges[RootSignature.RootParameterCount], Visibility);
						RootSignature.RootParameterCount++;
					}

					if (ReflectionData.ConstantBufferCount > MAX_ROOT_CBVS)
					{
						RootSignature.DescriptorRanges[RootSignature.RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, ReflectionData.ConstantBufferCount - MAX_ROOT_CBVS, MAX_ROOT_CBVS, RootSignature.BindingSpace, CBVDescriptorRangeFlags);
						RootSignature.TableSlots[RootSignature.RootParameterCount].InitAsDescriptorTable(1, &RootSignature.DescriptorRanges[RootSignature.RootParameterCount], Visibility);
						RootSignature.RootParameterCount++;
					}

					if (ReflectionData.SamplerCount > 0)
					{
						RootSignature.DescriptorRanges[RootSignature.RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, ReflectionData.SamplerCount, 0u, RootSignature.BindingSpace, SamplerDescriptorRangeFlags);
						RootSignature.TableSlots[RootSignature.RootParameterCount].InitAsDescriptorTable(1, &RootSignature.DescriptorRanges[RootSignature.RootParameterCount], Visibility);
						RootSignature.RootParameterCount++;
					}

					if (ReflectionData.UnorderedAccessCount > 0)
					{
						RootSignature.DescriptorRanges[RootSignature.RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, ReflectionData.UnorderedAccessCount, 0u, RootSignature.BindingSpace, UAVDescriptorRangeFlags);
						RootSignature.TableSlots[RootSignature.RootParameterCount].InitAsDescriptorTable(1, &RootSignature.DescriptorRanges[RootSignature.RootParameterCount], Visibility);
						RootSignature.RootParameterCount++;
					}
					break;
				}

				case D3D12_ROOT_PARAMETER_TYPE_CBV:
				{
					for (uint32_t ShaderRegister = 0; (ShaderRegister < ReflectionData.ConstantBufferCount) && (ShaderRegister < MAX_ROOT_CBVS); ShaderRegister++)
					{
						RootSignature.TableSlots[RootSignature.RootParameterCount].InitAsConstantBufferView(ShaderRegister, RootSignature.BindingSpace, CBVRootDescriptorFlags, Visibility);
						RootSignature.RootParameterCount++;
					}
					break;
				}

				default:
					EA_ASSERT(false);
					break;
				}
			}
		}
	}

	RootSignature.RootDesc.Init_1_1(RootSignature.RootParameterCount, RootSignature.TableSlots, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	D3D12_FEATURE_DATA_ROOT_SIGNATURE FeatureData = {};

	// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
	FeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (!SUCCEEDED(GetD3D12Device()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &FeatureData, sizeof(FeatureData))))
	{
		FeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	ComPtr<ID3DBlob> ErrorBlob;
	VERIFYD3D12RESULT_ERRORBLOB(D3DX12SerializeVersionedRootSignature(&RootSignature.RootDesc, FeatureData.HighestVersion, &RootSignature.RootSignatureBlob, &ErrorBlob), ErrorBlob);
	VERIFYD3D12RESULT(GetD3D12Device()->CreateRootSignature(0, RootSignature.RootSignatureBlob->GetBufferPointer(), RootSignature.RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature.RootSignature)));

	return RootSignature;
}

void FD3D12RootSignatureManager::Init()
{
}

eastl::shared_ptr<FD3D12RootSignature> FD3D12RootSignatureManager::GetOrCreateRootSignature(const FBoundShaderSet& InBoundShaderSet)
{
	eastl::shared_ptr<FD3D12RootSignature> RootSignature{};
	auto FoundRootSignature = RootSignatureMap.find(InBoundShaderSet.CachedHash);
	if (FoundRootSignature != RootSignatureMap.end())
	{
		RootSignature = FoundRootSignature->second;
	}
	else
	{
		FD3D12RootSignature CreatedD3D12RootSinature = FD3D12RootSignature::CreateRootSignature(InBoundShaderSet);
		RootSignature = RootSignatureMap.emplace(InBoundShaderSet.CachedHash, eastl::make_shared<FD3D12RootSignature>(CreatedD3D12RootSinature)).first->second;
	}
	
	return RootSignature;
}