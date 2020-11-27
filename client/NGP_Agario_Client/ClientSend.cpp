#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#pragma once
#define WIN32_LEAN_AND_MEAN             / / 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS

#define SERVERPORT 9000
#define BUFSIZE    24
// Windows 헤더 파일
#include <windows.h>
#include <iostream>
// C 런타임 헤더 파일입니다.
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
    Files files; // 파일의 사이즈와 이름 
    printf("%s", fileName);
    strcpy(files.name, fileName);
    //getchar(); //엔터 제거용 함수

    printf("%s", files.name);
    fp = fopen(files.name, "rb");
    printf("파일 오픈완료");
    if (fp == NULL) { printf("파일이 존재하지 않아용"); };

    //std::cout << "서버 아이피를 입력해주세요 : ";
    //td::cin >> SeverIp;

    //파일 끝으로 위치 옮김
    fseek(fp, 0L, SEEK_END);
    //파일 바이트값 출력
    files.byte = ftell(fp);
    //다시 파일 처음으로 위치 옮김
    fseek(fp, 0L, SEEK_SET);
    //send, recv 함수 출력값 저장용

    int retval;
    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;
    // socket()
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit((char*)"socket()");
    // connect()
    SOCKADDR_IN serveraddr; //서버와 통신용 소켓
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SeverIp);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

    if (retval == SOCKET_ERROR) err_quit((char*)"connect()");

    // 데이터 통신에 사용할 변수
    char buf[BUFSIZE]; //보낼 데이터를 저장할 공간
                       //파일 기본 정보 전송

    retval = send(sock, (char*)&files, sizeof(files), 0); // 파일의 네임과 크기가 있는 files 를 먼저 전송

    if (retval == SOCKET_ERROR) {
        err_display((char*)"send()");
        exit(1);
    }

    unsigned int per;
    per = count = files.byte / BUFSIZE; // 한번에 보낼 수 있는 양

    while (count)
    {
        printf("전송하는 파일 : %s, 전송하는 파일 크기 : %d Byte\n", files.name, files.byte);
        //파일 읽어서 파일 크기를 버퍼에 저장
        fread(buf, 1, BUFSIZE, fp); // buf 에 각원소의 크기가 1인 걸 BufSize 많큼 읽음
                                    //전송
        retval = send(sock, buf, BUFSIZE, 0); // bufsize 만큼 읽음
        if (retval == SOCKET_ERROR) {
            err_display((char*)"send()");
            exit(1);
        }
        printf("\n %s 의진행도 : %s %%", files.name, fileSendPer);
        itoa((per - count) * 100 / per, fileSendPer, 10);

        //InvalidateRgn(hWnd, NULL, TRUE);

        //system("cls");
        count--;
    }
    //남은 파일 크기만큼 나머지 전송 // BUFSIZE 보다 작음
    count = files.byte - (per * BUFSIZE);
    fread(buf, 1, count, fp);
    retval = send(sock, buf, BUFSIZE, 0);
    itoa(100, fileSendPer, 10);
    if (retval == SOCKET_ERROR) {
        err_display((char*)"send()");
        exit(1);
    }

    //system("cls");

    printf("전송하는 파일 : %s, 전송하는 파일 크기 : %d Byte\n", files.name, files.byte);
    printf("\n진행도 : 100 %%");
    printf("\n\n전송이 완료되었습니다.");
    // closesocket()
    closesocket(sock);
    // 윈속 종료
    WSACleanup();
    //파일포인터 닫기
    fclose(fp);

    return 0;
}