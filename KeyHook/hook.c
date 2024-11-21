#include <windows.h>

/* DLL のインスタンス ハンドル */
extern HINSTANCE _hInstance;

/* フックプロシージャで使用する変数は共有メモリにおく */
/*                                             */
/* これは Visual C++ の場合で                    */
/* リンカのオプションに /SECTION:.share,RWS を追加 */
#pragma data_seg(".share")

HHOOK _hHook               = NULL;
HWND  _hWnd                = NULL;
int   _virtualKey          = 0;

#pragma data_seg()

/* ユーザー定義のメッセージの例                           */
/* (WM_USER + 50) が厭なら RegisterWindowMessage を使う */
#define WM_USERMESSAGE (WM_USER + 50)

/* フック プロシージャ */
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    /* 此処では 'A' が押されたら予め設定済みの _hWnd にメッセージ WM_USERMESSAGE を送る */
    if (nCode >= 0 && nCode != HC_NOREMOVE && wParam == _virtualKey) {
        LPARAM isShiftKeyPressed   = (GetAsyncKeyState(VK_SHIFT  ) & 0x8000) ? (0x01 << 0) : 0x00;
        LPARAM isControlKeyPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? (0x01 << 1) : 0x00;
        LPARAM isAltKeyPressed     = (GetAsyncKeyState(VK_MENU   ) & 0x8000) ? (0x01 << 2) : 0x00;
        LPARAM keyStatus           = isShiftKeyPressed | isControlKeyPressed | isAltKeyPressed;

        PostMessage(_hWnd, WM_USERMESSAGE, wParam, keyStatus);
        /* フックされたアプリケーションの方で 'A' の入力が要らない場合は下記2行を追加 */
        /* CallNextHookEx(_hHook, nCode, wParam, lParam);                   */
        /* return 1;                                                        */
    }
    return CallNextHookEx(_hHook, nCode, wParam, lParam);
}

/* フックのセット                             */
/* キー 'A' を押すと，ここで渡したウィンドウに    */
/* メッセージ WM_USERMESSAGE が送られる        */
/* この関数を export してアプリケーションから呼ぶ */
BOOL __declspec(dllexport) Set(HWND hWnd, int virtualKey/*, BOOL shiftKeyIsPressed, BOOL controlKeyIsPressed, BOOL altKeyIsPressed*/)
{
    /* キーのフック */
    _hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)HookProc, _hInstance, 0);
    /* ここで hInstance は DLL のインスタンス ハンドル */
    _hWnd = hWnd;
    _virtualKey = virtualKey;

    return (_hHook != NULL);
}

/* アンフック                                 */
/* フックが不必要になったら呼ぶ                  */
/* この関数を export してアプリケーションから呼ぶ */
void __declspec(dllexport) Reset()
{
    if (_hHook != NULL) {
        UnhookWindowsHookEx(_hHook);
        _hHook = NULL;
    }
}
