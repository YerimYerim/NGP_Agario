﻿// NGP_Agario_Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#include "framework.h"
#include "NGP_Agario_Client.h"
#include "CircleObject.h"
#include "Player.h"
#include "Map.h"
#include "Global.h"
#include "ClientSend.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 30000


#define MAX_LOADSTRING 100

// 서버
SOCKET sock; // 소켓
HANDLE MainEvent; // 이벤트
unsigned short Player_id; // 플레이어 아디
HANDLE ProtocolThread;
SendDirection sendDirection;
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
CircleObject Circle;
Player player;
Map map;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


// 서버 사용
DWORD WINAPI Server_Thread(LPVOID arg);

void GameSendRoutine(int& length);

// 소켓함수 오류 출력 후 종료
    void err_quit(const char* msg)
    {
        LPVOID IpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&IpMsgBuf, 0, NULL);
        MessageBox(NULL, (LPCTSTR)IpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
        LocalFree(IpMsgBuf);
        exit(-1);
    }

    // 소켓 함수 오류 출력
    void err_display(const char* msg)
    {
        LPVOID IpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&IpMsgBuf, 0, NULL);
        printf("[%s] %s", msg, (LPCTSTR)IpMsgBuf);
        LocalFree(IpMsgBuf);
    }

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


    DWORD WINAPI Server_Thread(LPVOID arg)
    {
        int retval;
        //retval = send(sock, (char*)&Player_id, sizeof(Player_id), 0);
        //if (retval == INVALID_SOCKET) {
        //    err_display((char*)"send()");
        //}
        mapPack temp;
        int length;
        
        player.SetRandomPosition();

        while (1)
        {

            sendDirection.id = Player_id;
            sendDirection.x = player.position.x;
            sendDirection.y = player.position.y;
            retval = send(sock, (char*)&sendDirection, sizeof(sendDirection), 0);
            if (retval == SOCKET_ERROR) {
                err_display((char*)"send()");
                exit(1);
            }
            retval = recv(sock, (char*)&temp, sizeof(temp), 0);
            if (retval == SOCKET_ERROR) {
                err_display((char*)"send()");
                exit(1);
            }
            map.Set(temp);
            map.Update();

        }
        return 0;
    }

  

   

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NGPAGARIOCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NGPAGARIOCLIENT));

    MSG msg;

    // 기본 메시지 루프입니다:
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
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NGPAGARIOCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NGPAGARIOCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
   char severIP[100];
   std::cin >> severIP;
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 200, 800, 800, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   int retval;
   // 윈속 초기화
   WSADATA wsa;
   if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
       return 1;
   // socket()
   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock == INVALID_SOCKET) err_quit((char*)"socket()");
   // connect()
   SOCKADDR_IN serveraddr; //서버와 통신용 소켓
   ZeroMemory(&serveraddr, sizeof(serveraddr));
   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = inet_addr(severIP);
   serveraddr.sin_port = htons(SERVERPORT);
   retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

   if (retval == SOCKET_ERROR) err_quit((char*)"connect()");

   // 데이터 통신에 사용할 변수
   //retval = send(sock, (char*)&Player_id, sizeof(Player_id), 0); 
   //if (retval == SOCKET_ERROR) {
   //    err_display((char*)"send()");
   //    exit(1);
   //}
   retval = recv(sock, (char*)&Player_id, sizeof(Player_id), 0);
   if (retval == SOCKET_ERROR) {
       err_display((char*)"recv()");
       exit(1);
   }
   retval = send(sock, (char*)&Player_id, sizeof(Player_id), 0);
   mapPack temp;
   // 받는곳
   retval = recv(sock, (char*)&temp, sizeof(temp), 0);
   map.Set(temp);
   ProtocolThread = CreateThread(NULL, 0, Server_Thread, (LPVOID)sock, 0, NULL);
   //cout << "맵 크기~~?"  << sizeof(map) << endl;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
        SetTimer(hWnd, 1, 100, NULL);

        sendDirection.id = 1;
        break;
    case WM_PAINT:
        PAINTSTRUCT ps;
        static HDC hdc, MemDC;
        static HBITMAP BackBit, oldBackBit;
        static RECT bufferRT;
        MemDC = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &bufferRT);
        hdc = CreateCompatibleDC(MemDC);
        BackBit = CreateCompatibleBitmap(MemDC, bufferRT.right, bufferRT.bottom);
        oldBackBit = (HBITMAP)SelectObject(hdc, BackBit);
        PatBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
        // draw 하는부분
        map.Draw(hdc);
        if (map.GameEnd())
        {
            int id;
            int score = 0;
            char buff[80];
            for (int i = 0; i < map.PlayerNum; ++i)
            {

                if (score < map.player[i].GetScore())
                {
                    score = map.player[i].GetScore();
                    id = i;
                }
            }
            sprintf(buff, "%d is WINNER %d",id);
            TextOut(hdc, 0, 0, buff, sizeof(buff) / sizeof(char));
            TextOut(hdc, 0, 20, "WAITING FOR SEVER RESTART", 25);
        }
        GetClientRect(hWnd, &bufferRT);
        BitBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, hdc, 0, 0, SRCCOPY);
        SelectObject(hdc, oldBackBit);
        DeleteObject(BackBit);
        DeleteDC(hdc);
        EndPaint(hWnd, &ps);
        break;
    case WM_KEYDOWN:
    {
        // 여기에서 센드 메시지 하면 될듯
        //KeyboardSend
        if (wParam == VK_ESCAPE)
        {
            exit(1);
        }

    }
    break;
    case WM_TIMER:
    {
        if (!map.GameEnd())
        {
            if (GetAsyncKeyState(VK_RIGHT) < 0 || GetAsyncKeyState(VK_LEFT) < 0)
                player.HorizontalMove(player.HorizontalInput(wParam));
            if (GetAsyncKeyState(VK_UP) < 0 || GetAsyncKeyState(VK_DOWN) < 0)
                player.VerticalMove(player.VerticalInput(wParam));
        }
        map.Update();
        InvalidateRect(hWnd, NULL, FALSE);
    }break;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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

