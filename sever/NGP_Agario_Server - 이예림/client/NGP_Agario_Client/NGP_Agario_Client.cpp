// NGP_Agario_Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#include "framework.h"
#include "NGP_Agario_Client.h"
#include "CircleObject.h"
#include "Player.h"
#include "Map.h"
#include"Global.h"
#include "ClientSend.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 30000


#define MAX_LOADSTRING 100

// 서버
SOCKET sock; // 소켓
HANDLE MainEvent; // 이벤트
unsigned short Player_id; // 플레이어 아디
HANDLE FindNewPlayer;

SendDirection sendDirection;
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING] = { L"Sever" };                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
CircleObject Circle;
Player player;
Map map;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


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

// 플레이어 번호 보냄 -> 키입력을 받기위한 무한으로 도는 쓰레드 생성 -> 맵 업데이트를 받는 쓰레드 생성
DWORD WINAPI UpdateGame(LPVOID arg) {

    SOCKET client_sock = (SOCKET)arg;  //클라이언트 저장 소켓
    SOCKADDR_IN clientaddr; //클라이언트 주소 저장
    int addrlen;   //주소 길이
    char buf[BUFSIZE]; //전송하는 데이터
    int retval;
    unsigned int count;
    addrlen = sizeof(clientaddr);
    if (client_sock == INVALID_SOCKET) {
        err_display((char*)"accept()");
    }
    // 접속한 클라이언트 정보 출력
    //클라이언트로 부터 파일 기본 정보 받기
    SendDirection recvDirection;
    map.AddPlayer();

    std::string packet;
    const char* str = map.GetPacket(packet);
    int size = packet.length();
    retval = recv(client_sock, (char*)&Player_id, sizeof(Player_id), 0);
    map.Update();

    retval = send(client_sock, (char*)&size, sizeof(int), 0); // 사이즈 먼저 전송
    retval = send(client_sock, str, size, 0); // 파일의 네임과 크기가 있는 files 를 먼저 전송


    std::string pack = "";
    const char* Str;
    int Size;

    Sleep(100);
    while (!map.GameEnd())
    {
        pack = "";
        retval = recv(client_sock, (char*)&recvDirection, sizeof(recvDirection), 0);
        Position* p = new Position(recvDirection.x, recvDirection.y);
        p->SetPosition(recvDirection.x, recvDirection.y);
        map.player[Player_id].SetPosition(*p);
        map.Update();

        if (retval == SOCKET_ERROR) {
            err_display((char*)"recv()");
            exit(1);
        }
        
        Str = map.GetPacket(pack);
        Size = pack.length();
        
        retval = send(client_sock, (char*)&Size, sizeof(int), 0); // 사이즈 먼저 전송
        retval = send(client_sock, Str, Size, 0); // 파일의 네임과 크기가 있는 files 를 먼저 전송
        delete p;
    }
    return 0;
}
DWORD WINAPI FindNewClient(LPVOID listen_sock)
{
    SOCKET client_sock;  //클라이언트 저장 소켓
    SOCKADDR_IN clientaddr; //클라이언트 주소 저장
    int addrlen;   //주소 길이
    int sockNum = 0;
    SOCKET Listen_sock = (SOCKET)listen_sock;
    HANDLE hThread;

    while (1)
    {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(Listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
                err_display((char*)"accept()");
            break;
        }
        // 접속한 클라이언트 정보 출력
        int Player_id = 0;

        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        printf("%d 번째 클라이언트", sockNum);
        hThread = CreateThread(NULL, 0, UpdateGame, (LPVOID)client_sock, 0, NULL);
        if (hThread == NULL) { closesocket(client_sock); }
        else { CloseHandle(hThread); }
        ++sockNum;
    }
    closesocket(Listen_sock);
    // 윈속 종료
    WSACleanup();
    return 0;
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    MainEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (MainEvent == NULL)
        return 1;

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
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      500, 200, 800, 800, nullptr, nullptr, hInstance, nullptr);
   SetWindowText(hWnd, { "Sever" });

   if (!hWnd)
   {
      return FALSE;
   }

   int retval;
   //(파일 크기 / 버퍼 사이즈) 계산한 값을 while문으로 돌리기 위한 변수
   unsigned int count;
   // 윈속 초기화
   WSADATA wsa;
   if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
       return 1;
   // socket()
   SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
   if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");
   // bind()
   SOCKADDR_IN serveraddr;
   ZeroMemory(&serveraddr, sizeof(serveraddr));
   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
   serveraddr.sin_port = htons(SERVERPORT);
   retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
   if (retval == SOCKET_ERROR) err_quit((char*)"bind()");
   // listen()
   retval = listen(listen_sock, SOMAXCONN);
   if (retval == SOCKET_ERROR) err_quit((char*)"listen()");
   FindNewPlayer = CreateThread(NULL, 0, FindNewClient, (LPVOID)listen_sock, 0, NULL);

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
        GetClientRect(hWnd, &bufferRT);
        BitBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, hdc, 0, 0, SRCCOPY);
        SelectObject(hdc, oldBackBit);
        DeleteObject(BackBit);
        DeleteDC(hdc);
        EndPaint(hWnd, &ps);
        break;
    case WM_KEYDOWN:
    {
        //if (GetAsyncKeyState(VK_UP) < 0 || GetAsyncKeyState(VK_DOWN) < 0)
        //{
        //    /*sendDirection.dir = */map.player[0].VerticalInput(wParam);
        //    //          SendKeyBoardThreadVertical = CreateThread(NULL, 0, KeyboardSend, NULL, 0, NULL);
        //}
        //if (GetAsyncKeyState(VK_RIGHT) < 0 || GetAsyncKeyState(VK_LEFT) < 0)
        //{
        //    /*sendDirection.dir =*/ map.player[0].HorizontalInput(wParam);
        //    //            SendKeyBoardThreadHorizontal = CreateThread(NULL, 0, KeyboardSend, NULL, 0, NULL);
        //}
        if (wParam == VK_ESCAPE)
        {
            exit(1);
        }
    }
    break;
    case WM_TIMER:
    {
        if (GetAsyncKeyState(VK_RIGHT) < 0 || GetAsyncKeyState(VK_LEFT) < 0)
             map.player[0].HorizontalMove(map.player[0].HorizontalInput(wParam));
        if (GetAsyncKeyState(VK_UP) < 0 || GetAsyncKeyState(VK_DOWN) < 0)
             map.player[0].VerticalMove(map.player[0].VerticalInput(wParam));
        
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

