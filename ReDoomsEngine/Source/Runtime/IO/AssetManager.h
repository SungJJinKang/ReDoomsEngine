#pragma once
#include "CommonInclude.h"

class FAssetManager : public EA::StdC::Singleton<FAssetManager>
{
public:

	static const eastl::wstring& GetAssetFolderDirectory();
	static const eastl::wstring& GetShaderFolderDirectory();
	static eastl::wstring MakeAbsolutePathFromShaderFolder(const eastl::wstring& InRelativePath);
	static eastl::wstring MakeAbsolutePathFromAssetFolder(const eastl::wstring& InRelativePath);
	static bool SimpleReadEntireFile(const const wchar_t* const FilePath, eastl::vector<uint8_t>& OutData);

private:


};

