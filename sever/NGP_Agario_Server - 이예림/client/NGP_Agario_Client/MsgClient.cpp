#define _CRT_SECURE_NO_WARNINGS         // �ֽ� VC++ ������ �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

#define ID_EDITBOX 1000
#define ID_BUTTEN_OK 2000
#define ID_CANCEL 2001

// ��ȭ���� ���ν���
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(const char* fmt, ...);
// ���� ��� �Լ�
void err_quit(const char* msg);
void err_display(const char* msg);
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags);
// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID arg);
DWORD WINAPI ServerMain(LPVOID arg);

SOCKET sock; // ����
char buf[BUFSIZE + 1]; // ������ �ۼ��� ����
HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ
HWND hSendButton; // ������ ��ư
HWND hView_Msg,hMsg_Windows, hEdit2,hControl; // ���� ��Ʈ��
HINSTANCE hInst; // �ν��Ͻ� �ڵ�

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;
    WNDCLASS wndclass;
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "MyWndClass";
    if (!RegisterClass(&wndclass)) return 1;

    // ������ ����
    HWND hWnd = CreateWindow("MyWndClass", "ä�� Ŭ���̾�Ʈ", WS_OVERLAPPEDWINDOW,
        0, 0, 600, 600, NULL, NULL, hInstance, NULL);
    if (hWnd == NULL) return 1;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // �̺�Ʈ ����
    hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (hReadEvent == NULL) return 1;
    hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hWriteEvent == NULL) return 1;

    // ���� ��� ������ ����
    CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
    // �̺�Ʈ ����
    CloseHandle(hReadEvent);
    CloseHandle(hWriteEvent);

    // closesocket()
    closesocket(sock);

    // ���� ����
    WSACleanup();
    return 0;
}

// ��ȭ���� ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE:
        //TODO ��ư
        

        hView_Msg = CreateWindow("edit", NULL,
            WS_CHILD | WS_VISIBLE | WS_HSCROLL |
            WS_VSCROLL | ES_AUTOHSCROLL |
            ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
            0, 0, 0, 400, hWnd, (HMENU)100, hInst, NULL);

        hMsg_Windows = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
            10, 480, 430, 60, hWnd, (HMENU)ID_EDITBOX, NULL, NULL);


        hControl = CreateWindow(TEXT("button"), TEXT("������"), WS_CHILD |
            WS_VISIBLE | WS_BORDER, 445, 480, 120, 25, hWnd, (HMENU)ID_BUTTEN_OK, NULL, NULL);
        hControl = CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD |
            WS_VISIBLE | WS_BORDER, 445, 510, 120, 25, hWnd, (HMENU)ID_CANCEL, NULL, NULL);

 
          return 0;
       
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_BUTTEN_OK:
            EnableWindow(hSendButton, FALSE); // ������ ��ư ��Ȱ��ȭ
            WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
            GetDlgItemText(hWnd, ID_EDITBOX, buf, BUFSIZE + 1);
            SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
            SetFocus(hMsg_Windows);
            SendMessage(hMsg_Windows, EM_SETSEL, 0, -1);
            return TRUE;
        case ID_CANCEL:
            EndDialog(hWnd, ID_CANCEL);
            return TRUE;
        }
    case WM_SIZE:
        MoveWindow(hView_Msg, 0, 0, LOWORD(lParam), 400, TRUE);
        
        return 0;
    case WM_SETFOCUS:
        SetFocus(hMsg_Windows);
        
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// ���� ��Ʈ�� ��� �Լ�
void DisplayText(const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    char cbuf[BUFSIZE + 256];
    vsprintf(cbuf, fmt, arg);
 
    int nLength = GetWindowTextLength(hView_Msg);
    SendMessage(hView_Msg, EM_SETSEL, nLength, nLength);
    SendMessage(hView_Msg, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

    va_end(arg);
}

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// ���� �Լ� ���� ���
void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    DisplayText("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
}

// TCP Ŭ���̾�Ʈ ���� �κ�
DWORD WINAPI ClientMain(LPVOID arg)
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // connect()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");

    // ������ ������ ���
    while (1) {
        WaitForSingleObject(hWriteEvent, INFINITE); // ���� �Ϸ� ��ٸ���

        // ���ڿ� ���̰� 0�̸� ������ ����
        if (strlen(buf) == 0) {
            EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
            SetEvent(hReadEvent); // �б� �Ϸ� �˸���
            continue;
        }

        // ������ ������
        retval = send(sock, buf, strlen(buf), 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
        //DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\r\n", retval);

        // ������ �ޱ�
        retval = recvn(sock, buf, retval, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // ���� ������ ���
        buf[retval] = '\0';
        DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\r\n", retval);
        DisplayText("�����1: %s\r\n", buf);


        EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
        SetEvent(hReadEvent); // �б� �Ϸ� �˸���
    }

    return 0;
}

//������ ������ ���
DWORD WINAPI ServerMain(LPVOID arg)
{
    SOCKET server_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN serveraddr;
    int addrlen;
    char buf[BUFSIZE + 1];

    // Ŭ���̾�Ʈ ���� ���
    addrlen = sizeof(serveraddr);
    getpeername(server_sock, (SOCKADDR*)&serveraddr, &addrlen);

    while (1) {
        // ������ �ޱ�
        retval = recv(server_sock, buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // ���� ������ ���
        buf[retval] = '\0';
        DisplayText("[TCP/%s:%d] %s\r\n", inet_ntoa(serveraddr.sin_addr),
            ntohs(serveraddr.sin_port), buf);

        // ������ ������
        retval = send(server_sock, buf, retval, 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
    }

    // closesocket()
    closesocket(server_sock);
    DisplayText("���� ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\r\n",
        inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));

    return 0;
}