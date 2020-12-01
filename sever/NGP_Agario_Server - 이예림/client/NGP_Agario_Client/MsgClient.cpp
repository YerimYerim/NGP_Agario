#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
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

// 대화상자 프로시저
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// 편집 컨트롤 출력 함수
void DisplayText(const char* fmt, ...);
// 오류 출력 함수
void err_quit(const char* msg);
void err_display(const char* msg);
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags);
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);
DWORD WINAPI ServerMain(LPVOID arg);

SOCKET sock; // 소켓
char buf[BUFSIZE + 1]; // 데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트
HWND hSendButton; // 보내기 버튼
HWND hView_Msg,hMsg_Windows, hEdit2,hControl; // 편집 컨트롤
HINSTANCE hInst; // 인스턴스 핸들

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

    // 윈도우 생성
    HWND hWnd = CreateWindow("MyWndClass", "채팅 클라이언트", WS_OVERLAPPEDWINDOW,
        0, 0, 600, 600, NULL, NULL, hInstance, NULL);
    if (hWnd == NULL) return 1;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 이벤트 생성
    hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (hReadEvent == NULL) return 1;
    hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hWriteEvent == NULL) return 1;

    // 소켓 통신 스레드 생성
    CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
    // 이벤트 제거
    CloseHandle(hReadEvent);
    CloseHandle(hWriteEvent);

    // closesocket()
    closesocket(sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}

// 대화상자 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CREATE:
        //TODO 버튼
        

        hView_Msg = CreateWindow("edit", NULL,
            WS_CHILD | WS_VISIBLE | WS_HSCROLL |
            WS_VSCROLL | ES_AUTOHSCROLL |
            ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
            0, 0, 0, 400, hWnd, (HMENU)100, hInst, NULL);

        hMsg_Windows = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
            10, 480, 430, 60, hWnd, (HMENU)ID_EDITBOX, NULL, NULL);


        hControl = CreateWindow(TEXT("button"), TEXT("보내기"), WS_CHILD |
            WS_VISIBLE | WS_BORDER, 445, 480, 120, 25, hWnd, (HMENU)ID_BUTTEN_OK, NULL, NULL);
        hControl = CreateWindow(TEXT("button"), TEXT("종료"), WS_CHILD |
            WS_VISIBLE | WS_BORDER, 445, 510, 120, 25, hWnd, (HMENU)ID_CANCEL, NULL, NULL);

 
          return 0;
       
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_BUTTEN_OK:
            EnableWindow(hSendButton, FALSE); // 보내기 버튼 비활성화
            WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
            GetDlgItemText(hWnd, ID_EDITBOX, buf, BUFSIZE + 1);
            SetEvent(hWriteEvent); // 쓰기 완료 알리기
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

// 편집 컨트롤 출력 함수
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

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

// 사용자 정의 데이터 수신 함수
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

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
    int retval;

    // 윈속 초기화
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

    // 서버와 데이터 통신
    while (1) {
        WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기

        // 문자열 길이가 0이면 보내지 않음
        if (strlen(buf) == 0) {
            EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
            SetEvent(hReadEvent); // 읽기 완료 알리기
            continue;
        }

        // 데이터 보내기
        retval = send(sock, buf, strlen(buf), 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
        //DisplayText("[TCP 클라이언트] %d바이트를 보냈습니다.\r\n", retval);

        // 데이터 받기
        retval = recvn(sock, buf, retval, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // 받은 데이터 출력
        buf[retval] = '\0';
        DisplayText("[TCP 클라이언트] %d바이트를 받았습니다.\r\n", retval);
        DisplayText("사용자1: %s\r\n", buf);


        EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
        SetEvent(hReadEvent); // 읽기 완료 알리기
    }

    return 0;
}

//서버와 데이터 통신
DWORD WINAPI ServerMain(LPVOID arg)
{
    SOCKET server_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN serveraddr;
    int addrlen;
    char buf[BUFSIZE + 1];

    // 클라이언트 정보 얻기
    addrlen = sizeof(serveraddr);
    getpeername(server_sock, (SOCKADDR*)&serveraddr, &addrlen);

    while (1) {
        // 데이터 받기
        retval = recv(server_sock, buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // 받은 데이터 출력
        buf[retval] = '\0';
        DisplayText("[TCP/%s:%d] %s\r\n", inet_ntoa(serveraddr.sin_addr),
            ntohs(serveraddr.sin_port), buf);

        // 데이터 보내기
        retval = send(server_sock, buf, retval, 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
    }

    // closesocket()
    closesocket(server_sock);
    DisplayText("서버 종료: IP 주소=%s, 포트 번호=%d\r\n",
        inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));

    return 0;
}