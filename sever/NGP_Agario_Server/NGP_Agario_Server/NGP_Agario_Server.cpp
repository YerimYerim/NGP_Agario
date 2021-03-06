#pragma	comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<stdlib.h>
#include<iostream>
#include<thread>
#include<vector>
#include<chrono>
#include<random>
#include"Map.h"

#define SERVERPORT 9000
#define BUFSIZE 1024
#define N_Player 3

using namespace std::chrono;
using namespace std;
Map map;
void err_quit(const char* msg)
{
	LPVOID IpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)IpMsgBuf,(LPCWSTR)msg, MB_ICONERROR);
	LocalFree(IpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID IpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (const char*)IpMsgBuf);
	LocalFree(IpMsgBuf);
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

// 변수
HANDLE CollideEvent; // Collide 쓰레드


// 이벤트
HANDLE MainEvent; // main 이벤트


// 클라이언트와 데이터 통신 쓰레드
DWORD WINAPI Client_Thread(LPVOID arg)
{
	// 소켓 생성
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	int Player_id; // 플레이어 아이디
	int rid;


	SOCKADDR_IN clientaddr;
	int addrlen;

	// 클라이언트 정보를 얻다.
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	// 플레이어 번호를 순서대로 보냄
	retval = send(client_sock, (char*)&rid, sizeof(rid), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send() - Player_id");
		exit(1);
	}

	/*
	// 플레이어의 초기 위치
	retval = send(client_sock, (char*));
	if (retval == SOCKET_ERROR) {
		err_display("send()_player_pos");
		exit(1);
	}

	while (1)
	{
		// 플레이어가 접속 할때 까지 기다리자
		retval = WaitForSingleObject(hMainEvent, INFINITY);
		if (retval == WAIT_OBJECT_0)
			return 1;

		// 플에이어 정보를 수신
		retval = recvn(client_sock, (char*)&r);
		if (retval == SOCKET_ERROR) {
			err_display("recvn() Playerinfo error");
			break;
		}

		SetEvent(h)


		// 충돌 쓰레드 계산이 모두 이루어 질때 까지 기다리자
		retval = WaitForSingleObject(hCollideEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			return 0;


		// 고정 길이를 전송한다.
		retval = send(client_sock, (char*)&my_fg, sizeof(my_fg), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		// 처리된 결과 값을 전송
		retval = send(client_sock, );
		if (retval == SOCKET_ERROR)
		{
			err_display("send() 결과값error");
			break;
		}

		// 





	}

	*/

	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));


	// closesocket()
	closesocket(client_sock);

	return 0;
}

/*
DWORD WINAPI Collide_Thread(LPVOID arg)
{

}
*/


int main()
{
	/*
	// 시간 측정 나중에 알맞은 위치에 옮기면 된다.
	auto start_time = high_resolution_clock::now();

	auto end_time = high_resolution_clock::now();
	auto exec_time = end_time - start_time;
	int exec_ms = duration_cast<seconds>(exec_time).count();


	// 시간 출력
	cout << exec_ms << "s" << endl;
	*/

	// 서버 초기화
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket() error");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR)
		err_quit("bind() error");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen() error");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	// 플레이어가 모두 입장시 쓰레드 시작 이벤트
	MainEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (MainEvent == NULL)
		return 1;



	// 윈속 종료
	WSACleanup();

	return 0;
}