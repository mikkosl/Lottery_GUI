// Lottery_GUI.cpp : Defines the entry point for the application.
//

#include "Lottery_GUI.h"

#define MAX_LOADSTRING 100
#pragma comment(lib, "Winspool.lib")

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
int g_numRows = 0; // Default number of rows
int mode = 0; // 1 = Lotto, 2 = Vikinglotto, 3 = Eurojackpot
std::vector<std::vector<int>> rows;

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

// Generate new Lotto rows
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

// Generate new Vikinglotto rows
std::vector<int> GenerateVikingNumbers(int count = 6, int min = 1, int max = 48)
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
    std::uniform_int_distribution<> dis1(1, 5);
    int num = dis1(gen);
    numbers.push_back(num);
    return numbers;
}

// Generate new Eurojackpot rows
std::vector<int> GenerateEuroNumbers(int count = 5, int min = 1, int max = 50)
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
    std::uniform_int_distribution<> dis1(1, 12);
    int num1 = dis1(gen);
    int num2 = dis1(gen);
    while (num1 == num2) num2 = dis1(gen);
    if (num1 < num2) {
        numbers.push_back(num1);
        numbers.push_back(num2);
    } else {
        numbers.push_back(num2);
        numbers.push_back(num1);
    }
    return numbers;
}

std::wstring FormatLotteryRow(const std::vector<int>& numbers)
{
    std::wstringstream ss;
    for (size_t i = 0; i < numbers.size(); ++i) {
        ss << numbers[i];
        if (i < numbers.size() - 1)
            if (i > 4 && mode == 2) ss << L"  +  ";
            else if (i == 4 && mode == 3) ss << L"  +  ";
            else ss << L", ";
    }
    return ss.str();
}

std::wstring ToLower(const std::wstring& input) {
    std::wstring result = input;
    for (wchar_t& ch : result) {
        ch = towlower(ch);
    }
    return result;
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
                    if (val < 1 || val > 60) {
                        MessageBox(hDlg, L"Please enter a number between 1 and 60.", L"Invalid Input", MB_OK | MB_ICONERROR);
                        return TRUE; // Stay in dialog
					}
                    EndDialog(hDlg, val);
                    return TRUE;
                }
                if (LOWORD(wParam) == IDCANCEL) {
					g_numRows = -1; // Indicate cancellation
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
        case IDM_HELP_NEWLOTTERYROWS:
            MessageBox(hWnd, L"Open New menu and select the game you want to play. Then input the number of rows you want to generate.", L"Getting Started", MB_OK | MB_ICONINFORMATION);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_NEWLOTTERYROWS:
            mode = 1;
            g_numRows = PromptForNumRows(hWnd, g_numRows);        // Prompt user for new number of rows
            if (g_numRows == -1) {                                // User cancelled
                g_numRows = 0;                                    // Reset to 0 or previous valid state
                break;
            }
            rows.clear();                                         // Clear existing rows
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
            InvalidateRect(hWnd, NULL, TRUE);                     // Force a repaint to display new rows
            break;
        case IDM_NEWVIKINGROWS:
            mode = 2;
            g_numRows = PromptForNumRows(hWnd, g_numRows);        // Prompt user for new number of rows
            if (g_numRows == -1) {                                // User cancelled
                g_numRows = 0;                                    // Reset to 0 or previous valid state
                break;
            }
            rows.clear();                                         // Clear existing rows
            while (rows.size() < g_numRows) {
                auto candidate = GenerateVikingNumbers();
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
            InvalidateRect(hWnd, NULL, TRUE);                     // Force a repaint to display new rows
            break;
        case IDM_NEWEUROROWS:
            mode = 3;
            g_numRows = PromptForNumRows(hWnd, g_numRows);        // Prompt user for new number of rows
            if (g_numRows == -1) {                                // User cancelled
                g_numRows = 0;                                    // Reset to 0 or previous valid state
                break;
            }
            rows.clear();                                         // Clear existing rows
            while (rows.size() < g_numRows) {
                auto candidate = GenerateEuroNumbers();
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
            InvalidateRect(hWnd, NULL, TRUE);                     // Force a repaint to display new rows
            break;
        case IDM_SAVELOTTERYROWS:
        {
            if (rows.empty()) {
                MessageBox(hWnd, L"No rows to save. Please generate numbers first.", L"Error", MB_OK | MB_ICONERROR);
                break;
            }
            // Open file dialog to select save location
            wchar_t filename[MAX_PATH] = L"";
            switch (mode)
            {
            case 1:
                wcscpy_s(filename, L"Lotto.txt");
                break;
            case 2:
                wcscpy_s(filename, L"Vikinglotto.txt");
                break;
            case 3:
                wcscpy_s(filename, L"Eurojackpot.txt");
                break;
            default:
                wcscpy_s(filename, L"rows.txt");
                break;
            }
            OPENFILENAME ofn = { 0 };
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFile = filename;
            ofn.nMaxFile = sizeof(filename) / sizeof(filename[0]);
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
            if (GetSaveFileName(&ofn)) {
                std::wofstream outFile(filename);
                if (!outFile) {
                    MessageBox(hWnd, L"Failed to open file for writing.", L"Error", MB_OK | MB_ICONERROR);
                    break;
                }
                for (size_t i = 0; i < rows.size(); ++i) {
                    if (i < 9) outFile << L"(Row 0" << (i + 1) << L"): " << FormatLotteryRow(rows[i]) << std::endl;
                    else outFile << L"(Row " << (i + 1) << L"): " << FormatLotteryRow(rows[i]) << std::endl;
                }
                outFile.close();
                MessageBox(hWnd, L"Rows saved successfully.", L"Success", MB_OK | MB_ICONINFORMATION);
            }
        }
            break;
        case IDM_PRINTLOTTERYROWS:
        {
            if (rows.empty()) {
                MessageBox(hWnd, L"No rows to print. Please generate numbers first.", L"Error", MB_OK | MB_ICONERROR);
                break;
            }

            PRINTDLG pd = { 0 };
            pd.lStructSize = sizeof(pd);
            pd.Flags = PD_RETURNDC | PD_USEDEVMODECOPIESANDCOLLATE;
            pd.hwndOwner = hWnd;
            HANDLE hPrinter = NULL;

            if (PrintDlg(&pd)) {
                // Extract printer name AFTER PrintDlg succeeds
                WCHAR safePrinterName[256] = { 0 };
                if (pd.hDevNames) {
                    DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(pd.hDevNames);
                    if (pDevNames) {
                        LPCWSTR printerName = (LPCWSTR)pDevNames + pDevNames->wDeviceOffset;
                        // Use printerName safely
                        GlobalUnlock(pd.hDevNames);
                    } else {
                        if (!pDevNames) {
                            DWORD err = GetLastError();
                            WCHAR msg[256];
                            FormatMessageW(
                                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL,
                                err,
                                0,
                                msg,
                                _countof(msg),
                                NULL
                            );
                            MessageBoxW(NULL, msg, L"GlobalLock Failed", MB_ICONERROR);
                        }  
					}
                if (pDevNames) {
                    LPCWSTR rawPtr = (LPCWSTR)pDevNames + pDevNames->wDeviceOffset;
                    wcsncpy_s(safePrinterName, rawPtr, _countof(safePrinterName) - 1);
                }
                GlobalUnlock(pd.hDevNames);
            } else {
                 MessageBoxW(nullptr, L"Failed to get printer name", L"Error", MB_ICONERROR);
                 break;
			}
                PRINTER_DEFAULTS defaults = { 0 };
                defaults.pDatatype = nullptr;
                defaults.pDevMode = (DEVMODE*)GlobalLock(pd.hDevMode);
                defaults.DesiredAccess = PRINTER_ACCESS_USE;
                if (!OpenPrinterW(safePrinterName, &hPrinter, &defaults)) {
                    MessageBoxW(nullptr, L"Failed to open printer", safePrinterName, MB_ICONERROR);
                } else {
                    HDC hdc = CreateDCW(L"WINSPOOL", safePrinterName, NULL, NULL);
                    if (!hdc) {
                        ClosePrinter(hPrinter);
                        MessageBoxW(nullptr, L"Failed to create printer DC", safePrinterName, MB_ICONERROR);
                        break;
                    }
                    DOCINFOW di = { 0 };
                    di.cbSize = sizeof(DOCINFOW);
                    di.lpszDocName = L"Lottery Ticket";
                    di.lpszOutput = nullptr; // or a valid path for PDF printers
                    if (StartDocW(hdc, &di) <= 0) {
                        DeleteDC(hdc);
                        ClosePrinter(hPrinter);
                        MessageBoxW(nullptr, L"Failed to start document", safePrinterName, MB_ICONERROR);
                        break;
                    }
                    StartPage(hdc);
                    std::wstring rowStr;
                    int y = 200;
                    for (size_t i = 0; i < rows.size(); ++i) {
                        if (i < 9) rowStr = L"(Row 0" + std::to_wstring(i + 1) + L"): " + FormatLotteryRow(rows[i]);
                        else rowStr = L"(Row " + std::to_wstring(i + 1) + L"): " + FormatLotteryRow(rows[i]);
                        TextOutW(hdc, 200, y, rowStr.c_str(), (int)rowStr.length());
                        y += 110;
                    }
                    EndPage(hdc);
                    EndDoc(hdc);
                    DeleteDC(hdc);
                    ClosePrinter(hPrinter);
                    MessageBox(hWnd, L"Printing...", safePrinterName, MB_OK | MB_ICONINFORMATION);
                }
            }
        }
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
        std::wstring rowStr;
        for (size_t i = 0; i < rows.size(); ++i) {
            if (i < 9) rowStr = L"(Row 0" + std::to_wstring(i + 1) + L"): " + FormatLotteryRow(rows[i]);
            else rowStr = L"(Row " + std::to_wstring(i + 1) + L"): " + FormatLotteryRow(rows[i]);
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
