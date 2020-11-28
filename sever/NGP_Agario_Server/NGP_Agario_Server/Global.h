#pragma once
#include<stdlib.h>
#include<iostream>
#include<thread>
#include<vector>
#include<chrono>
#include<random>
#include"Pack_Struct.h"

short N_Client = 0; // 클라이언트 접속 순서
short flag = 2; // 플레이어 전송

PlayerKeyBoradInput Player_info; // 플레이어들 키보드 정보
PlayerStartInfo Player_Start;	 // 플레이어 시작 위치
PlayerMap Player_Map;		 // 맵 정보



