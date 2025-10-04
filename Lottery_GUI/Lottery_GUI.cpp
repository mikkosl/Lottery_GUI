// Lottery_GUI.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Lottery_GUI.h"
#include <vector>
#include <random>
#include <string>
#include <sstream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
int g_numRows = 5; // Default number of rows

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

bool AreRowsEqual(const std::vector<int>& a, const std::vector<int>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// GenerateLotteryRow now returns vector<int> for easier comparison
std::vector<int> GenerateLotteryNumbers(int count = 7, int min = 1, int max = 40)
{
    std::vector<int> numbers;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    while (numbers.size() < count) {
        int num = dis(gen);
        if (std::find(numbers.begin(), numbers.end(), num) == numbers.end()) {
            numbers.push_back(num);
        }
    }
    std::sort(numbers.begin(), numbers.end());
    return numbers;
}

std::wstring FormatLotteryRow(const std::vector<int>& numbers)
{
    std::wstringstream ss;
    // Removed: ss << L"Lottery row: ";
    for (size_t i = 0; i < numbers.size(); ++i) {
        ss << numbers[i];
        if (i < numbers.size() - 1)
            ss << L", ";
    }
    return ss.str();
}

// Add a function to prompt for number of rows
int PromptForNumRows(HWND hWnd, int currentRows) {
    wchar_t buf[16];
    swprintf_s(buf, L"%d", currentRows);
   
    INT_PTR result = DialogBoxParam(
        hInst,
        MAKEINTRESOURCE(IDD_SETROWS),
        hWnd,
        [](HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) -> INT_PTR {
        switch (msg) {
            case WM_INITDIALOG:
                SetDlgItemText(hDlg, IDC_EDIT_ROWS, (LPCWSTR)lParam);
                return TRUE;
            case WM_COMMAND:
                if (LOWORD(wParam) == IDOK) {
                    wchar_t input[16];
                    GetDlgItemText(hDlg, IDC_EDIT_ROWS, input, 16);
                    int val = _wtoi(input);
                    EndDialog(hDlg, val > 0 ? val : 0);
                    return TRUE;
                }
                if (LOWORD(wParam) == IDCANCEL) {
                    EndDialog(hDlg, g_numRows);
                    return TRUE;
                }
                break;
            }
            return FALSE;
        },
        (LPARAM)buf
    );
    return (int)result;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.


    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LOTTERYGUI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LOTTERYGUI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOTTERYGUI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LOTTERYGUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
        int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_NEWLOTTERYROWS:
                // Force a repaint to generate new rows
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            int y = 10;
            int x = 45;
            std::vector<std::vector<int>> rows;
            g_numRows = PromptForNumRows(hWnd, g_numRows);           // Prompt user for new number of rows
            while (rows.size() < g_numRows) {
                auto candidate = GenerateLotteryNumbers();
                bool duplicate = false;
                for (const auto& row : rows) {
                    if (AreRowsEqual(row, candidate)) {
                        duplicate = true;
                        break;
                    }
                }
                if (!duplicate) {
                    rows.push_back(candidate);
                }
            }
            for (size_t i = 0; i < rows.size(); ++i) {
                std::wstring rowStr = L"Row " + std::to_wstring(i + 1) + L": " + FormatLotteryRow(rows[i]);
                TextOutW(hdc, x, y, rowStr.c_str(), (int)rowStr.length());
                y += 20;
                if ((i + 1) % 20 == 0) {
                    x += 300;
                    y = 10;
                }
            }

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
