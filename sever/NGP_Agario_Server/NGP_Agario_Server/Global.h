#pragma once
#include<stdlib.h>
#include<iostream>
#include<thread>
#include<vector>
#include<chrono>
#include<random>
#include"Pack_Struct.h"

#define GAME_OVER 1 // ���� ����

short N_Client = 0; // Ŭ���̾�Ʈ ���� ����
short flag = 2; // �÷��̾� ����
bool GameEnd = FALSE; // ���� ����

PlayerKeyInput Player_info[2]; // �÷��̾�� Ű���� ����	 // �÷��̾� ���� ��ġ
PlayerMap Player_Map;		 // �� ����



