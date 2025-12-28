// tic_tac_tio.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "tic_tac_tio.h"
#include <stdio.h>
#include <process.h>
#include <Windows.h>

#define MAX_LOADSTRING 100
#define LINE_WDITH  300

#define EMPTY 0
#define ROUND 10
#define CROSS 50
#define COMPUTER 1
#define HUMAN 2
#define BOTH 3
#define DRAW 0
#define SLEEP_TIME 500
#define WAIT_COUNT 6

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];
			// the main window class name

static int board[9];
 const int path[8][3] = 
{
 {0,1,2},
 {3,4,5},
 {6,7,8},
 {0,3,6},
 {1,4,7},
 {2,5,8},
 {0,4,8},
 {2,4,6},
};

const int cell_value[9] = {4,0,2,6,8,1,3,5,7};//cell value is depend how many path cross the cell

int path_status[8][2];

int game_count = 0;

int who_win = -1;
int play_life;
bool game = true;

HWND game_hwnd;
RECT rect;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void print_board(HWND hWnd, HDC hdc);
void play_computer();
void varify_path();
bool mark_heighest_value_cell();
int win_chance();
int loss_chance();
bool fill_empty_cell_in_path(int path_id);
int get_my_path();
void fill_empty_cell();
void print_result(int result);
bool is_human_win();
void re_start();
DWORD WINAPI timer(__in LPVOID lpParameter);
void fill_my_path(int path_id);



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TIC_TAC_TIO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TIC_TAC_TIO));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TIC_TAC_TIO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_TIC_TAC_TIO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
      350, 250, 400, 600, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	int xpos;
	int ypos;
	int row = 0;
	int column = 0;
	int cell;

	switch (message)
	{
	case WM_CREATE:
		game_hwnd = hWnd;
		CreateThread(0, 0, &timer,0,0,0);
		CreateWindow (TEXT("static"), TEXT("You have 4 second time to take step"), WS_VISIBLE | WS_CHILD , 50, 500, 250, 20, hWnd, (HMENU) 1, NULL, NULL);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_LBUTTONDOWN:
			xpos = LOWORD(lParam);
			ypos = HIWORD(lParam);
			if(xpos > 50 && xpos < 350 && ypos > 50 && ypos < 350)
			{
				
				xpos -= 50;
				ypos -=50;
				row = ypos / 100;
				column = xpos / 100;
				cell = (row * 3) + column;
				
				if(board[cell] != 0)
				{
					MessageBox(game_hwnd,L"Place is alredy marked",L"Wrong Move",NULL);
					return 0;
				}
				game_count++;
				play_life = WAIT_COUNT;
				board[cell] = CROSS;
				GetClientRect(hWnd, &rect);
				InvalidateRect(hWnd,&rect, FALSE);
				
				if(is_human_win())
				{
					print_result(HUMAN);
					
				}
				else
				{
					if(game_count == 9)
					{
						print_result(DRAW);
					}
					else
					{
						play_computer();
						
					}
				}
				
				
			}
			break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		print_board(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		game = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void print_board(HWND hWnd, HDC hdc)
{
	HPEN			hLinePen;
	COLORREF		qLineColor;
	int xpos = 55;
	int ypos = 55;
	int position_incriminator=0;
	Rectangle(hdc,50,50,350,350);
	qLineColor		= RGB(0,0,0);//line color
	hLinePen		= CreatePen(PS_SOLID, 5, qLineColor);
	SelectObject(hdc, hLinePen);
	//MoveToEx(hdc, 50,100,NULL);
	//LineTo(hdc,50+LINE_WDITH,100);

	//board[5] = CROSS;

	MoveToEx(hdc, 52,150,NULL);
	LineTo(hdc,348,150);
	MoveToEx(hdc, 52,250,NULL);
	LineTo(hdc,348,250);

	MoveToEx(hdc, 150,52,NULL);
	LineTo(hdc,150,348);
	MoveToEx(hdc, 250,52,NULL);
	LineTo(hdc,250,348);

	for(int i = 0 ; i< 3 ; i++)
	{
		xpos = 55;
		for(int j = 0; j < 3; j++)
		{
			switch(board[position_incriminator])

			{
			case EMPTY:
				break;
			case ROUND:
				Ellipse(hdc,xpos+12,ypos+12, xpos+77, ypos+77);
				break;
			case CROSS:
				MoveToEx(hdc,xpos+15,ypos+15,NULL);
				LineTo(hdc,xpos + 75, ypos + 75);
				MoveToEx(hdc,xpos + 75, ypos +15,NULL);
				LineTo(hdc,xpos +15, ypos +75);
				break;

			}
			position_incriminator++;
			xpos += 100;
			
		}
		ypos += 100;
	}

	DeleteObject(hLinePen);
}

void play_computer()
{
	
	int win_path;
	int loss_path;
	int my_path;
	varify_path();
	if(game_count == 0 || game_count == 1)
	{
		
		mark_heighest_value_cell();
	}
	else
	{
		if(game_count == 3 && board[0] == ROUND && board[4] == CROSS && board[8] == CROSS)
		{
			board[2] = ROUND;
			game_count++;
			GetClientRect(game_hwnd, &rect);
			InvalidateRect(game_hwnd,&rect, FALSE);
			return;
		}
		win_path = win_chance();
		if(win_path >= 0)
		{
			fill_empty_cell_in_path(win_path);
			print_result(COMPUTER);
			return ;
		}
		else
		{
			loss_path = loss_chance();
			if(loss_path >= 0)
			{
				fill_empty_cell_in_path(loss_path);
			}
			else
			{
				my_path = get_my_path();
				if(my_path >= 0)
				{
					fill_empty_cell_in_path(my_path);
				}
				else
				{
					fill_empty_cell();
				}
			}

		}
	}
	game_count++;
	play_life = WAIT_COUNT;
	GetClientRect(game_hwnd, &rect);
	InvalidateRect(game_hwnd,&rect, FALSE);
}

void varify_path()
{
	
	for(int i = 0 ; i < 8; i++)
	{
		switch(board[path[i][0]] + board[path[i][1]] + board[path[i][2]])
		{

		case 0:
			path_status[i][0] = EMPTY;
		break;

		case 10:
			path_status[i][0] = ROUND;
			path_status[i][1] = 1;
		break;

		case 20:
			path_status[i][0] = ROUND;
			path_status[i][1] = 2;
		break;

		case 50:
			path_status[i][0] = CROSS;
			path_status[i][1] = 1;
		break;

		case 100:
			path_status[i][0] = CROSS;
			path_status[i][1] = 2;
		break;

		default:
			path_status[i][0] = BOTH;
		break;

		}

	}

}

bool mark_heighest_value_cell()
{
	
	int heigh_value;
	//for(int i = 0;  i < 9; i++)
	//{
	//
	//	if(board[cell_value[i]] == 0)
	//	{

	//		board[cell_value[i]] = ROUND;
	//		return true;
	//	}
	//}

	/*if(board[4] == EMPTY)
	{
		board[4] = ROUND;
	}
	else
	{
		int heigh_value = ((rand() % 10) / 4 ) + 1;
		board[cell_value[heigh_value]] = ROUND;
	}*/
	do
	{
		heigh_value = (rand() % 10) ;
	}while(board[heigh_value] != EMPTY);

	board[heigh_value] = ROUND;


	return false;
}

int win_chance()//this will return the path number in which tow round is marked
{
	for(int i = 0; i < 8; i++)
	{
		if(path_status[i][0] == ROUND && path_status[i][1] == 2)
		{
			return i;
		}
	}
	return -1;
}

int loss_chance()
{
	for(int i = 0; i < 8; i++)
	{
		if(path_status[i][0] == CROSS && path_status[i][1] == 2)
		{
			return i;
		}
	}
	return -1;
}

bool fill_empty_cell_in_path(int path_id)
{
	for(int i = 0; i < 3; i++)
	{
		if(board[path[path_id][i]] == EMPTY)
		{
			board[path[path_id][i]] = ROUND;
			return true;
		}
	}

	return false;
}

int get_my_path()
{
	for(int i = 0; i < 8; i++)
	{
		if(path_status[i][0] == ROUND && path_status[i][1] == 1)
		{
			return i;
		}
	}
	return -1;
}

void fill_empty_cell()
{
	for(int i = 0; i<9 ; i++)
	{
		if(board[i] == EMPTY)
		{
			board[i] = ROUND;
			break;
		}
	}
}

void print_result(int result)
{
	game_count = 0;
	switch(result)
	{
	case HUMAN:
		MessageBox(game_hwnd,L"You win I loss",L"WIN",NULL);
		break;
	case COMPUTER:
		MessageBox(game_hwnd,L"I win You loss",L"WIN",NULL);
		break;
	case DRAW:
		MessageBox(game_hwnd,L"Match Draw",L"DRAW",NULL);
		break;
	}

	re_start();

	
}

bool is_human_win()
{
	//{
	//	char	a[100];
	//	sprintf (a, "\ninside is win board value\n%d, %d, %d\n%d, %d, %d\n%d, %d, %d\n",board[0],board[1],board[2],board[3],board[4],board[5],board[6],board[7],board[8]);
	//	OutputDebugStringA (a);
	//}
	for(int i = 0; i < 8; i++)
	{
		if(board[path[i][0]] == CROSS && board[path[i][1]] == CROSS && board[path[i][2]] == CROSS)
			return true;
	}

	return false;
}

void re_start()
{
	for(int i = 0; i < 9; i++)
	{
		board[i] = EMPTY;
	}

	for(int i = 0; i < 8; i++)
	{
		path_status[i][0] = EMPTY;
		path_status[i][1] = EMPTY;
	}

	game_count = 0;

	GetClientRect(game_hwnd, &rect);
	InvalidateRect(game_hwnd,&rect, FALSE);
	/*{
		char	a[100];
		sprintf (a, "\nrestart board value\n%d, %d, %d\n%d, %d, %d\n%d, %d, %d\n",board[0],board[1],board[2],board[3],board[4],board[5],board[6],board[7],board[8]);
		OutputDebugStringA (a);
	}*/
}

DWORD WINAPI timer(__in LPVOID lpParameter)
{
	while(game)
	{
		if(game_count > 0)
		{
			while(play_life > 0)
			{
				Sleep(500);
				play_life--;
	//			{
	//	char	a[100];
	//	sprintf (a, "\nrestart board value\n%d, %d, %d\n%d, %d, %d\n%d, %d, %d\n",board[0],board[1],board[2],board[3],board[4],board[5],board[6],board[7],board[8]);
	//	OutputDebugStringA (a);
	//}
			}
			if(game_count > 0 && game_count < 9)
			{
				if(game_count % 2 == 0)
				{
					MessageBox(game_hwnd,L"Your Time is Over and You Loss",L"Loss",NULL);
					re_start();
				}
				else
				{
					MessageBox(game_hwnd,L"My Time is Over and I Loss",L"Loss",NULL);
					re_start();
				}
			}
		}
	}

	return 0;
}

void fill_my_path(int path_id)
{
	
	
	int next_cell = ((rand() % 10) / 2) + 1;
	board[path[path_id][next_cell]] = ROUND;
}