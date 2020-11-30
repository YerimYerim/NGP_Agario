#pragma once
#include<stdlib.h>
#include<iostream>
#include<thread>
#include<vector>
#include<chrono>
#include<random>
#include"Pack_Struct.h"

#define GAME_OVER 1 // 게임 오버

short N_Client = 0; // 클라이언트 접속 순서
short flag = 2; // 플레이어 전송
bool GameEnd = FALSE; // 게임 상태

PlayerKeyInput Player_info[2]; // 플레이어들 키보드 정보	 // 플레이어 시작 위치
PlayerMap Player_Map;		 // 맵 정보



