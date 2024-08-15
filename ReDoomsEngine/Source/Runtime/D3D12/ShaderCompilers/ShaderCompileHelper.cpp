#include "ShaderCompileHelper.h"
#include "DirectXShaderCompiler/include/dxc/dxcapi.h"
#include "DirectXShaderCompiler/include/dxc/dxctools.h"
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

ComPtr<IDxcRewriter2> FShaderCompileHelper::GetDxcRewriterInstance()
{
	static ComPtr<IDxcRewriter2> Rewriter;
	VERIFYD3D12RESULT(DxcCreateInstance(CLSID_DxcRewriter, IID_PPV_ARGS(&Rewriter)));
	return Rewriter;
}

FShaderCompileResult FShaderCompileHelper::CompileShader(FShaderCompileArguments InShaderCompileArguments)
{
	ComPtr<IDxcBlobEncoding> ShaderTextBlob{};

	EA_ASSERT(!InShaderCompileArguments.ShaderText.empty());
	if (InShaderCompileArguments.ShaderTextLength == 0)
	{
		InShaderCompileArguments.ShaderTextLength = InShaderCompileArguments.ShaderText.size();
	}

	VERIFYD3D12RESULT(GetDxcLibrary()->CreateBlobWithEncodingFromPinned((LPCVOID)InShaderCompileArguments.ShaderText.data(),
		InShaderCompileArguments.ShaderTextLength, DXC_CP_UTF8, ShaderTextBlob.GetAddressOf()));

	std::filesystem::path ShaderTextFilePath{ InShaderCompileArguments.ShaderTextFilePath.data() };
	ShaderTextFilePath.make_preferred();

	eastl::vector<LPCWSTR> CommonArguments{};

	// hlsl version
	CommonArguments.emplace_back(EA_WCHAR("-HV"));
	CommonArguments.emplace_back(EA_WCHAR("2021"));

	if (InShaderCompileArguments.ShaderDeclaration.ShaderCompileFlags & EShaderCompileFlag::Allow16BitTYpes)
	{
		CommonArguments.emplace_back(EA_WCHAR("-enable-16bit-types"));
	}

	switch (InShaderCompileArguments.WarningLevel)
	{
	case FShaderCompileArguments::EWarningLevel::W0:
		CommonArguments.emplace_back(EA_WCHAR("/W0"));
		break;
	case FShaderCompileArguments::EWarningLevel::W1:
		CommonArguments.emplace_back(EA_WCHAR("/W1"));
		break;
	case FShaderCompileArguments::EWarningLevel::W2:
		CommonArguments.emplace_back(EA_WCHAR("/W2"));
		break;
	case FShaderCompileArguments::EWarningLevel::W3:
		CommonArguments.emplace_back(EA_WCHAR("/W3"));
		break;
	case FShaderCompileArguments::EWarningLevel::All:
		CommonArguments.emplace_back(EA_WCHAR("/Wall"));
		break;
	}

	if (InShaderCompileArguments.bGenerateDebugInformation)
	{
		CommonArguments.emplace_back(DXC_ARG_DEBUG);
	}

	if (InShaderCompileArguments.bWarningAsError)
	{
		CommonArguments.emplace_back(DXC_ARG_WARNINGS_ARE_ERRORS);
	}

	// Reflection will be removed later, otherwise the disassembly won't contain variables
	//CommonArguments.emplace_back(EA_WCHAR("-Qstrip_reflect"));


	CommonArguments.emplace_back(EA_WCHAR("-encoding"));
	CommonArguments.emplace_back(EA_WCHAR("utf8"));

	// Add directory to include search path
	std::filesystem::path ParentDirectory = ShaderTextFilePath.parent_path();

	eastl::wstring ShaderTextFilePathStr = EA_WCHAR("-I");
	ShaderTextFilePathStr += ParentDirectory.make_preferred().c_str();
	CommonArguments.emplace_back(ShaderTextFilePathStr.data());


	ComPtr<IDxcCompilerArgs> DxcCompilerArgs{};

	eastl::vector<DxcDefine> DxcDefineList{};
	for (FShaderPreprocessorDefine& Definition : InShaderCompileArguments.ShaderDeclaration.AdditionalPreprocessorDefineList)
	{
		DxcDefineList.push_back_uninitialized();
		DxcDefine& Define = DxcDefineList.back();
		Define.Name = Definition.Name.c_str();
		Define.Value = Definition.Value.c_str();
	}

	FShaderCompileResult ShaderCompileResult{};

	{
		eastl::vector<LPCWSTR> RewriterArguements{ CommonArguments };
		RewriterArguements.emplace_back(EA_WCHAR("-remove-unused-globals"));
	
		eastl::wstring EntryPointArgument{ EA_WCHAR("-E") };
		EntryPointArgument += InShaderCompileArguments.ShaderDeclaration.ShaderEntryPoint;
		RewriterArguements.emplace_back(EntryPointArgument.data());

		ComPtr<IDxcOperationResult> DxcRewriteResult{};
		VERIFYD3D12RESULT(GetDxcRewriterInstance()->RewriteWithOptions(
			ShaderTextBlob.Get(),
			ShaderTextFilePath.filename().c_str(),
			RewriterArguements.data(), RewriterArguements.size(),
			DxcDefineList.data(), DxcDefineList.size(),
			GetDxcDefualtIncludeHandler().Get(),
			DxcRewriteResult.GetAddressOf()));

		HRESULT DxcResultStatus = 0;
		VERIFYD3D12RESULT(DxcRewriteResult->GetStatus(&DxcResultStatus));

		if (SUCCEEDED(DxcResultStatus))
		{
			ComPtr<IDxcBlob> RewrittenShaderTextBlob{};
			VERIFYD3D12RESULT(DxcRewriteResult->GetResult(RewrittenShaderTextBlob.GetAddressOf()));

			VERIFYD3D12RESULT(GetDxcLibrary()->GetBlobAsUtf8(RewrittenShaderTextBlob.Get(), ShaderTextBlob.GetAddressOf()));

			// print rewritten shader text
 			eastl::string8 RewrittenShaderText{ reinterpret_cast<const char8_t*>(ShaderTextBlob->GetBufferPointer()), ShaderTextBlob->GetBufferSize() };
 			RD_LOG(ELogVerbosity::Log, EA_WCHAR("%s"), RewrittenShaderText);
		}
		else
		{
			ComPtr<IDxcBlobEncoding> ErrorBuffer{};

			VERIFYD3D12RESULT(DxcRewriteResult->GetErrorBuffer(ErrorBuffer.GetAddressOf()));

			eastl::string8 ErrorStr{ reinterpret_cast<const char8_t*>(ErrorBuffer->GetBufferPointer()), ErrorBuffer->GetBufferSize() };
			RD_LOG(ELogVerbosity::Fatal, EA_WCHAR("\
------------------------------------\n \
- Shader Rewrite Fail -\n \
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
			return ShaderCompileResult;
		}
	}

	{
		eastl::vector<LPCWSTR> DXCArguements{ CommonArguments };

		switch (InShaderCompileArguments.OptimizationLevel)
		{
		case FShaderCompileArguments::EOptimizationLevel::DisableOptimization:
			DXCArguements.emplace_back(DXC_ARG_SKIP_OPTIMIZATIONS);
			break;
		case FShaderCompileArguments::EOptimizationLevel::O0:
			DXCArguements.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL0);
			break;
		case FShaderCompileArguments::EOptimizationLevel::O1:
			DXCArguements.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL1);
			break;
		case FShaderCompileArguments::EOptimizationLevel::O2:
			DXCArguements.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL2);
			break;
		case FShaderCompileArguments::EOptimizationLevel::O3:
			DXCArguements.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL3);
			break;
		}

		// Unpack uniform matrices as row-major to match the CPU layout.
		DXCArguements.emplace_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR);

		DXCArguements.emplace_back(DXC_ARG_DEBUG_NAME_FOR_SOURCE); // this is required for DXC_OUT_SHADER_HASH

		// disable undesired warnings
		DXCArguements.emplace_back(EA_WCHAR("-Wno-parentheses-equality"));

		if (InShaderCompileArguments.bGenerateSymbols)
		{
			// ref : https://simoncoenen.com/blog/programming/graphics/DxcCompiling#custom-include-handler
			DXCArguements.emplace_back(EA_WCHAR("-Qembed_debug"));

			DXCArguements.emplace_back(EA_WCHAR("-Fd"));
			DXCArguements.emplace_back(EA_WCHAR(".\\"));
		}
		else
		{
			DXCArguements.emplace_back(EA_WCHAR("-Qstrip_reflect"));
		}

		DXCArguements.emplace_back(EA_WCHAR("-Zi"));

		// check https://learn.microsoft.com/en-us/windows/win32/direct3d12/resource-binding-in-hlsl
		DXCArguements.emplace_back(EA_WCHAR("-auto-binding-space"));
		DXCArguements.emplace_back(EA_WCHAR("0"));

		VERIFYD3D12RESULT(GetDxcUtiles()->BuildArguments(ShaderTextFilePath.filename().c_str(),
			InShaderCompileArguments.ShaderDeclaration.ShaderEntryPoint,
			FShaderCompileArguments::ConvertShaderFrequencyToShaderProfile(InShaderCompileArguments.ShaderDeclaration.ShaderFrequency),
			DXCArguements.data(),
			DXCArguements.size(),
			DxcDefineList.data(),
			DxcDefineList.size(),
			DxcCompilerArgs.GetAddressOf()));

		ComPtr<IDxcResult> DxcResult{};

		// Create DxcBuffer from IDxcBlob
		DxcBuffer DxcShaderBuffer = {};
		DxcShaderBuffer.Ptr = ShaderTextBlob->GetBufferPointer();
		DxcShaderBuffer.Size = ShaderTextBlob->GetBufferSize();

		BOOL bKnown = 0;
		UINT32 Encoding = 0;
		if (SUCCEEDED(ShaderTextBlob->GetEncoding(&bKnown, &Encoding)))
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


		if (SUCCEEDED(DxcResultStatus))
		{
			{
				ComPtr<IDxcBlob> ShaderHashDxcBlob{};
				EA_ASSERT(DxcResult->HasOutput(DXC_OUT_KIND::DXC_OUT_OBJECT));
				VERIFYD3D12RESULT(DxcResult->GetOutput(DXC_OUT_KIND::DXC_OUT_OBJECT, IID_PPV_ARGS(&ShaderHashDxcBlob), nullptr));

				ShaderCompileResult.ShaderBlobData = ShaderHashDxcBlob;
			}

			{
				EA_ASSERT(DxcResult->HasOutput(DXC_OUT_KIND::DXC_OUT_SHADER_HASH));

				ComPtr<IDxcBlob> ShaderHashDxBlob{};
				VERIFYD3D12RESULT(DxcResult->GetOutput(DXC_OUT_KIND::DXC_OUT_SHADER_HASH, IID_PPV_ARGS(&ShaderHashDxBlob), nullptr));
				DxcShaderHash* DxcShaderHashBuffer = (DxcShaderHash*)ShaderHashDxBlob->GetBufferPointer();

				EA::StdC::Memcpy(ShaderCompileResult.ShaderHash.Value, DxcShaderHashBuffer->HashDigest, sizeof(ShaderCompileResult.ShaderHash.Value[0]));
				EA::StdC::Memcpy(ShaderCompileResult.ShaderHash.Value + 1, DxcShaderHashBuffer->HashDigest + sizeof(ShaderCompileResult.ShaderHash.Value[0]), sizeof(ShaderCompileResult.ShaderHash.Value[0]));
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

			eastl::string8 ErrorStr{ reinterpret_cast<const char8_t*>(ErrorBuffer->GetBufferPointer()), ErrorBuffer->GetBufferSize() };
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
	}

	return ShaderCompileResult;
}

