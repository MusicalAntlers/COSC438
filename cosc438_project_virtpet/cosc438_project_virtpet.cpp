// cosc438_project_virtpet.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "cosc438_project_virtpet.h"
#include <string>

#define MAX_LOADSTRING 100
#define CLIENT_W 420
#define CLIENT_H 360

// -----------------------------------------------------------------------------
// Pet state (global for simplicity in a Win32 single-file project)
// -----------------------------------------------------------------------------
static int g_hunger = 80; //0 - starving, 100 - full
static int g_happiness = 70; // 0 = miserable, 100 = esctatic
static int g_health = 90; // 0 = sick, 100 = healthy

// -----------------------------------------------------------------------------
// Global Variables:
// -----------------------------------------------------------------------------
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// -----------------------------------------------------------------------------
// Forward declarations of functions included in this code module:
// -----------------------------------------------------------------------------
void DrawStatBar(HDC, int, int, int, int, int, COLORREF, const wchar_t*);
void DrawPet(HDC, int);
void DrawStats(HDC);
void Feed_RefreshLabel(HWND);

ATOM                MyRegisterClass(HINSTANCE);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    FeedDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Game1DlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Game2DlgProc(HWND, UINT, WPARAM, LPARAM);

// Helper: draw a centered stat bar using GDI
void DrawStatBar(HDC hdc, int x, int y, int barW, int barH, int value, COLORREF fillColor, const wchar_t* label)

{
    // Label
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(50, 50, 50));
	TextOutW(hdc, x, y, label, (int)wcslen(label));

    int barX = x + 100;
	int filled = (int)((value / 100.0) * barW);

	// Draw background (empty bar)
    RECT rcBg = { barX, y + 1, barX + barW, y + barH - 1 };
	HBRUSH hBg = CreateSolidBrush(RGB(210, 210, 210));
    FillRect(hdc, &rcBg, hBg);
	DeleteObject(hBg);

	// Draw filled portion
    if (filled > 0)
    {
		RECT rcFill = { barX, y + 1, barX + filled, y + barH -1 };
		HBRUSH hFill = CreateSolidBrush(fillColor);
		FillRect(hdc, &rcFill, hFill);
		DeleteObject(hFill);
    }

	// Draw border
	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
	HBRUSH hOldBr = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, barX, y,  barX + barW, y + barH);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBr);
    DeleteObject(hPen);

	// Numeric value
	std::wstring val = std::to_wstring(value) + L"%";
	TextOutW(hdc, barX + barW + 8, y, val.c_str(), (int)val.size());
}

// -----------------------------------------------------------------------------
// DrawPet - simple GDI "ASCII-style" pet drawn with lines and shapes.
//           The pet's expression changes based on overall wellness.
// -----------------------------------------------------------------------------
void DrawPet(HDC hdc, int clientW)
{
	int wellness = (g_hunger + g_happiness + g_health) / 3;

    //Center the pet horizontally in the top portion of the window
	int cx = clientW / 2;
    int topY = 20;

    // --- Choose pen/brush ---
    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(60, 60, 60));
    HBRUSH hBody = CreateSolidBrush(RGB(255, 220, 120));   //warm yellow body
	HBRUSH hFace = CreateSolidBrush(RGB(255, 240, 180));   //lighter face
	HBRUSH hDark = CreateSolidBrush(RGB(40, 40, 40));       //dark eyes

    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBody);

    // --- Body (rounded rectangle approximated as ellipse) ---
    Ellipse(hdc, cx - 55, topY + 40, cx + 55, topY + 130);

    // --- Head ---
    SelectObject(hdc, hFace);
	Ellipse(hdc, cx - 55, topY + 40, cx + 55, topY + 130);

    // --- Ears (two small filled ellipses above the head) ---
	SelectObject(hdc, hBody);
	Ellipse(hdc, cx - 45, topY - 18, cx - 20, topY + 10);   // Left ear
	Ellipse(hdc, cx + 20, topY - 18, cx + 45, topY + 10);   // Right ear

    // --- Arms/paws ---
	Ellipse(hdc, cx - 80, topY + 65, cx = 52, topY + 90);   // Left arm
	Ellipse(hdc, cx + 52, topY + 105, cx + 80, topY + 130); // Right arm

    // Tail
    {
        HPEN hTail = CreatePen(PS_SOLID, 3, RGB(195, 155, 55));
		SelectObject(hdc, hTail);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		MoveToEx(hdc, cx + 53, topY + 96, nullptr);
		LineTo(hdc, cx + 76, topY + 80);
		LineTo(hdc, cx + 90, topY + 62);
		LineTo(hdc, cx + 82, topY + 48);
        SelectObject(hdc, hPen);
		DeleteObject(hTail);
    }

    // Eyes
	SelectObject(hdc, hDark);
    if (wellness >= 50)
    {
        // Open happy eyes
        Ellipse(hdc, cx - 24, topY + 18, cx - 11, topY + 36);
        Ellipse(hdc, cx + 11, topY + 18, cx + 24, topY + 31);
    }
    else
    {
        // Tired flat lines
		HPEN hSad = CreatePen(PS_SOLID, 3, RGB(40, 40, 40));
		SelectObject(hdc, hSad);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		MoveToEx(hdc, cx - 24, topY + 25, nullptr); LineTo(hdc, cx - 11, topY + 25);
		MoveToEx(hdc, cx + 11, topY + 25, nullptr); LineTo(hdc, cx + 24, topY + 25);
		SelectObject(hdc, hPen);
		DeleteObject(hSad);
		SelectObject(hdc, hDark);
    }

    // Nose
	SelectObject(hdc, hDark);
	Ellipse(hdc, cx - 5, topY + 36, cx + 5, topY + 44);

    // Mouth
    {
		HPEN hMouth = CreatePen(PS_SOLID, 2, RGB(180, 70, 70));
		SelectObject(hdc, hMouth);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
        if (wellness >= 70)
            Arc(hdc, cx - 18, topY + 42, cx + 18, cx + 64, cx - 18, topY + 52, cx + 18, topY + 52);
        else if (wellness >= 40)
        {
			MoveToEx(hdc, cx - 13, topY + 53, nullptr);
			LineTo(hdc, cx + 13, topY + 53);
        }
        else
            Arc(hdc, cx - 18, topY + 46, cx + 18, topY + 68, cx + 18, topY + 57, cx - 18, topY + 57);
			SelectObject(hdc, hPen);
			DeleteObject(hMouth);
    }

    // --- Status label below the pet ---
    const wchar_t* mood =
    wellness >= 75 ? L"( ^..^ ) Your pet is very happy!":
    wellness >= 50 ? L"( -..- ) Your pet is doing okay.":
    wellness >= 25 ? L"( >..< ) Your pet needs attention!":
	                 L"( T..T ) Your pet is miserable!";

	HFONT hFont = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(70, 70, 160));

    SIZE sz;
	int len = (int)wcslen(mood);
	GetTextExtentPoint32W(hdc, mood, len, &sz);
	TextOutW(hdc, cx - sz.cx / 2, topY + 138, mood, len);

	SelectObject(hdc, hOldFont);
    DeleteObject(hFont);

    // --- Cleanup ---
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
	DeleteObject(hBody);
	DeleteObject(hFace);
	DeleteObject(hDark);
}

// -----------------------------------------------------------------------------
// DrawStats - hunger / happiness / health bars below the pet
// -----------------------------------------------------------------------------
void DrawStats(HDC hdc)
{
    int x = 30;
    int y = 210;
    int barW = 180;
    int barH = 16;
    int gap = 28;

    HFONT hFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT hOld = (HFONT)SelectObject(hdc, hFont);

    DrawStatBar(hdc, x, y, barW, barH, g_hunger, RGB(255, 140, 40), L"Hunger:     ");
    DrawStatBar(hdc, x, y + gap, barW, barH, g_happiness, RGB(80, 180, 80), L"Happiness:");
    DrawStatBar(hdc, x, y + gap * 2, barW, barH, g_health, RGB(60, 140, 220), L"Health:    ");

    SelectObject(hdc, hOld);
    DeleteObject(hFont);
}

// =============================================================================
// wWinMain
// =============================================================================

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_COSC438PROJECTVIRTPET, szWindowClass, MAX_LOADSTRING);
    //MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING) || szTitle[0] == L'\0') wcscpy_s(szTitle, MAX_LOADSTRING, L"Virtual Pet");

    if (!LoadStringW(hInstance, IDC_COSC438PROJECTVIRTPET, szWindowClass, MAX_LOADSTRING) || szWindowClass[0] == L'\0') wcscpy_s(szWindowClass, MAX_LOADSTRING, L"COSC438PROJECTVIRTPET");

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COSC438PROJECTVIRTPET));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}


// =============================================================================
//  FUNCTION: MyRegisterClass()
// =============================================================================
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_COSC438PROJECTVIRTPET);
    wcex.lpszClassName  = szWindowClass;
	//wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COSC438PROJECTVIRTPET));
    //wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// =============================================================================
//   FUNCTION: InitInstance(HINSTANCE, int)
// =============================================================================
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

   // Start with desired client sizea and expand to windwo size
   RECT rc = { 0, 0, CLIENT_W, CLIENT_H };
   DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX; // fixed-size window
   AdjustWindowRect(&rc, style, TRUE); //True = has window

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, style,
	   CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
	   return FALSE;

	   ShowWindow(hWnd, nCmdShow);
       UpdateWindow(hWnd);
       return TRUE;
 }

// =============================================================================
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
// =============================================================================
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
            // Parse the menu selections:
            switch (LOWORD(wParam))
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

                // --- Feed ---
            case IDM_FEED:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_FEED), hWnd, FeedDlgProc);
                InvalidateRect(hWnd, nullptr, TRUE);  // repaint pet after feeding
                break;

                // --- Game 1 ---
/*            case IDM_GAME1:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_GAME1), hWnd, Game1DlgProc);
                InvalidateRect(hWnd, nullptr, TRUE);
                break;

                // --- Game 2 ---
            case IDM_GAME2:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_GAME2), hWnd, Game2DlgProc);
                InvalidateRect(hWnd, nullptr, TRUE);
                break;*/

            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
    }
        break;

        // ----------------------------------------------------------------------------------

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            // TODO: Add any drawing code here...
			HDC hdc = BeginPaint(hWnd, &ps);

            RECT rc;
			GetClientRect(hWnd, &rc);

            // Fill background
			HBRUSH hBg = CreateSolidBrush(RGB(245, 245, 255));
			FillRect(hdc, &rc, hBg);
			DeleteObject(hBg);

            // Draw pet and stats into the memory DC
			DrawPet(hdc, rc.right);
			DrawStats(hdc);

			EndPaint(hWnd, &ps);
            break;
        }

		// ----------------------------------------------------------------------------------

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// =============================================================================
// Message handler for about box.
// =============================================================================
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

// =============================================================================
// Feed dialog helpers
// =============================================================================

//Updates the hunger label inside IDD_FEED with the current g_hunger value.
static void Feed_RefreshLabel(HWND hDlg)
{
    std::wstring s = L"Hunger: " + std::to_wstring(g_hunger) + L" / 100";
	SetDlgItemTextW(hDlg, IDC_FEED_HUNGER_LABEL, s.c_str());
}

// =============================================================================
// Feed dialog
//  - Shows current hunger level
//  - "Feed" button raises hunger by 20 (capped at 100)
//  - Happiness and health nudge slightly
// =============================================================================
INT_PTR CALLBACK FeedDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        Feed_RefreshLabel(hDlg);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BTN_FEED:
        {
            if (g_hunger >= 100)
            {
                MessageBoxW(hDlg, L"Your pet is already full!", L"Feed", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                g_hunger = min(100, g_hunger + 20);
                g_health = min(100, g_health + 5);
                Feed_RefreshLabel(hDlg);
                MessageBoxW(hDlg, L"Yum! Your pet enjoyed the meal.", L"Feed", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
