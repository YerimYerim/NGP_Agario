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
HANDLE CollideEvent; // Collide ������


// �̺�Ʈ
HANDLE MainEvent; // main �̺�Ʈ


// Ŭ���̾�Ʈ�� ������ ��� ������
DWORD WINAPI Client_Thread(LPVOID arg)
{
	// ���� ����
	int retval;
	SOCKET client_sock = (SOCKET)arg;
	int Player_id; // �÷��̾� ���̵�
	int rid;


	SOCKADDR_IN clientaddr;
	int addrlen;

	// Ŭ���̾�Ʈ ������ ���.
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	// �÷��̾� ��ȣ�� ������� ����
	retval = send(client_sock, (char*)&rid, sizeof(rid), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send() - Player_id");
		exit(1);
	}

	/*
	// �÷��̾��� �ʱ� ��ġ
	retval = send(client_sock, (char*));
	if (retval == SOCKET_ERROR) {
		err_display("send()_player_pos");
		exit(1);
	}

	while (1)
	{
		// �÷��̾ ���� �Ҷ� ���� ��ٸ���
		retval = WaitForSingleObject(hMainEvent, INFINITY);
		if (retval == WAIT_OBJECT_0)
			return 1;

		// �ÿ��̾� ������ ����
		retval = recvn(client_sock, (char*)&r);
		if (retval == SOCKET_ERROR) {
			err_display("recvn() Playerinfo error");
			break;
		}

		SetEvent(h)


		// �浹 ������ ����� ��� �̷�� ���� ���� ��ٸ���
		retval = WaitForSingleObject(hCollideEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			return 0;


		// ���� ���̸� �����Ѵ�.
		retval = send(client_sock, (char*)&my_fg, sizeof(my_fg), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		// ó���� ��� ���� ����
		retval = send(client_sock, );
		if (retval == SOCKET_ERROR)
		{
			err_display("send() �����error");
			break;
		}

		// 





	}

	*/

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

	/*
	for (int i = 0; i < 3; i++) {
		CollideEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

	}
	*/


	// �÷��̾ ��� ����� ������ ���� �̺�Ʈ
	MainEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (MainEvent == NULL)
		return 1;
	




	// ������ ����
	// �浹 ������

	// ���� ������
	while (true){
		
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
				SetEvent(MainEvent); // ���� �̺�Ʈ ȣ��
			}

			CloseHandle(hThread);
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