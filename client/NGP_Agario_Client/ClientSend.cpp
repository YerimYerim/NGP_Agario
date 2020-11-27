#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#pragma once
#define WIN32_LEAN_AND_MEAN             / / ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS

#define SERVERPORT 9000
#define BUFSIZE    24
// Windows ��� ����
#include <windows.h>
#include <iostream>
// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "resource.h"
#include "framework.h"
#include "targetver.h"

#include <winsock2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void err_quit(char*);
void err_display(char*);
int recvn(SOCKET, char*, int, int);
DWORD WINAPI SendInput(LPVOID Name);
char SeverIp[100] = { NULL };
char fileSendPer[100] = { 0 };

typedef struct Files {
    char name[255];
    unsigned int byte;
} Files;


void err_display(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}



void err_quit(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf,(LPCTSTR)msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
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

DWORD WINAPI SendInput(LPVOID Key)
{
    unsigned int count;
    char* fileName = (char*)Key;
    FILE* fp;
    Files files; // ������ ������� �̸� 
    printf("%s", fileName);
    strcpy(files.name, fileName);
    //getchar(); //���� ���ſ� �Լ�

    printf("%s", files.name);
    fp = fopen(files.name, "rb");
    printf("���� ���¿Ϸ�");
    if (fp == NULL) { printf("������ �������� �ʾƿ�"); };

    //std::cout << "���� �����Ǹ� �Է����ּ��� : ";
    //td::cin >> SeverIp;

    //���� ������ ��ġ �ű�
    fseek(fp, 0L, SEEK_END);
    //���� ����Ʈ�� ���
    files.byte = ftell(fp);
    //�ٽ� ���� ó������ ��ġ �ű�
    fseek(fp, 0L, SEEK_SET);
    //send, recv �Լ� ��°� �����

    int retval;
    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;
    // socket()
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit((char*)"socket()");
    // connect()
    SOCKADDR_IN serveraddr; //������ ��ſ� ����
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SeverIp);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

    if (retval == SOCKET_ERROR) err_quit((char*)"connect()");

    // ������ ��ſ� ����� ����
    char buf[BUFSIZE]; //���� �����͸� ������ ����
                       //���� �⺻ ���� ����

    retval = send(sock, (char*)&files, sizeof(files), 0); // ������ ���Ӱ� ũ�Ⱑ �ִ� files �� ���� ����

    if (retval == SOCKET_ERROR) {
        err_display((char*)"send()");
        exit(1);
    }

    unsigned int per;
    per = count = files.byte / BUFSIZE; // �ѹ��� ���� �� �ִ� ��

    while (count)
    {
        printf("�����ϴ� ���� : %s, �����ϴ� ���� ũ�� : %d Byte\n", files.name, files.byte);
        //���� �о ���� ũ�⸦ ���ۿ� ����
        fread(buf, 1, BUFSIZE, fp); // buf �� �������� ũ�Ⱑ 1�� �� BufSize ��ŭ ����
                                    //����
        retval = send(sock, buf, BUFSIZE, 0); // bufsize ��ŭ ����
        if (retval == SOCKET_ERROR) {
            err_display((char*)"send()");
            exit(1);
        }
        printf("\n %s �����൵ : %s %%", files.name, fileSendPer);
        itoa((per - count) * 100 / per, fileSendPer, 10);

        //InvalidateRgn(hWnd, NULL, TRUE);

        //system("cls");
        count--;
    }
    //���� ���� ũ�⸸ŭ ������ ���� // BUFSIZE ���� ����
    count = files.byte - (per * BUFSIZE);
    fread(buf, 1, count, fp);
    retval = send(sock, buf, BUFSIZE, 0);
    itoa(100, fileSendPer, 10);
    if (retval == SOCKET_ERROR) {
        err_display((char*)"send()");
        exit(1);
    }

    //system("cls");

    printf("�����ϴ� ���� : %s, �����ϴ� ���� ũ�� : %d Byte\n", files.name, files.byte);
    printf("\n���൵ : 100 %%");
    printf("\n\n������ �Ϸ�Ǿ����ϴ�.");
    // closesocket()
    closesocket(sock);
    // ���� ����
    WSACleanup();
    //���������� �ݱ�
    fclose(fp);

    return 0;
}