#include <windows.h>

/* DLL �̃C���X�^���X �n���h�� */
extern HINSTANCE _hInstance;

/* �t�b�N�v���V�[�W���Ŏg�p����ϐ��͋��L�������ɂ��� */
/*                                             */
/* ����� Visual C++ �̏ꍇ��                    */
/* �����J�̃I�v�V������ /SECTION:.share,RWS ��ǉ� */
#pragma data_seg(".share")

HHOOK _hHook               = NULL;
HWND  _hWnd                = NULL;
int   _virtualKey          = 0;

#pragma data_seg()

/* ���[�U�[��`�̃��b�Z�[�W�̗�                           */
/* (WM_USER + 50) ���}�Ȃ� RegisterWindowMessage ���g�� */
#define WM_USERMESSAGE (WM_USER + 50)

/* �t�b�N �v���V�[�W�� */
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    /* �����ł� 'A' �������ꂽ��\�ߐݒ�ς݂� _hWnd �Ƀ��b�Z�[�W WM_USERMESSAGE �𑗂� */
    if (nCode >= 0 && nCode != HC_NOREMOVE && wParam == _virtualKey) {
        LPARAM isShiftKeyPressed   = (GetAsyncKeyState(VK_SHIFT  ) & 0x8000) ? (0x01 << 0) : 0x00;
        LPARAM isControlKeyPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? (0x01 << 1) : 0x00;
        LPARAM isAltKeyPressed     = (GetAsyncKeyState(VK_MENU   ) & 0x8000) ? (0x01 << 2) : 0x00;
        LPARAM keyStatus           = isShiftKeyPressed | isControlKeyPressed | isAltKeyPressed;

        PostMessage(_hWnd, WM_USERMESSAGE, wParam, keyStatus);
        /* �t�b�N���ꂽ�A�v���P�[�V�����̕��� 'A' �̓��͂��v��Ȃ��ꍇ�͉��L2�s��ǉ� */
        /* CallNextHookEx(_hHook, nCode, wParam, lParam);                   */
        /* return 1;                                                        */
    }
    return CallNextHookEx(_hHook, nCode, wParam, lParam);
}

/* �t�b�N�̃Z�b�g                             */
/* �L�[ 'A' �������ƁC�����œn�����E�B���h�E��    */
/* ���b�Z�[�W WM_USERMESSAGE ��������        */
/* ���̊֐��� export ���ăA�v���P�[�V��������Ă� */
BOOL __declspec(dllexport) Set(HWND hWnd, int virtualKey/*, BOOL shiftKeyIsPressed, BOOL controlKeyIsPressed, BOOL altKeyIsPressed*/)
{
    /* �L�[�̃t�b�N */
    _hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)HookProc, _hInstance, 0);
    /* ������ hInstance �� DLL �̃C���X�^���X �n���h�� */
    _hWnd = hWnd;
    _virtualKey = virtualKey;

    return (_hHook != NULL);
}

/* �A���t�b�N                                 */
/* �t�b�N���s�K�v�ɂȂ�����Ă�                  */
/* ���̊֐��� export ���ăA�v���P�[�V��������Ă� */
void __declspec(dllexport) Reset()
{
    if (_hHook != NULL) {
        UnhookWindowsHookEx(_hHook);
        _hHook = NULL;
    }
}
