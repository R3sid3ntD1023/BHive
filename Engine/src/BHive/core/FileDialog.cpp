#include "Application.h"
#include "FileDialog.h"

#include <commdlg.h>
#include <ShlObj.h>
#include <glfw/glfw3.h>
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

namespace BHive
{
	std::string FileDialogs::OpenFile(const char *filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = {0};
		CHAR currentDir[256] = {0};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow *)Application::Get().GetWindow().GetNative());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}

	std::string FileDialogs::SaveFile(const char *filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = {0};
		CHAR currentDir[256] = {0};
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow *)Application::Get().GetWindow().GetNative());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}

	std::string FileDialogs::GetDirectory()
	{
		std::string folder;

		auto hwnd = glfwGetWin32Window((GLFWwindow *)Application::Get().GetWindow().GetNative());

		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
											  COINIT_DISABLE_OLE1DDE);

		if (SUCCEEDED(hr))
		{
			IFileOpenDialog *pFileOpen;

			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
								  IID_IFileOpenDialog, (void **)&pFileOpen);

			if (SUCCEEDED(hr))
			{
				DWORD dialogoptions;
				hr = pFileOpen->GetOptions(&dialogoptions);
				if (SUCCEEDED(hr))
				{
					hr = pFileOpen->SetOptions(dialogoptions | FOS_PICKFOLDERS);
					hr = pFileOpen->Show(hwnd);

					if (SUCCEEDED(hr))
					{
						IShellItem *pItem;
						hr = pFileOpen->GetFolder(&pItem);
						if (SUCCEEDED(hr))
						{
							PWSTR pszFolder;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolder);

							if (SUCCEEDED(hr))
							{
								char str[512];
								wcstombs(str, pszFolder, 512);
								folder = str;

								CoTaskMemFree(pszFolder);
							}
						}
					}
				}

				pFileOpen->Release();
			}
		}

		CoUninitialize();

		return folder;
	}
}