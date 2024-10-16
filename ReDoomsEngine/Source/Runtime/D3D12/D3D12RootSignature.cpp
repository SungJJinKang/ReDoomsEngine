﻿#include "D3D12RootSignature.h"
#include "D3D12Shader.h"
#include "D3D12Device.h"

#define MAX_ROOT_CBVS 16

// this code is copied from ue5

template <typename ROOT_SIGNATURE_DESC_TYPE>
static void AnalyizeRootSignature(ROOT_SIGNATURE_DESC_TYPE& Desc, FD3D12RootSignature& RootSignature)
{
	for (uint32_t ParameterIndex = 0; ParameterIndex < Desc.NumParameters; ++ParameterIndex)
	{
		const auto& RootParameter = Desc.pParameters[ParameterIndex];

		UINT RegisterSpace = -1;

		switch (RootParameter.ParameterType)
		{
		case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
			EA_ASSERT(RootParameter.DescriptorTable.NumDescriptorRanges == 1); // Code currently assumes a single descriptor range.
			RegisterSpace = RootParameter.DescriptorTable.pDescriptorRanges[0].RegisterSpace;
			break;
		case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
			RegisterSpace = RootParameter.Constants.RegisterSpace;
			break;
		case D3D12_ROOT_PARAMETER_TYPE_CBV:
		case D3D12_ROOT_PARAMETER_TYPE_SRV:
		case D3D12_ROOT_PARAMETER_TYPE_UAV:
			RegisterSpace = RootParameter.Descriptor.RegisterSpace;
			break;
		default:
			EA_ASSERT(false);
			break;
		}

		EA_ASSERT(RegisterSpace == 0); // only consider register space 0

		// Determine shader resource counts.
		{
			switch (RootParameter.ParameterType)
			{
				case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
				{
					EA_ASSERT(RootParameter.DescriptorTable.NumDescriptorRanges == 1);	// Code currently assumes a single descriptor range.
					const auto& CurrentRange = RootParameter.DescriptorTable.pDescriptorRanges[0];

					{
						EA_ASSERT(CurrentRange.BaseShaderRegister == 0);	// Code currently assumes always starting at register 0.

						switch (CurrentRange.RangeType)
						{
						case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
						{
							if (RootParameter.ShaderVisibility == D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL)
							{
								for (size_t ShaderFrequency = 0; ShaderFrequency <= EShaderFrequency::NumShaderFrequency; ++ShaderFrequency)
								{
									if (IsSupportedShaderFrequency(D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)))
									{
										RootSignature.Stage[ShaderFrequency].MaxSRVCount = CurrentRange.NumDescriptors;
										RootSignature.SRVBindSlot[ShaderFrequency] = ParameterIndex;
									}
								}
							}
							else
							{
								if (IsSupportedShaderFrequency(D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)))
								{
									RootSignature.Stage[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)].MaxSRVCount = CurrentRange.NumDescriptors;
									RootSignature.SRVBindSlot[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)] = ParameterIndex;
								}
							}
							break;
						}
						case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
						{
							if (RootParameter.ShaderVisibility == D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL)
							{
								for (size_t ShaderFrequency = 0; ShaderFrequency <= EShaderFrequency::NumShaderFrequency; ++ShaderFrequency)
								{
									if (IsSupportedShaderFrequency(D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)))
									{
										RootSignature.Stage[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)].MaxUAVCount = CurrentRange.NumDescriptors;
									}
								}
							}
							else
							{
								if (IsSupportedShaderFrequency(D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)))
								{
									RootSignature.Stage[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)].MaxUAVCount = CurrentRange.NumDescriptors;
								}
							}
							RootSignature.UAVBindSlot = ParameterIndex;
							break;
						}
						case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
						{
							if (RootParameter.ShaderVisibility == D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL)
							{
								for (size_t ShaderFrequency = 0; ShaderFrequency <= EShaderFrequency::NumShaderFrequency; ++ShaderFrequency)
								{
									if (IsSupportedShaderFrequency(D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)))
									{
										RootSignature.Stage[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)].MaxCBVCount += CurrentRange.NumDescriptors;
									}
								}
							}
							else
							{
								RootSignature.Stage[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)].MaxCBVCount += CurrentRange.NumDescriptors;
							}
							RootSignature.CBVBindSlot[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)] = ParameterIndex;

							const uint32_t StartRegister = CurrentRange.BaseShaderRegister;
							const uint32_t EndRegister = StartRegister + CurrentRange.NumDescriptors;
							const EShaderFrequency StartShaderFrequency = (RootParameter.ShaderVisibility == D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL) ? EShaderFrequency::Vertex : D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility);
							const EShaderFrequency EndShaderFrequency = (RootParameter.ShaderVisibility == D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL) ? EShaderFrequency::Vertex : D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility);
							for (uint8_t CurrentShaderFrequency = StartShaderFrequency; CurrentShaderFrequency <= EndShaderFrequency; CurrentShaderFrequency++)
							{
								for (uint32 Register = StartRegister; Register < EndRegister; Register++)
								{
									// The bit shouldn't already be set for the current register.
									EA_ASSERT((RootSignature.Stage[CurrentShaderFrequency].CBVRegisterMask & (1 << Register)) == 0);
									RootSignature.Stage[CurrentShaderFrequency].CBVRegisterMask |= (1 << Register);
								}
							}

							break;
						}
						case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
						{
							if (RootParameter.ShaderVisibility == D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL)
							{
								for (size_t ShaderFrequency = 0; ShaderFrequency <= EShaderFrequency::NumShaderFrequency; ++ShaderFrequency)
								{
									RootSignature.Stage[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)].MaxSamplerCount = CurrentRange.NumDescriptors;
								}
							}
							else
							{
								RootSignature.Stage[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)].MaxSamplerCount = CurrentRange.NumDescriptors;
							}
							RootSignature.SamplerBindSlot[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)] = ParameterIndex;
							break;
						}
						default: EA_ASSERT(false); break;
						}
					}
					break;
				}
				case D3D12_ROOT_PARAMETER_TYPE_CBV:
				{
					EA_ASSERT(RootParameter.Descriptor.RegisterSpace == 0); // Parameters in other binding spaces are expected to be filtered out at this point

					if (RootParameter.ShaderVisibility == D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL)
					{
						for (size_t ShaderFrequency = 0; ShaderFrequency <= EShaderFrequency::NumShaderFrequency; ++ShaderFrequency)
						{
							RootSignature.Stage[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)].MaxCBVCount += 1;
						}
					}
					else
					{
						RootSignature.Stage[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)].MaxCBVCount += 1;
					}

					if (RootParameter.Descriptor.ShaderRegister == 0)
					{
						RootSignature.RootCBVBindSlot[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)] = ParameterIndex;
					}

					const EShaderFrequency StartShaderFrequency = (RootParameter.ShaderVisibility == D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL) ? EShaderFrequency::Vertex : D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility);
					const EShaderFrequency EndShaderFrequency = (RootParameter.ShaderVisibility == D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL) ? EShaderFrequency::Vertex : D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility);
					for (uint8_t CurrentShaderFrequency = StartShaderFrequency; CurrentShaderFrequency <= EndShaderFrequency; CurrentShaderFrequency++)
					{
						// The bit shouldn't already be set for the current register.
						EA_ASSERT((RootSignature.Stage[CurrentShaderFrequency].CBVRegisterMask & (1 << RootParameter.Descriptor.ShaderRegister)) == 0);
						RootSignature.Stage[CurrentShaderFrequency].CBVRegisterMask |= (1 << RootParameter.Descriptor.ShaderRegister);
					}

					// The first CBV for this stage must come first in the root signature, and subsequent root CBVs for this stage must be contiguous.
					EA_ASSERT(RootSignature.RootCBVBindSlot[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)] != 0xFF);
					EA_ASSERT(RootSignature.RootCBVBindSlot[D3D12ShaderVisibilityToShaderFrequency(RootParameter.ShaderVisibility)] + RootParameter.Descriptor.ShaderRegister == ParameterIndex);
				}
				break;

				default:
				{
					EA_ASSERT(false);
					break;
				}
			}
		}
	}
}

static eastl::array<D3D12_STATIC_SAMPLER_DESC, EStaticSamplerType::NumStaticSamplerType> GStaticSamplerDescList
{
	D3D12_STATIC_SAMPLER_DESC{
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
			0, // MipLODBias
			D3D12_MAX_MAXANISOTROPY,
			D3D12_COMPARISON_FUNC_NEVER,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
			0, // MinLOD
			FLT_MAX, // MaxLOD
			0, // ShaderRegister
			0, // RegisterSpace
			D3D12_SHADER_VISIBILITY_ALL,
	},
	D3D12_STATIC_SAMPLER_DESC{
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressW
			0, // MipLODBias
			D3D12_MAX_MAXANISOTROPY,
			D3D12_COMPARISON_FUNC_NEVER,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
			0, // MinLOD
			FLT_MAX, // MaxLOD
			0, // ShaderRegister
			0, // RegisterSpace
			D3D12_SHADER_VISIBILITY_ALL,
	},
	D3D12_STATIC_SAMPLER_DESC{
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR, // AddressU
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR, // AddressV
			D3D12_TEXTURE_ADDRESS_MODE_MIRROR, // AddressW
			0, // MipLODBias
			D3D12_MAX_MAXANISOTROPY,
			D3D12_COMPARISON_FUNC_NEVER,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
			0, // MinLOD
			FLT_MAX, // MaxLOD
			0, // ShaderRegister
			0, // RegisterSpace
			D3D12_SHADER_VISIBILITY_ALL,
	},
	D3D12_STATIC_SAMPLER_DESC{
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
		0, // MipLODBias
		D3D12_MAX_MAXANISOTROPY,
		D3D12_COMPARISON_FUNC_NEVER,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
		0, // MinLOD
		FLT_MAX, // MaxLOD
		0, // ShaderRegister
		0, // RegisterSpace
		D3D12_SHADER_VISIBILITY_ALL,
	},
	D3D12_STATIC_SAMPLER_DESC{
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressW
		0, // MipLODBias
		D3D12_MAX_MAXANISOTROPY,
		D3D12_COMPARISON_FUNC_NEVER,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
		0, // MinLOD
		FLT_MAX, // MaxLOD
		0, // ShaderRegister
		0, // RegisterSpace
		D3D12_SHADER_VISIBILITY_ALL,
	},
	D3D12_STATIC_SAMPLER_DESC{
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_MIRROR, // AddressU
		D3D12_TEXTURE_ADDRESS_MODE_MIRROR, // AddressV
		D3D12_TEXTURE_ADDRESS_MODE_MIRROR, // AddressW
		0, // MipLODBias
		D3D12_MAX_MAXANISOTROPY,
		D3D12_COMPARISON_FUNC_NEVER,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
		0, // MinLOD
		FLT_MAX, // MaxLOD
		0, // ShaderRegister
		0, // RegisterSpace
		D3D12_SHADER_VISIBILITY_ALL,
	},
	D3D12_STATIC_SAMPLER_DESC{
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP, // AddressW
		0, // MipLODBias
		D3D12_MAX_MAXANISOTROPY,
		D3D12_COMPARISON_FUNC_NEVER,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
		0, // MinLOD
		FLT_MAX, // MaxLOD
		0, // ShaderRegister
		0, // RegisterSpace
		D3D12_SHADER_VISIBILITY_ALL,
	},
	D3D12_STATIC_SAMPLER_DESC{
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // AddressW
		0, // MipLODBias
		D3D12_MAX_MAXANISOTROPY,
		D3D12_COMPARISON_FUNC_NEVER,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
		0, // MinLOD
		FLT_MAX, // MaxLOD
		0, // ShaderRegister
		0, // RegisterSpace
		D3D12_SHADER_VISIBILITY_ALL,
	},
	D3D12_STATIC_SAMPLER_DESC{
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_MIRROR, // AddressU
		D3D12_TEXTURE_ADDRESS_MODE_MIRROR, // AddressV
		D3D12_TEXTURE_ADDRESS_MODE_MIRROR, // AddressW
		0, // MipLODBias
		D3D12_MAX_MAXANISOTROPY,
		D3D12_COMPARISON_FUNC_NEVER,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK,
		0, // MinLOD
		FLT_MAX, // MaxLOD
		0, // ShaderRegister
		0, // RegisterSpace
		D3D12_SHADER_VISIBILITY_ALL,
	}
};

/*
	https://gpuopen.com/wp-content/uploads/2016/03/GDC_2016_D3D12_Right_On_Queue_final.pdf
	Root copied to SGPR on launch
	‒Layout defined at compile time
	‒Only what’s required for each shader
	stage
	‒Too many SGPR ->
	Root Signature will spill into local
	memory
	 Most frequently changed entries
	first
	 Avoid spilling of Descriptor Tables!
*/

FD3D12RootSignature FD3D12RootSignature::CreateRootSignature(const FBoundShaderSet& InBoundShaderSet)
{
	FD3D12RootSignature RootSignature;
	MEM_ZERO(RootSignature.RootDesc);

	const D3D12_SHADER_VISIBILITY ShaderVisibilityPriorityOrder[] =
	{
		D3D12_SHADER_VISIBILITY_PIXEL,
		D3D12_SHADER_VISIBILITY_VERTEX,
	};

	// Root ConstantBuffer can degrade performance because it's size is big(https://learn.microsoft.com/en-us/windows/win32/direct3d12/using-descriptors-directly-in-the-root-signature)
	// -> Never use root constant buffer if possible
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


	eastl::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplerDescList{};

	for (uint32_t RootParameterTypeIndex = 0; RootParameterTypeIndex < ARRAY_LENGTH(RootParameterTypePriorityOrder); RootParameterTypeIndex++)
	{
		const D3D12_ROOT_PARAMETER_TYPE& RootParameterType = RootParameterTypePriorityOrder[RootParameterTypeIndex];

		// ... and each shader stage visibility ...
		for (uint32_t ShaderVisibilityIndex = 0; ShaderVisibilityIndex < ARRAY_LENGTH(ShaderVisibilityPriorityOrder); ShaderVisibilityIndex++)
		{
			const D3D12_SHADER_VISIBILITY Visibility = ShaderVisibilityPriorityOrder[ShaderVisibilityIndex];
			const EShaderFrequency ShaderFrequency = D3D12ShaderVisibilityToShaderFrequency(Visibility);
			const FD3D12Material* const Material = InBoundShaderSet.GetMaterialList()[ShaderFrequency];
			
			if (Material)
			{
				const FD3D12ShaderReflectionData& ReflectionData = Material->GetShader()->GetD3D12ShaderReflection();

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
						EA_ASSERT(false);

// 						RootSignature.DescriptorRanges[RootSignature.RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, ReflectionData.ConstantBufferCount - MAX_ROOT_CBVS, MAX_ROOT_CBVS, RootSignature.BindingSpace, CBVDescriptorRangeFlags);
// 						RootSignature.TableSlots[RootSignature.RootParameterCount].InitAsDescriptorTable(1, &RootSignature.DescriptorRanges[RootSignature.RootParameterCount], Visibility);
// 						RootSignature.RootParameterCount++;
					}

					if (ReflectionData.SamplerCount > 0)
					{
// 						RootSignature.DescriptorRanges[RootSignature.RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, ReflectionData.SamplerCount, 0u, RootSignature.BindingSpace, SamplerDescriptorRangeFlags);
// 						RootSignature.TableSlots[RootSignature.RootParameterCount].InitAsDescriptorTable(1, &RootSignature.DescriptorRanges[RootSignature.RootParameterCount], Visibility);
// 						RootSignature.RootParameterCount++;

						for (const FSamplerResourceBindingDesc& SamplerResourceBindingDesc : ReflectionData.SamplerResourceBindingDescList)
						{
							EA_ASSERT(SamplerResourceBindingDesc.SamplerType < EStaticSamplerType::NumStaticSamplerType);
							if(SamplerResourceBindingDesc.SamplerType < EStaticSamplerType::NumStaticSamplerType)
							{
								D3D12_STATIC_SAMPLER_DESC StaticSamplerDesc = GStaticSamplerDescList[SamplerResourceBindingDesc.SamplerType];
								StaticSamplerDesc.ShaderRegister = SamplerResourceBindingDesc.Desc.BindPoint;
								StaticSamplerDescList.emplace_back(StaticSamplerDesc);
							}
							else if(SamplerResourceBindingDesc.SamplerType == EStaticSamplerType::DynamicSampler)
							{
								RootSignature.DescriptorRanges[RootSignature.RootParameterCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, ReflectionData.SamplerCount, 0u, RootSignature.BindingSpace, SamplerDescriptorRangeFlags);
								RootSignature.TableSlots[RootSignature.RootParameterCount].InitAsDescriptorTable(1, &RootSignature.DescriptorRanges[RootSignature.RootParameterCount], Visibility);
								RootSignature.RootParameterCount++;
							}
							else
							{
								EA_ASSERT(false);
							}
						}
						
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
					// if possible, root cbv is used other than root cbv descriptor table
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

	RootSignature.RootDesc.Init_1_1(RootSignature.RootParameterCount, RootSignature.TableSlots, StaticSamplerDescList.size(), StaticSamplerDescList.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	D3D12_FEATURE_DATA_ROOT_SIGNATURE FeatureData = {};

	// @todo support 1_2
	if (SUCCEEDED(GetD3D12Device()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &FeatureData, sizeof(FeatureData))))
	{
		FeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	}
	else
	{
		FeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	ComPtr<ID3DBlob> ErrorBlob;
	VERIFYD3D12RESULT_ERRORBLOB(D3DX12SerializeVersionedRootSignature(&RootSignature.RootDesc, FeatureData.HighestVersion, &RootSignature.RootSignatureBlob, &ErrorBlob), ErrorBlob);
	VERIFYD3D12RESULT(GetD3D12Device()->CreateRootSignature(0, RootSignature.RootSignatureBlob->GetBufferPointer(), RootSignature.RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&RootSignature.RootSignature)));

	if (FeatureData.HighestVersion == D3D_ROOT_SIGNATURE_VERSION_1_1)
	{
		AnalyizeRootSignature(RootSignature.RootDesc.Desc_1_1, RootSignature);
	}
	else if (FeatureData.HighestVersion == D3D_ROOT_SIGNATURE_VERSION_1_0)
	{
		AnalyizeRootSignature(RootSignature.RootDesc.Desc_1_0, RootSignature);
	}
	else
	{
		EA_ASSERT(false);
	}
	
	return RootSignature;
}

void FD3D12RootSignatureManager::Init()
{
}

eastl::shared_ptr<FD3D12RootSignature> FD3D12RootSignatureManager::GetOrCreateRootSignature(const FBoundShaderSet& InBoundShaderSet)
{
	eastl::shared_ptr<FD3D12RootSignature> RootSignature{};
	auto FoundRootSignature = RootSignatureMap.find(InBoundShaderSet.GetCachedHash());
	if (FoundRootSignature != RootSignatureMap.end())
	{
		RootSignature = FoundRootSignature->second;
	}
	else
	{
		FD3D12RootSignature CreatedD3D12RootSinature = FD3D12RootSignature::CreateRootSignature(InBoundShaderSet);
		RootSignature = RootSignatureMap.emplace(InBoundShaderSet.GetCachedHash(), eastl::make_shared<FD3D12RootSignature>(CreatedD3D12RootSinature)).first->second;
	}
	EA_ASSERT(RootSignature);
	
	return RootSignature;
}

void FD3D12RootSignatureManager::OnStartFrame(FD3D12CommandContext& InCommandContext)
{

}

void FD3D12RootSignatureManager::OnEndFrame(FD3D12CommandContext& InCommandContext)
{

}
