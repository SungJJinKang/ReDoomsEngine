#include "ShaderCompileHelper.h"
#include "DirectXShaderCompiler/inc/dxcapi.h"
#include  <filesystem>

ComPtr<IDxcLibrary> FShaderCompileHelper::GetDxcLibrary()
{
	static ComPtr<IDxcLibrary> DxcLibrary{};
	VERIFYD3D12RESULT(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&DxcLibrary)));
	return DxcLibrary;
}

ComPtr<IDxcUtils> FShaderCompileHelper::GetDxcUtiles()
{
	static ComPtr<IDxcUtils> DxcUtils{};
	VERIFYD3D12RESULT(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&DxcUtils)));
	return DxcUtils;
}

ComPtr<IDxcCompiler3> FShaderCompileHelper::GetDxcCompilerInstance()
{
	static ComPtr<IDxcCompiler3> DxcCompiler3{};
	VERIFYD3D12RESULT(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&DxcCompiler3)));
	return DxcCompiler3;
}

ComPtr<IDxcIncludeHandler> FShaderCompileHelper::GetDxcDefualtIncludeHandler()
{
	static ComPtr<IDxcIncludeHandler> DxcDefualtIncludeHandler{};
	VERIFYD3D12RESULT(GetDxcUtiles()->CreateDefaultIncludeHandler(DxcDefualtIncludeHandler.GetAddressOf()));
	return DxcDefualtIncludeHandler;
}

ComPtr<IDxcContainerReflection> FShaderCompileHelper::GetDxcContainerReflectionInstance()
{
	static ComPtr<IDxcContainerReflection> Reflection;
	VERIFYD3D12RESULT(DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&Reflection)));
	return Reflection;
}

FShaderCompileResult FShaderCompileHelper::CompileShader(FShaderCompileArguments InShaderCompileArguments)
{
	ComPtr<IDxcBlobEncoding> DxcTextBlob{};

	EA_ASSERT(!InShaderCompileArguments.ShaderText.empty());
	if (InShaderCompileArguments.ShaderTextLength == 0)
	{
		InShaderCompileArguments.ShaderTextLength = InShaderCompileArguments.ShaderText.size();
	}

	VERIFYD3D12RESULT(GetDxcLibrary()->CreateBlobWithEncodingFromPinned((LPCVOID)InShaderCompileArguments.ShaderText.data(),
		InShaderCompileArguments.ShaderTextLength, DXC_CP_UTF8, DxcTextBlob.GetAddressOf()));

	std::filesystem::path ShaderTextFilePath{ InShaderCompileArguments.ShaderTextFilePath.data() };
	ShaderTextFilePath.make_preferred();

	eastl::vector<eastl::wstring> Arguments{};
	{
		// hlsl version
		Arguments.emplace_back(EA_WCHAR("-HV"));
		Arguments.emplace_back(EA_WCHAR("2021"));

		// Unpack uniform matrices as row-major to match the CPU layout.
		Arguments.emplace_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR);

		Arguments.emplace_back(EA_WCHAR("-auto-binding-space"));
		Arguments.emplace_back(EA_WCHAR("0"));

		if (InShaderCompileArguments.ShaderDeclaration.ShaderCompileFlags & EShaderCompileFlag::Allow16BitTYpes)
		{
			Arguments.emplace_back(EA_WCHAR("-enable-16bit-types"));
		}

		switch (InShaderCompileArguments.OptimizationLevel)
		{
		case FShaderCompileArguments::EOptimizationLevel::DisableOptimization:
			Arguments.emplace_back(DXC_ARG_SKIP_OPTIMIZATIONS);
			break;
		case FShaderCompileArguments::EOptimizationLevel::O0:
			Arguments.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL0);
			break;
		case FShaderCompileArguments::EOptimizationLevel::O1:
			Arguments.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL1);
			break;
		case FShaderCompileArguments::EOptimizationLevel::O2:
			Arguments.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL2);
			break;
		case FShaderCompileArguments::EOptimizationLevel::O3:
			Arguments.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL3);
			break;
		}

		switch (InShaderCompileArguments.WarningLevel)
		{
		case FShaderCompileArguments::EWarningLevel::W0:
			Arguments.emplace_back(EA_WCHAR("/W0"));
			break;
		case FShaderCompileArguments::EWarningLevel::W1:
			Arguments.emplace_back(EA_WCHAR("/W1"));
			break;
		case FShaderCompileArguments::EWarningLevel::W2:
			Arguments.emplace_back(EA_WCHAR("/W2"));
			break;
		case FShaderCompileArguments::EWarningLevel::W3:
			Arguments.emplace_back(EA_WCHAR("/W3"));
			break;
		case FShaderCompileArguments::EWarningLevel::All:
			Arguments.emplace_back(EA_WCHAR("/Wall"));
			break;
		}

		if (InShaderCompileArguments.bGenerateDebugInformation)
		{
			Arguments.emplace_back(DXC_ARG_DEBUG);
		}

		if (InShaderCompileArguments.bWarningAsError)
		{
			Arguments.emplace_back(DXC_ARG_WARNINGS_ARE_ERRORS);
		}

		Arguments.emplace_back(DXC_ARG_DEBUG_NAME_FOR_SOURCE); // this is required for DXC_OUT_SHADER_HASH
		Arguments.emplace_back(EA_WCHAR("-Zi"));

		if (InShaderCompileArguments.bGenerateSymbols)
		{
			// ref : https://simoncoenen.com/blog/programming/graphics/DxcCompiling#custom-include-handler
			Arguments.emplace_back(EA_WCHAR("-Qembed_debug"));

			Arguments.emplace_back(EA_WCHAR("-Fd"));
			Arguments.emplace_back(EA_WCHAR(".\\"));
		}
		else
		{
			Arguments.emplace_back(EA_WCHAR("-Qstrip_reflect"));
		}

		// Reflection will be removed later, otherwise the disassembly won't contain variables
		//Arguments.emplace_back(EA_WCHAR("-Qstrip_reflect"));

		// disable undesired warnings
		Arguments.emplace_back(EA_WCHAR("-Wno-parentheses-equality"));

		// @lh-todo: This fixes a loop unrolling issue that showed up in DOFGatherKernel with cs_6_6 with the latest DXC revision
		Arguments.emplace_back(EA_WCHAR("-disable-lifetime-markers"));
		Arguments.emplace_back(EA_WCHAR("-encoding"));
		Arguments.emplace_back(EA_WCHAR("utf8"));

		// Add directory to include search path
		Arguments.emplace_back(EA_WCHAR("-I"));
		std::filesystem::path ParentDirectory = ShaderTextFilePath.parent_path();
		Arguments.emplace_back(ParentDirectory.make_preferred().c_str());
	}

	ComPtr<IDxcCompilerArgs> DxcCompilerArgs{};

	eastl::vector<DxcDefine> DxcDefineList{};
	for (FShaderPreprocessorDefine& Definition : InShaderCompileArguments.ShaderDeclaration.AdditionalPreprocessorDefineList)
	{
		DxcDefineList.push_back_uninitialized();
		DxcDefine& Define = DxcDefineList.back();
		Define.Name = Definition.Name.c_str();
		Define.Value = Definition.Value.c_str();
	}

	eastl::vector<const wchar_t*> WcharArguments{};
	for (eastl::wstring& Arg : Arguments)
	{
		WcharArguments.push_back(Arg.data());
	}

	VERIFYD3D12RESULT(GetDxcUtiles()->BuildArguments(ShaderTextFilePath.filename().c_str(),
		InShaderCompileArguments.ShaderDeclaration.ShaderEntryPoint,
		FShaderCompileArguments::ConvertShaderFrequencyToShaderProfile(InShaderCompileArguments.ShaderDeclaration.ShaderFrequency),
		WcharArguments.data(),
		WcharArguments.size(),
		DxcDefineList.data(),
		DxcDefineList.size(),
		DxcCompilerArgs.GetAddressOf()));

	ComPtr<IDxcResult> DxcResult{};

	// Create DxcBuffer from IDxcBlob
	DxcBuffer DxcShaderBuffer = {};
	DxcShaderBuffer.Ptr = DxcTextBlob->GetBufferPointer();
	DxcShaderBuffer.Size = DxcTextBlob->GetBufferSize();

	BOOL bKnown = 0;
	UINT32 Encoding = 0;
	if (SUCCEEDED(DxcTextBlob->GetEncoding(&bKnown, &Encoding)))
	{
		if (bKnown)
		{
			DxcShaderBuffer.Encoding = Encoding;
		}
	}

	VERIFYD3D12RESULT(GetDxcCompilerInstance()->Compile(&DxcShaderBuffer, DxcCompilerArgs->GetArguments(), DxcCompilerArgs->GetCount(), 
		GetDxcDefualtIncludeHandler().Get(), IID_PPV_ARGS(&DxcResult)));
		
	HRESULT DxcResultStatus = 0;
	VERIFYD3D12RESULT(DxcResult->GetStatus(&DxcResultStatus));

	FShaderCompileResult ShaderCompileResult{};

	if (SUCCEEDED(DxcResultStatus))
	{
		{
			ComPtr<IDxcBlob> ShaderHashDxcBlob{};
			EA_ASSERT(DxcResult->HasOutput(DXC_OUT_KIND::DXC_OUT_OBJECT));
			VERIFYD3D12RESULT(DxcResult->GetOutput(DXC_OUT_KIND::DXC_OUT_OBJECT, IID_PPV_ARGS(&ShaderHashDxcBlob), nullptr));

			ShaderCompileResult.ShaderBlobData.resize(ShaderHashDxcBlob->GetBufferSize());
			EA::StdC::Memcpy(ShaderCompileResult.ShaderBlobData.data(), ShaderHashDxcBlob->GetBufferPointer(), ShaderHashDxcBlob->GetBufferSize());
		}

		{
			EA_ASSERT(DxcResult->HasOutput(DXC_OUT_KIND::DXC_OUT_SHADER_HASH));

			ComPtr<IDxcBlob> ShaderHashDxBlob{};
			VERIFYD3D12RESULT(DxcResult->GetOutput(DXC_OUT_KIND::DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&ShaderHashDxBlob), nullptr));
			DxcShaderHash* DxcShaderHashBuffer = (DxcShaderHash*)ShaderHashDxBlob->GetBufferPointer();
			EA::StdC::Memcpy(ShaderCompileResult.ShaderHash.Value, DxcShaderHashBuffer->HashDigest, SHADER_HASH_SIZE);
		}

		{
			EA_ASSERT(DxcResult->HasOutput(DXC_OUT_KIND::DXC_OUT_REFLECTION));
			ComPtr<IDxcBlob> ReflectionDataBlob{};
			VERIFYD3D12RESULT(DxcResult->GetOutput(DXC_OUT_KIND::DXC_OUT_REFLECTION, IID_PPV_ARGS(&ReflectionDataBlob), nullptr));

			DxcBuffer ReflectionDataBlobDxcBuffer = {};
			ReflectionDataBlobDxcBuffer.Ptr = ReflectionDataBlob->GetBufferPointer();
			ReflectionDataBlobDxcBuffer.Size = ReflectionDataBlob->GetBufferSize();
			
			VERIFYD3D12RESULT(GetDxcUtiles()->CreateReflection(&ReflectionDataBlobDxcBuffer, IID_PPV_ARGS(&ShaderCompileResult.DxcContainerReflection)));

			// refer : https://rtarun9.github.io/blogs/shader_reflection/
			ShaderCompileResult.DxcContainerReflection->GetDesc(&ShaderCompileResult.ShaderDesc);

		}

		ShaderCompileResult.bIsValid = true;
	}
	else
	{
		ComPtr<IDxcBlobEncoding> ErrorBuffer{};

		EA_ASSERT(DxcResult->HasOutput(DXC_OUT_KIND::DXC_OUT_ERRORS));
		VERIFYD3D12RESULT(DxcResult->GetErrorBuffer(ErrorBuffer.GetAddressOf()));

		eastl::string8 ErrorStr{ reinterpret_cast<const char8_t*>(ErrorBuffer->GetBufferPointer()), ErrorBuffer->GetBufferSize()};
		RD_LOG(ELogVerbosity::Fatal, EA_WCHAR("\
------------------------------------\n \
- Shader Compile Fail -\n \
ShaderName : %s\n \
ShaderPath : %s\n \
ShaderFrequency : %s\n \
Reason :\n\n \
%s\n \
------------------------------------\n"), 
			InShaderCompileArguments.ShaderDeclaration.ShaderName,
			InShaderCompileArguments.ShaderDeclaration.ShaderTextFileRelativePath,
			GetShaderFrequencyString(InShaderCompileArguments.ShaderDeclaration.ShaderFrequency),
			ANSI_TO_WCHAR(ErrorStr.c_str()));

		ShaderCompileResult.bIsValid = false;
	}

	return ShaderCompileResult;
}
