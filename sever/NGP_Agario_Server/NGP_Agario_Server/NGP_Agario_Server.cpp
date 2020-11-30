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

// �����Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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


// ����
HANDLE Collide; // Collide ������
short Client_id[N_Player]; 
short ClientConnet; // �̰� ���� �Ҽ��� ����
int	Pdead_num; // ���� �÷��̾� ��
RecvDirection recvDirection;
SOCKET sock; // ����
Player player;

// �̺�Ʈ
HANDLE MainEvent; // main �̺�Ʈ
HANDLE CollideEvent[2]; // �浹 �̺�Ʈ 2��
vector<HANDLE> ClientEvent;


// Ŭ���̾�Ʈ�� ������ ��� ������
DWORD WINAPI Client_Thread(LPVOID arg)
{
	// ���� ����
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	unsigned short Player_id; // �÷��̾� ���̵�
	unsigned short rid = N_Client - 1; // Ŭ�� ���� 1����

	SOCKADDR_IN clientaddr;
	int addrlen;


	// Ŭ���̾�Ʈ ������ ���.
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	// �÷��̾� ��ȣ�� ������� ����
	retval = send(client_sock, (char*)&rid, sizeof(rid), 0);
	cout << retval << endl;
	if (retval == SOCKET_ERROR)
	{
		err_display("send() - Player_id");
		exit(1);
	}

	// �÷��̾��� �ʱ� ��ġ�� ���� �� ����
	/*
	retval = send(client_sock, (char*)&Player_Start, sizeof(Player_Start), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()_player_pos");
		exit(1);
	}
	*/
	while (1)
	{
		// �÷��̾ ���� �Ҷ� ���� ��ٸ���
		/*
		retval = WaitForSingleObject(MainEvent, INFINITY);
		if (retval == WAIT_OBJECT_0)
			return 1;
			*/
		Player_id = Client_id[N_Player];

		// Ű���� �Է��� �޴´�.
		retval = recvn(sock, (char*)&recvDirection, sizeof(recvDirection), 0);
		cout << recvDirection.dir << endl;
		if (retval == SOCKET_ERROR) {
			err_display("recvn() - key input");
			exit(1);
		}

		// Ű�Է� ����
		recvDirection.id = Player_info[rid].id;
		recvDirection.dir = Player_info[rid].dir;

		cout << Player_info[rid].dir << endl;

		// �̺�Ʈ �Լ��� ���� ��ȣ�� ����
		//SetEvent(ClientEvent[Client_id[rid]]);

		/**
		// �浹 ������ ����� ��� �̷�� ���� ���� ��ٸ���
		retval = WaitForSingleObject(CollideEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			return 0;
			*/

			/*
			// ó���� ��� ���� ����
			retval = send(client_sock, );
			if (retval == SOCKET_ERROR)
			{
				err_display("send() �����error");
				break;
			}
			*/
			// 

	}

	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

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
	// �ð� ���� ���߿� �˸��� ��ġ�� �ű�� �ȴ�.
	auto start_time = high_resolution_clock::now();

	auto end_time = high_resolution_clock::now();
	auto exec_time = end_time - start_time;
	int exec_ms = duration_cast<seconds>(exec_time).count();


	// �ð� ���
	cout << exec_ms << "s" << endl;
	*/

	// ���� �ʱ�ȭ
	int retval;

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread; 
	bool Standyscreen = false;

	/*
	// �浹 �̺�Ʈ ����
	for (int i = 0; i < 2; i++) {
		CollideEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (CollideEvent[i] == NULL)
			return 1;
	}
	*/

	// �÷��̾ ��� ����� ������ ���� �̺�Ʈ 2��
	MainEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (MainEvent == NULL)
		return 1;

	ClientEvent.reserve(4); // Ŭ���̾�Ʈ �Լ� ũ�� �Ҵ�
	for (int i = 0; i < N_Player; ++i){
		ClientEvent.emplace_back(); // ����� �̺�Ʈ �Լ� �߰�

		ClientEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (ClientEvent[i] == NULL)
			return 1;
	}

	/*
	// ������ ����
	// �浹 ������
	Collide = CreateThread(NULL, 0, Collide_Thread, 0, 0, NULL);
	if (Collide != NULL) {
		CloseHandle(Collide);
	}
	*/

	// ���� ������
	while (1){
		if (Standyscreen == FALSE) {
			if (flag == GAME_OVER) {

				Standyscreen = TRUE; // �ѹ� �� ����
				GameEnd = FALSE;
				ResetEvent(MainEvent);
				N_Client = 0; //Ŭ���̾�Ʈ ����
				
			}
		}

		if (GameEnd == FALSE)
		{
			// 2�� �̻��϶� ���� ���°� TRUE�� 

			// accept
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET)
			{
				err_display("accept()");
				break;
			}

			// Nagle �˰��� ����
			int opt_val = TRUE; // FALSE : Nagle �˰��� ����
			setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));

			// ������ Ŭ���̾�Ʈ ���� ���
			printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));


			// Ŭ���̾�Ʈ ������
			hThread = CreateThread(NULL, 0, Client_Thread, (LPVOID)client_sock, 0, NULL);
			if (hThread == NULL)
				closesocket(client_sock);
			else
			{
				N_Client++; // Ŭ���̾�Ʈ ����

				if (N_Client == 2)
				{
					GameEnd = TRUE;
					Standyscreen = FALSE;
					SetEvent(MainEvent); // ���� �̺�Ʈ ȣ��
				}

				CloseHandle(hThread);
			}
		}
	}

	// closesocket()
	closesocket(listen_sock);

	// �̺�Ʈ ����

	CloseHandle(MainEvent);

	
	// ���� ����
	WSACleanup();
	
	return 0;
}

