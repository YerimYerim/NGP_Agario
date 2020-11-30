#pragma	comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include"Global.h"

#define SERVERPORT 9000
#define BUFSIZE 1024
#define N_Player 2

using namespace std::chrono;
using namespace std;

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


// 변수
HANDLE Collide; // Collide 쓰레드
short Client_id[N_Player]; 
short ClientConnet; // 이건 삭제 할수도 있음
int	Pdead_num; // 죽은 플레이어 수
RecvDirection recvDirection;
SOCKET sock; // 소켓
Player player;

// 이벤트
HANDLE MainEvent; // main 이벤트
HANDLE CollideEvent[2]; // 충돌 이벤트 2명
vector<HANDLE> ClientEvent;


// 클라이언트와 데이터 통신 쓰레드
DWORD WINAPI Client_Thread(LPVOID arg)
{
	// 소켓 생성
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	unsigned short Player_id; // 플레이어 아이디
	unsigned short rid = N_Client - 1; // 클라 순서 1부터

	SOCKADDR_IN clientaddr;
	int addrlen;


	// 클라이언트 정보를 얻다.
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	// 플레이어 번호를 순서대로 보냄
	retval = send(client_sock, (char*)&rid, sizeof(rid), 0);
	cout << retval << endl;
	if (retval == SOCKET_ERROR)
	{
		err_display("send() - Player_id");
		exit(1);
	}

	// 플레이어의 초기 위치나 현재 맵 상태
	/*
	retval = send(client_sock, (char*)&Player_Start, sizeof(Player_Start), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()_player_pos");
		exit(1);
	}
	*/
	while (1)
	{
		// 플레이어가 접속 할때 까지 기다리자
		/*
		retval = WaitForSingleObject(MainEvent, INFINITY);
		if (retval == WAIT_OBJECT_0)
			return 1;
			*/
		Player_id = Client_id[N_Player];

		// 키보드 입력을 받는다.
		retval = recvn(sock, (char*)&recvDirection, sizeof(recvDirection), 0);
		cout << recvDirection.dir << endl;
		if (retval == SOCKET_ERROR) {
			err_display("recvn() - key input");
			exit(1);
		}

		// 키입력 저장
		recvDirection.id = Player_info[rid].id;
		recvDirection.dir = Player_info[rid].dir;

		cout << Player_info[rid].dir << endl;

		// 이벤트 함수로 정지 신호를 설정
		//SetEvent(ClientEvent[Client_id[rid]]);

		/**
		// 충돌 쓰레드 계산이 모두 이루어 질때 까지 기다리자
		retval = WaitForSingleObject(CollideEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			return 0;
			*/

			/*
			// 처리된 결과 값을 전송
			retval = send(client_sock, );
			if (retval == SOCKET_ERROR)
			{
				err_display("send() 결과값error");
				break;
			}
			*/
			// 

	}

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
	bool Standyscreen = false;

	/*
	// 충돌 이벤트 생성
	for (int i = 0; i < 2; i++) {
		CollideEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (CollideEvent[i] == NULL)
			return 1;
	}
	*/

	// 플레이어가 모두 입장시 쓰레드 시작 이벤트 2명
	MainEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (MainEvent == NULL)
		return 1;

	ClientEvent.reserve(4); // 클라이언트 함수 크기 할당
	for (int i = 0; i < N_Player; ++i){
		ClientEvent.emplace_back(); // 연결시 이벤트 함수 추가

		ClientEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (ClientEvent[i] == NULL)
			return 1;
	}

	/*
	// 쓰레드 생성
	// 충돌 쓰레드
	Collide = CreateThread(NULL, 0, Collide_Thread, 0, 0, NULL);
	if (Collide != NULL) {
		CloseHandle(Collide);
	}
	*/

	// 메인 쓰레드
	while (1){
		if (Standyscreen == FALSE) {
			if (flag == GAME_OVER) {

				Standyscreen = TRUE; // 한번 만 실행
				GameEnd = FALSE;
				ResetEvent(MainEvent);
				N_Client = 0; //클라이언트 접속
				
			}
		}

		if (GameEnd == FALSE)
		{
			// 2명 이상일때 게임 상태가 TRUE로 

			// accept
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET)
			{
				err_display("accept()");
				break;
			}

			// Nagle 알고리즘 해제
			int opt_val = TRUE; // FALSE : Nagle 알고리즘 설정
			setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));

			// 접속한 클라이언트 정보 출력
			printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));


			// 클라이언트 스레드
			hThread = CreateThread(NULL, 0, Client_Thread, (LPVOID)client_sock, 0, NULL);
			if (hThread == NULL)
				closesocket(client_sock);
			else
			{
				N_Client++; // 클라이언트 갯수

				if (N_Client == 2)
				{
					GameEnd = TRUE;
					Standyscreen = FALSE;
					SetEvent(MainEvent); // 메인 이벤트 호출
				}

				CloseHandle(hThread);
			}
		}
	}

	// closesocket()
	closesocket(listen_sock);

	// 이벤트 종료

	CloseHandle(MainEvent);

	
	// 윈속 종료
	WSACleanup();
	
	return 0;
}

