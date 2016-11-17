// DLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <Windowsx.h>
#include <string>
#include <fstream>
using namespace std;
#define EXPORT  __declspec(dllexport)

HHOOK hHook = NULL;
HINSTANCE hinstLib;
HWND ghWnd;
string content = "cbc...ccsjdf\nhjsaAB...sasaddefd\ndddeeeeffff...ff\ndfddfdf...dfdfd";
string buffer = "";
string tmpContent = "";
string sign = "...";
int numMove = 0;
int prevKey = -1;
int padding = 0;
int currentPos = -1;

void selectText(string &content, bool &isTheFirstSelectText, int padding)
{
	int n = sign.length();

	int posCursor = content.find(sign);

	if (posCursor < 0)
		return;

	for (int i = 0; i < posCursor+padding; i++)
	{
		keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, 0), 0, 0);
		keybd_event(VK_RIGHT, MapVirtualKey(VK_RIGHT, 0), KEYEVENTF_KEYUP, 0);
	}
	keybd_event(VK_SHIFT, 0, 0, 0);
	for (int i = 0; i < n; i++)
	{
		keybd_event(VK_RIGHT, 0, 0, 0);
		keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
	}
	keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);

	if (posCursor >= 0)
		content = content.substr(posCursor+n);
	else 
		content = "";

}

void replaceKeyByContent(string content, string key)
{
	int lenContent = content.length();
	for (int i = 0; i < key.length(); i++)
	{
				
		keybd_event(VK_BACK, 0, 0, 0);
		keybd_event(VK_BACK, 0, KEYEVENTF_KEYUP, 0);

	}
	for (int i = lenContent-1; i >= 0; i--)
	{
		if (content[i] <= 'Z' && content[i] >= 'A')
		{
			keybd_event(VK_SHIFT, 0, 0, 0);
			keybd_event(content[i], 0, 0, 0);
			keybd_event(content[i], 0, KEYEVENTF_KEYUP, 0);
			keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
		}
		else if (content[i] <= 'z' && content[i] >= 'a')
		{
			keybd_event(content[i] - 32, 0, 0, 0);
			keybd_event(content[i] - 32, 0, KEYEVENTF_KEYUP, 0);

		}
		else if (content[i] == '\n')
		{
			keybd_event(VK_RETURN, 0, 0, 0);
			keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
		}
		else if (content[i] == '.')
		{
			keybd_event(VK_OEM_PERIOD, 0, 0, 0);
			keybd_event(VK_OEM_PERIOD, 0, KEYEVENTF_KEYUP, 0);
		}
		else 
		{
			keybd_event(content[i], 0, 0, 0);
			keybd_event(content[i], 0, KEYEVENTF_KEYUP, 0);
		}

		keybd_event(VK_LEFT, 0, 0, 0);
		keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
	}
}


string getKey(string buffer)
{
	int len = buffer.length();
	for (int i = len - 1; i >= 0; i--)
		if (buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == '\n' || buffer[i] == VK_END)
			return buffer.substr(i+1);

	return buffer;
}

LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam) 
{

	string key = "";
	char s[255];
	size_t size;
	int posCursor = 0;
	int lenContent = 0;
	bool isTheFirstSelectText = false;

	KBDLLHOOKSTRUCT *keyHook = NULL;
	if (nCode < 0) // không xử lý message 
		return CallNextHookEx(hHook, nCode, wParam, lParam);


	if (wParam == WM_KEYDOWN)
	{
		
		keyHook = (KBDLLHOOKSTRUCT*)lParam;

		if (isalpha(keyHook->vkCode) 
			|| (keyHook->vkCode <= '9' && keyHook->vkCode >= '0')
			|| (keyHook->vkCode >= 0xBB && keyHook->vkCode <= 0xC0)
			|| (keyHook->vkCode >= 0xDB && keyHook->vkCode <= 0xDE) 
			|| keyHook->vkCode == 32 || keyHook->vkCode == VK_TAB || keyHook->vkCode == '\n'
			|| keyHook->vkCode == VK_END
			|| keyHook->vkCode == VK_BACK)
		{
				if (keyHook->vkCode != VK_BACK)
					buffer += keyHook->vkCode;
				else if (keyHook->vkCode == VK_BACK && buffer != "")
					buffer.erase(buffer.end()-1);
			
				padding = (prevKey == VK_TAB && keyHook->vkCode == VK_TAB);
				prevKey = keyHook->vkCode;
		}


		key = getKey(buffer);
		int k = 0;
		if (keyHook->vkCode == VK_RIGHT && key == "ABC")
		{

			currentPos = tmpContent.find(sign);
			if (currentPos == tmpContent.length()-1)
			{
				isTheFirstSelectText = true;
				tmpContent = content;
				currentPos = tmpContent.length()-1;
			}
			else
			{
				isTheFirstSelectText = false;
				tmpContent = content + tmpContent;
				currentPos = tmpContent.find(sign);

			}

			replaceKeyByContent(content, key);
			
			buffer = "";
			selectText(tmpContent, isTheFirstSelectText, padding);
			buffer += ' ';
			prevKey = VK_TAB;
			return 1;
		}

		k = tmpContent.find(sign);
		if (k < 0)
			return CallNextHookEx(hHook, nCode, wParam, lParam);

		if (keyHook->vkCode == VK_TAB && tmpContent.length())
		{
			prevKey = keyHook->vkCode;
			selectText(tmpContent, isTheFirstSelectText, padding); 
			return 1;
		}
	
	}
	
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}


	
EXPORT void _doInstallHook(HWND hWnd)
{
	if (hHook!=NULL) return;
	ghWnd = hWnd;
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) KeyHookProc, hinstLib, 0);
	if (hHook)
		MessageBox(hWnd, L"Setup hook successfully", L"Result", MB_OK);
	else
		MessageBox(hWnd, L"Setup hook fail", L"Result", MB_OK);
}

EXPORT void _doRemoveHook(HWND hWnd)
{
	if (hHook==NULL) return;
	UnhookWindowsHookEx(hHook);
	hHook = NULL;
	MessageBox(hWnd, L"Remove hook successfully", L"Result", MB_OK);
}