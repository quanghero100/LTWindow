// G3.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "G3.h"
#include <string>
#include <vector>
using namespace std;

#define MAX_LOADSTRING 100
#define MAX_LEN	1024000

// Global Variables:
HINSTANCE hInst;                                // current instance
HINSTANCE hinstLib = NULL;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


typedef bool(*INSERT_TEMPLATE)(string, string, string);
typedef bool(*DELETE_TEMPLATE)(string);
typedef int(*GET_NUMBER_TEMPLATE)();
typedef vector<vector<string>> (*GET_ALL_TEMPLATE)();

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AddTemplateProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ListTemplateProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void doInstallHook(HWND);
void doRemoveHook(HWND);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_G3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_G3));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_G3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_G3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		hinstLib = LoadLibrary(L"Database.dll");
		if (hinstLib == NULL) {
			MessageBox(hWnd, L"Khong load duoc Database.dll!", L"Thong bao", 0);
			return FALSE;
		}
		doInstallHook(hWnd);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case IDM_OPTION_ADDTEMPLATE:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ADD_TEMPLATE), hWnd, AddTemplateProc);
				break;
			case IDM_OPTION_LISTTEMPLATE:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_LIST_TEMPLATE), hWnd, ListTemplateProc);
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		doRemoveHook(hWnd);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// Message handler for add template box.
INT_PTR CALLBACK AddTemplateProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
	{
		int command = LOWORD(wParam);
		switch (command)
		{
		case IDOK:
		{
			WCHAR* short_code = new WCHAR[MAX_LEN];
			WCHAR* full_code = new WCHAR[MAX_LEN];
			WCHAR* description = new WCHAR[MAX_LEN];

			char* bsShortCode = new char[MAX_LEN];
			char* bsFullCode = new char[MAX_LEN];
			char* bsDescription = new char[MAX_LEN];

			HWND hShortCtl = GetDlgItem(hDlg, IDC_EDIT_SHORT_CODE);
			HWND hFullCtl = GetDlgItem(hDlg, IDC_EDIT_FULL_CODE);
			HWND hDescriptCtl = GetDlgItem(hDlg, IDC_EDIT_DESCRIPTION);

			SendMessage(hShortCtl, WM_GETTEXT, MAX_LEN, (LPARAM)short_code);
			SendMessage(hFullCtl, WM_GETTEXT, MAX_LEN, (LPARAM)full_code);
			SendMessage(hDescriptCtl, WM_GETTEXT, MAX_LEN, (LPARAM)description);

			INSERT_TEMPLATE insertTemplate = (INSERT_TEMPLATE)GetProcAddress(hinstLib, "insertTemplate");
			
			if (insertTemplate != NULL)
			{
				wcstombs(bsShortCode, short_code, MAX_LEN);
				wcstombs(bsFullCode, full_code, MAX_LEN);
				wcstombs(bsDescription, description, MAX_LEN);
				bool result = insertTemplate(bsShortCode, bsFullCode, bsDescription);
			}
			else {
				MessageBox(hDlg, L"Khong load duoc insertTemplate!", L"Thong bao", 0);
				return FALSE;
			}

			delete[]short_code;
			delete[]full_code;
			delete[]description;
			delete[]bsShortCode;
			delete[]bsFullCode;
			delete[]bsDescription;
			EndDialog(hDlg, LOWORD(wParam));
		}
		break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		default:
			break;
		}
	}
	break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for list template.
INT_PTR CALLBACK ListTemplateProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	static int numTemplate = 0;
	static vector<vector<string>> dataTemplate;
	static HWND hLbox = NULL;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		hLbox = GetDlgItem(hDlg, IDC_LB_LIST_TEMPLATE);
		
		GET_NUMBER_TEMPLATE getNumberTemplate = (GET_NUMBER_TEMPLATE)GetProcAddress(hinstLib, "getNumberTemplate");
		GET_ALL_TEMPLATE getAllTemplate = (GET_ALL_TEMPLATE)GetProcAddress(hinstLib, "getAllTemplate");

		if (getNumberTemplate != NULL && getAllTemplate != NULL)
		{
			numTemplate = getNumberTemplate();
			dataTemplate = getAllTemplate();

			for (int i = 0; i < numTemplate; i++) {
				WCHAR tmp[1000] = { 0 };
				mbstowcs(tmp, dataTemplate[i][0].c_str(), 1000);
				SendMessage(hLbox, LB_ADDSTRING, 0, (LPARAM)tmp);
			}
		}
		else {
			MessageBox(hDlg, L"Khong load duoc insertTemplate!", L"Thong bao", 0);
			return FALSE;
		}

		return (INT_PTR)TRUE;
	}
	break;
	case WM_COMMAND:
	{
		int command = LOWORD(wParam);
		switch (command)
		{
		case IDC_DELETE_TEMPLATE:
		{
			int itemIndex = (int)SendMessage(hLbox, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			int textLen = (int)SendMessage(hLbox, LB_GETTEXTLEN, (WPARAM)itemIndex, 0);
			WCHAR * textBuffer = new WCHAR[textLen + 1];
			SendMessage(hLbox, LB_GETTEXT, (WPARAM)itemIndex, (LPARAM)textBuffer);			

			DELETE_TEMPLATE deleteTemplate = (DELETE_TEMPLATE)GetProcAddress(hinstLib, "deleteTemplate");

			if (deleteTemplate != NULL)
			{
				char* temp = new char[MAX_LEN];
				wcstombs(temp, textBuffer, MAX_LEN);
				bool result = deleteTemplate(temp);
				SendMessage(hLbox, LB_DELETESTRING, itemIndex, 0);
			}
			else {
				MessageBox(hDlg, L"Khong load duoc insertTemplate!", L"Thong bao", 0);
				return FALSE;
			}

			delete[] textBuffer;
		}
		break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		default:
			break;
		}
	}
	break;
	}
	return (INT_PTR)FALSE;
}

void doInstallHook(HWND hWnd)
{
	// gọi hàm DLL theo kiểu Run-time
	// Định nghĩa prototype của hàm
	typedef VOID (*MYPROC)(HWND); 
	
	HINSTANCE hinstLib; 
	MYPROC ProcAddr; 
	
	// load DLL và lấy handle của DLL module
	hinstLib = LoadLibrary(L"G3-DLL.dll");
	// Nếu load thành công, lấy địa chỉ của hàm DrawCircle trong DLL
	if (hinstLib != NULL) { 
		ProcAddr = (MYPROC) GetProcAddress(hinstLib, "_doInstallHook"); 
		// Nếu lấy được địa chỉ hàm, gọi thực hiện hàm
		if (ProcAddr != NULL) 
			ProcAddr (hWnd);
	} 
	//_doInstallHook(hWnd);
}

void doRemoveHook(HWND hWnd)
{
	// gọi hàm DLL theo kiểu Run-time
	// Định nghĩa prototype của hàm
	typedef VOID (*MYPROC)(HWND); 
	
	HINSTANCE hinstLib; 
	MYPROC ProcAddr; 
	
	// load DLL và lấy handle của DLL module
	hinstLib = LoadLibrary(L"G3-DLL.dll");
	// Nếu load thành công, lấy địa chỉ của hàm DrawCircle trong DLL
	if (hinstLib != NULL) { 
		ProcAddr = (MYPROC) GetProcAddress(hinstLib, "_doRemoveHook"); 
		// Nếu lấy được địa chỉ hàm, gọi thực hiện hàm
		if (ProcAddr != NULL) 
			ProcAddr (hWnd);
	} 
	//_doRemoveHook(hWnd);
}