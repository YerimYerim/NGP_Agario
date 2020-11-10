#include <windows.h>
#include <iostream>
#include <string>
#include "resource.h"
#pragma warning(disable:4996)
HINSTANCE g_hInst;
LPCTSTR lpszClass = "Window Class Name";
LPCTSTR lpszWindowName = "Window Programming Lab";
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Dlg6_1Proc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;

	g_hInst = hInstance;
	// 윈도우 클래스 구조체 값 설정
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	// 윈도우 클래스 등록
	RegisterClassEx(&WndClass);
	// 윈도우 생성
	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);
	// 윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	// 이벤트 루프 처리
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (iMsg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//-- 이곳에서 dc를 이용하여 출력이 이루어짐
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN: // 마우스 클릭하면 대화상자 띄우기
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dlg6_1Proc);
		break;
	case WM_TIMER:
		hdc = GetDC(hWnd);
		//-- dc를 이용하여 출력 수행
		ReleaseDC(hWnd, hdc);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam); // 위의 세 메시지 외의 나머지 메시지는 OS로
}

BOOL CALLBACK Dlg6_1Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	char msg[67];
	char newline[5] = "\n";
	
	//SendMessage(hDlg, LB_ADDSTRING, 0, (LPARAM)msg);
	//int ncount=(int)SendMessage(hDlg, LB_GETCOUNT, 0, 0);
	//SendMessage(hDlg, LB_SETTOPINDEX, ncount - 1, 0);
	switch (iMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT2, msg, 100); // 문자열 가져오기
			strcat(msg, newline);
			SetDlgItemText(hDlg, IDC_STATIC1, msg); // 문자열 출력
			SetDlgItemText(hDlg, IDC_EDIT2, " ");
			break;
		case IDC_BUTTON1:
			EndDialog(hDlg, 0);
		case WM_QUIT:
			return 0;
		}
		break;
	}

	return 0;
}