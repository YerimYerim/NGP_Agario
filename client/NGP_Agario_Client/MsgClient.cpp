#include <windows.h>
#include <stdio.h>
#include <process.h>
#include "resource.h"
#pragma warning(disable:4996)

SOCKET g_ClinetSocket = NULL;
HWND g_hwndName;
HWND g_hwndIP;
HWND g_hwndEdit;
HWND g_hwndList;
HWND hWnd;
HWND hWndFocus;

char servIP[15];
char Name[20];
char NameStr[128];
char str[128];
BOOL SEND = FALSE;

HANDLE hThread1, hThread2;
DWORD dwThreadID1, dwThreadID2;

unsigned int __stdcall SendMSG(void* arg);
unsigned int __stdcall RecvMSG(void* arg);

BOOL CALLBACK DIgProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM IParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dlg6_1Proc);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_INITDIALOG: OnInitDiaglog(hWnd, hWnd, lParam);
    case WM_COMMAND: OnCommand(hWnd, wParam); retunr TRUE;
    case WM_CLOSE: OnClose(hWnd);
    }
}

void OnCommand(HWND hwnd, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case IDC_CONNECTBUTTON: OnConnect(hwnd); break;
    case IDC_EXITBUTTON: OnDisConnnect(); break;
    case IDC_SENDBUTTON: OnSend(hwnd); break;
    }
}

void OnClose(HWND hWnd) //종료버튼 처리
{
    closesocket(g_ClinetSocket);
}

void AddStringToList(HWND g_hwndList, char* szltem)
{
    SendMessage(g_hwndList, LB_ADDSTRING, 0, (LPARAM)szltem);
    int nCount = (int)SendMessage(g_hwndList, LB_GETCOUNT, 0, 0);
    SendMessage(g_hwndList, LB_SETTOPINDEX, nCount - 1, 0);
}

//대화상자 초기화
BOOL OnInitDiaglog(HWND hWnd, HWND hWndFocus, LPARAM IParam)
{
    //윈도우 핸들 구하기
    g_hwndName = GetDlgItem(hWnd, IDC_IDEDIT);
    g_hwndEdit = GetDlgItem(hWnd, IDC_CHATEDIT);
    g_hwndList = GetDlgItem(hWnd, IDC_CHATLIST);

    AddStringToList(g_hwndList, "IP입력후 연결클릭");
    return TRUE;
}

void OnConnect(HWND hwnd)
{
    GetDlgItemText(hwnd, IDC_IDEDIT, Name, 20);
    //주소 생성
    GetDlgItemText(hwnd, IDC_IPEDIT, servIP, 15);

    //소켓 생성
    g_ClinetSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (g_ClinetSocket == INVALID_SOCKET) AddStringToList(g_hwndList, "Socket Error");

    //서버 주소 초기화
    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    //여기 수정해야 할듯
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.S_un.S_addr = inet_addr(servIP);
    servAddr.sin_port = htons(atoi("9000"));
    if (connect(g_ClinetSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        AddStringToList(g_hwndList, "서버와연결이 끊겼습니다.");
}

//종료
void OnDisConnect()
{
    closesocket(g_ClinetSocket);
    AddStringToList(g_hwndList, "서버와 연결이 종료되었습니다.");
}

//메세지 전송
void OnSend(HWND hwnd) //송신 쓰레드
{
    //데이터 전송
    GetDlgItemText(hwnd, IDC_CHATEDIT, str, sizeof(str));

    //전송문자 삭제
    SetDlgItemText(hwnd, IDC_CHATEDIT, "");
    SetFocus(GetDlgItem(hwnd, IDC_CHATEDIT));

    SEND = TRUE;
    //쓰레드에서 메세지 전송
}

unsigned int __stdcall SendMSG(void* arg)
{
    while (1)
    {
        if (SEND) //시지 전송?
        {
            sprintf(NameStr, "[%S] %S", Name, str);
            send(g_ClinetSocket, NameStr, (int)strlen(NameStr), 0);

            SEND = FALSE;
        }
        return 0;
    }
}

unsigned int __stdcall RecvMSG(void* arg)
{
    while (1)
    {
        int nRecv;
        nRecv = recv(g_ClinetSocket, NameStr, sizeof(NameStr) - 1, 0);
        if (nRecv == -1)
            return 1;
        NameStr[nRecv] = 0;

        if (nRecv > 0)
            AddStringToList(g_hwndList, NameStr);
        else
            AddStringToList(g_hwndList, "서버에 접속 되어있지 않습니다.");
    }
    return 0;
}