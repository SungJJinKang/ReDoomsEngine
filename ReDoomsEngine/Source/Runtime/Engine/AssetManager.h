#pragma once
#include "CommonInclude.h"

class FAssetManager : public EA::StdC::Singleton<FAssetManager>
{
public:

	static const eastl::wstring& GetAssetFolderDirectory();
	static const eastl::wstring& GetShaderFolderDirectory();
	static bool SimpleReadEntireFile(const const wchar_t* const FilePath, eastl::vector<uint8_t>& OutData);

private:


};

