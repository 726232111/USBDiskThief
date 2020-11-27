#include "ChoosePath.h"
#include "ChoosePath.h"
#include "pch.h"
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <new>
#include <objbase.h> 
#pragma comment(lib,"Comctl32.lib")	//TaskDialog
#pragma comment(lib,"shlwapi.lib")	//QueryInterface
#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


class CDialogEventHandler : public IFileDialogEvents,
	public IFileDialogControlEvents
{
public:
	// IUnknown methods
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] = {
			QITABENT(CDialogEventHandler, IFileDialogEvents),
			QITABENT(CDialogEventHandler, IFileDialogControlEvents),
			{ 0 },
#pragma warning(suppress:4838)
		};
		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		long cRef = InterlockedDecrement(&_cRef);
		if (!cRef)
			delete this;
		return cRef;
	}

	// IFileDialogEvents methods
	IFACEMETHODIMP OnFileOk(IFileDialog*) { return S_OK; };
	IFACEMETHODIMP OnFolderChange(IFileDialog*) { return S_OK; };
	IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*) { return S_OK; };
	IFACEMETHODIMP OnHelp(IFileDialog*) { return S_OK; };
	IFACEMETHODIMP OnSelectionChange(IFileDialog*) { return S_OK; };
	IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*) { return S_OK; };
	IFACEMETHODIMP OnTypeChange(IFileDialog* pfd) { return S_OK; };;
	IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*) { return S_OK; };

	// IFileDialogControlEvents methods
	IFACEMETHODIMP OnItemSelected(IFileDialogCustomize* pfdc, DWORD dwIDCtl, DWORD dwIDItem) { return S_OK; };;
	IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize*, DWORD) { return S_OK; };
	IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize*, DWORD, BOOL) { return S_OK; };
	IFACEMETHODIMP OnControlActivating(IFileDialogCustomize*, DWORD) { return S_OK; };

	CDialogEventHandler() : _cRef(1) { };
private:
	~CDialogEventHandler() { };
	long _cRef;
};





// Instance creation helper
HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void** ppv)
{
	*ppv = NULL;
	CDialogEventHandler* pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
	HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = pDialogEventHandler->QueryInterface(riid, ppv);
		pDialogEventHandler->Release();
	}
	return hr;
}

// This code snippet demonstrates how to work with the common file dialog interface
BOOL BasicPathChoose(TCHAR* SelectPath, rsize_t SelectPathSize)
{
	// CoCreate the File Open Dialog object.
	IFileDialog* pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		// Create an event handling object, and hook it up to the dialog.
		IFileDialogEvents* pfde = NULL;
		hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
		if (SUCCEEDED(hr))
		{
			// Hook up the event handler.
			DWORD dwCookie;
			hr = pfd->Advise(pfde, &dwCookie);
			if (SUCCEEDED(hr))
			{
				// Set the options on the dialog.
				DWORD dwFlags;

				// Before setting, always get the options first in order not to override existing options.
				hr = pfd->GetOptions(&dwFlags);
				if (SUCCEEDED(hr))
				{
					// In this case, get shell items only for file system items.
					// 根据提示更改样式就成了选择文件夹了
					hr = pfd->SetOptions(dwFlags | FOS_PICKFOLDERS);
					if (SUCCEEDED(hr))
					{
						hr = pfd->Show(NULL);
						if (SUCCEEDED(hr))
						{
							// Obtain the result, once the user clicks the 'Open' button.
							// The result is an IShellItem object.
							IShellItem* psiResult;
							hr = pfd->GetResult(&psiResult);
							if (SUCCEEDED(hr))
							{
								// We are just going to print out the name of the file for sample sake.
								PWSTR pszFilePath = NULL;
								hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
								if (SUCCEEDED(hr))
								{
									/*TaskDialog(NULL,
										NULL,
										L"CommonFileDialogApp",
										pszFilePath,
										NULL,
										TDCBF_OK_BUTTON,
										TD_INFORMATION_ICON,
										NULL);
									*/
									if (wcscpy_s(SelectPath, SelectPathSize, pszFilePath))
									{
										return FALSE;
									}
									CoTaskMemFree(pszFilePath);
								}
								psiResult->Release();
							}
						}
					}
				}
				// Unhook the event handler.
				pfd->Unadvise(dwCookie);
			}
			pfde->Release();
		}
		pfd->Release();
	}
	return SUCCEEDED(hr);
}


/*
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, INT nCmdShow)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		TASKDIALOGCONFIG taskDialogParams = { sizeof(taskDialogParams) };
		taskDialogParams.dwFlags = TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION;

		TASKDIALOG_BUTTON const buttons[] =
		{
			{ 1,                       L"Basic File Open" }
		};

		taskDialogParams.pButtons = buttons;
		taskDialogParams.cButtons = ARRAYSIZE(buttons);
		taskDialogParams.pszMainInstruction = L"Pick the file dialog sample you want to try";
		taskDialogParams.pszWindowTitle = L"Common File Dialog";

		while (SUCCEEDED(hr))
		{
			int selectedId;
			hr = TaskDialogIndirect(&taskDialogParams, &selectedId, NULL, NULL);
			if (SUCCEEDED(hr))
			{
				if (selectedId == IDCANCEL)
				{
					break;
				}
				BasicPathChoose();
			}
		}
		CoUninitialize();
	}
	return 0;


}
*/