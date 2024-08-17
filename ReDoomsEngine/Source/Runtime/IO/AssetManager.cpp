#include "AssetManager.h"
#include "WindowsApplication.h"
#include <Windows.h>

const eastl::wstring& FAssetManager::GetAssetFolderDirectory()
{
    static eastl::wstring AssetFolderDirectory{};
    if (AssetFolderDirectory.empty())
    {
		#if RD_EDITOR
		AssetFolderDirectory += EA_WCHAR(VS_SOLUTION_DIR);
		#else
		AssetFolderDirectory += FWindowsApplication::GetExecutableDirectory();
		#endif
        AssetFolderDirectory += EA_WCHAR("Assets/");
    }
    return AssetFolderDirectory;
}

const eastl::wstring& FAssetManager::GetShaderFolderDirectory()
{
    static eastl::wstring ShaderFolderDirectory{};
    if (ShaderFolderDirectory.empty())
    {
        ShaderFolderDirectory += GetAssetFolderDirectory();
        ShaderFolderDirectory += EA_WCHAR("Shader/");
    }
    return ShaderFolderDirectory;
}

eastl::wstring FAssetManager::MakeAbsolutePathFromShaderFolder(const eastl::wstring& InRelativePath)
{
	eastl::wstring AbsolutePath{};
	AbsolutePath += FAssetManager::GetShaderFolderDirectory();
	AbsolutePath += InRelativePath;

	return AbsolutePath;
}

eastl::wstring FAssetManager::MakeAbsolutePathFromAssetFolder(const eastl::wstring& InRelativePath)
{
	eastl::wstring AbsolutePath{};
	AbsolutePath += FAssetManager::GetAssetFolderDirectory();
	AbsolutePath += InRelativePath;
    
    return AbsolutePath;
}

bool FAssetManager::SimpleReadEntireFile(const wchar_t* const FilePath, eastl::vector<uint8_t>& OutData)
{
    bool bIsSuccess = false;

    HANDLE FileHandle = CreateFileW(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        const DWORD FileSize = GetFileSize(FileHandle, NULL);
        OutData.resize(FileSize);
        if (ReadFile(FileHandle, OutData.data(), FileSize, NULL, NULL))
        {
            bIsSuccess = true;
        }
        else
        {
            EA_ASSERT_FORMATTED(false, ("\"ReadFile\" Fail ErrorCode : %d", GetLastError()));
        }

        CloseHandle(FileHandle);
    }
    else
    {
        EA_ASSERT_FORMATTED(false, ("\"CreateFileW\" Fail ErrorCode : %d", GetLastError()));
    }

    return bIsSuccess;
}