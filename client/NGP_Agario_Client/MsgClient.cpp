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
	// ������ Ŭ���� ����ü �� ����
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
	// ������ Ŭ���� ���
	RegisterClassEx(&WndClass);
	// ������ ����
	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);
	// ������ ���
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	// �̺�Ʈ ���� ó��
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
		//-- �̰����� dc�� �̿��Ͽ� ����� �̷����
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN: // ���콺 Ŭ���ϸ� ��ȭ���� ����
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dlg6_1Proc);
		break;
	case WM_TIMER:
		hdc = GetDC(hWnd);
		//-- dc�� �̿��Ͽ� ��� ����
		ReleaseDC(hWnd, hdc);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam); // ���� �� �޽��� ���� ������ �޽����� OS��
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
			GetDlgItemText(hDlg, IDC_EDIT2, msg, 100); // ���ڿ� ��������
			strcat(msg, newline);
			SetDlgItemText(hDlg, IDC_STATIC1, msg); // ���ڿ� ���
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