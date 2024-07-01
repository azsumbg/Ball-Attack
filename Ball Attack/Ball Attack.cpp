#include "framework.h"
#include "Ball Attack.h"
#include <mmsystem.h>
#include "FCheck.h"
#include "errh.h"
#include "D2BMPLOADER.h"
#include "BallEngine.h"
#include <d2d1.h>
#include <dwrite.h>
#include <vector>
#include <ctime>
#include <chrono>
#include <fstream>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "fcheck.lib")
#pragma comment(lib, "errh.lib")
#pragma comment(lib, "d2bmploader.lib")
#pragma comment(lib, "ballengine.lib")

#define bWinClassName L"AllienBalls"

#define tmp_file ".\\res\\data\\temp.dat"
#define Ltmp_file L".\\res\\data\\temp.dat"
#define record_file L".\\res\\data\\record.dat"
#define save_file L".\\res\\data\\save.dat"
#define help_file L".\\res\\data\\help.dat"
#define snd_file L".\\res\\snd\\sound.dat"

#define mNew 1001
#define mLevel 1002
#define mExit 1003
#define mSave 1004
#define mLoad 1005
#define mHoF 1006

#define record 2001
#define first_record 2002
#define no_record 2003

WNDCLASS bWin = { 0 };
HINSTANCE bIns = nullptr;
HICON mainIcon = nullptr;
HCURSOR mainCursor = nullptr;
HCURSOR outCursor = nullptr;
HWND bHwnd = nullptr;
HMENU bBar = nullptr;
HMENU bMain = nullptr;
HMENU bStore = nullptr;
HDC PaintDC = nullptr;
PAINTSTRUCT bPaint = { 0 };
MSG bMsg = { 0 };
BOOL bRet = 0;
POINT cur_pos = { 0 };
UINT_PTR bTimer = -1;

bool pause = false;
bool sound = true;
bool show_help = false;
bool in_client = true;
bool name_set = false;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;

int level = 1;
int mins = 0;
int secs = 0;
int score = 0;
int lifes = 100;
int intruders = 0;

wchar_t current_player[16] = L"THE WARRIOR";

D2D1_RECT_F b1Rect = { 0, 0, scr_width / 3 - 50.0f, 50.0f };
D2D1_RECT_F b2Rect = { scr_width / 3, 0, scr_width * 2 / 3 - 50.0f, 50.0f };
D2D1_RECT_F b3Rect = { scr_width * 2 / 3, 0, scr_width, 50.0f };

D2D1_RECT_F b1TxtR = { 20.0f, 0, scr_width / 3 - 50.0f, 50.0f };
D2D1_RECT_F b2TxtR = { scr_width / 3 + 20.0f, 0, scr_width * 2 / 3 - 50.0f, 50.0f };
D2D1_RECT_F b3TxtR = { scr_width * 2 / 3 + 20.0f, 0, scr_width, 50.0f };

ID2D1Factory* iFactory = nullptr;
ID2D1HwndRenderTarget* Draw = nullptr;
ID2D1RadialGradientBrush* ButBckgBrush = nullptr;
ID2D1SolidColorBrush* TxtBrush = nullptr;
ID2D1SolidColorBrush* HgltBrush = nullptr;
ID2D1SolidColorBrush* InactBrush = nullptr;

IDWriteFactory* iWriteFactory = nullptr;
IDWriteTextFormat* nrmText = nullptr;
IDWriteTextFormat* bigText = nullptr;

ID2D1Bitmap* bmpField = nullptr;
ID2D1Bitmap* bmpAxe = nullptr;

ID2D1Bitmap* bmpBlueEgg = nullptr;
ID2D1Bitmap* bmpRedEgg = nullptr;
ID2D1Bitmap* bmpGreenEgg = nullptr;
ID2D1Bitmap* bmpYellowEgg = nullptr;
ID2D1Bitmap* bmpPurpleEgg = nullptr;
ID2D1Bitmap* bmpBlueBall = nullptr;
ID2D1Bitmap* bmpRedBall = nullptr;
ID2D1Bitmap* bmpGreenBall = nullptr;
ID2D1Bitmap* bmpYellowBall = nullptr;
ID2D1Bitmap* bmpPurpleBall = nullptr;

ID2D1Bitmap* bmpMidBlueEgg = nullptr;
ID2D1Bitmap* bmpMidRedEgg = nullptr;
ID2D1Bitmap* bmpMidGreenEgg = nullptr;
ID2D1Bitmap* bmpMidYellowEgg = nullptr;
ID2D1Bitmap* bmpMidPurpleEgg = nullptr;
ID2D1Bitmap* bmpMidBlueBall = nullptr;
ID2D1Bitmap* bmpMidRedBall = nullptr;
ID2D1Bitmap* bmpMidGreenBall = nullptr;
ID2D1Bitmap* bmpMidYellowBall = nullptr;
ID2D1Bitmap* bmpMidPurpleBall = nullptr;

ID2D1Bitmap* bmpSmallBlueEgg = nullptr;
ID2D1Bitmap* bmpSmallRedEgg = nullptr;
ID2D1Bitmap* bmpSmallGreenEgg = nullptr;
ID2D1Bitmap* bmpSmallYellowEgg = nullptr;
ID2D1Bitmap* bmpSmallPurpleEgg = nullptr;
ID2D1Bitmap* bmpSmallBlueBall = nullptr;
ID2D1Bitmap* bmpSmallRedBall = nullptr;
ID2D1Bitmap* bmpSmallGreenBall = nullptr;
ID2D1Bitmap* bmpSmallYellowBall = nullptr;
ID2D1Bitmap* bmpSmallPurpleBall = nullptr;

ID2D1Bitmap* bmpCatapultL[3] = { nullptr };
ID2D1Bitmap* bmpCatapultR[3] = { nullptr };
///////////////////////////////////////////////////////////////////

dll::Object Catapult = nullptr;
std::vector < dll::Object> vEnemies;

///////////////////////////////////////////////////////////////////

template<typename GARBAGE> bool CleanMem(GARBAGE** what)
{
    if ((*what))
    {
        (*what)->Release();
        (*what) = nullptr;
        return true;
    }
    return false;
}
void ErrLog(LPCWSTR what)
{
    std::wofstream log(L".\\res\\data\\errorlog.dat", std::ios::app);
    log << what << L" ! Timestamp: " << std::chrono::system_clock::now() << std::endl;
    log.close();
}
void ReleaseResources()
{
    if (!CleanMem(&iFactory))ErrLog(L"Error releasing iFactory");
    if (!CleanMem(&Draw))ErrLog(L"Error releasing Draw");
    if (!CleanMem(&ButBckgBrush))ErrLog(L"Error releasing ButBckgBrush");
    if (!CleanMem(&TxtBrush))ErrLog(L"Error releasing TxtBrush");
    if (!CleanMem(&HgltBrush))ErrLog(L"Error releasing HgltBrush");
    if (!CleanMem(&InactBrush))ErrLog(L"Error releasing InatBrush");
    if (!CleanMem(&iWriteFactory))ErrLog(L"Error releasing iWriteFactory");
    if (!CleanMem(&nrmText))ErrLog(L"Error releasing nrmText");
    if (!CleanMem(&bigText))ErrLog(L"Error releasing bigText");

    if (!CleanMem(&bmpField))ErrLog(L"Error releasing bmpField");
    if (!CleanMem(&bmpAxe))ErrLog(L"Error releasing bmpAxe");

    if (!CleanMem(&bmpBlueEgg))ErrLog(L"Error releasing bmpBlueEgg");
    if (!CleanMem(&bmpRedEgg))ErrLog(L"Error releasing bmpRedEgg");
    if (!CleanMem(&bmpGreenEgg))ErrLog(L"Error releasing bmpGreenEgg");
    if (!CleanMem(&bmpYellowEgg))ErrLog(L"Error releasing bmpYellowEgg");
    if (!CleanMem(&bmpPurpleEgg))ErrLog(L"Error releasing bmpPurpleEgg");
    if (!CleanMem(&bmpBlueBall))ErrLog(L"Error releasing bmpBlueBall");
    if (!CleanMem(&bmpRedBall))ErrLog(L"Error releasing bmpRedBall");
    if (!CleanMem(&bmpGreenBall))ErrLog(L"Error releasing bmpGreenBall");
    if (!CleanMem(&bmpYellowBall))ErrLog(L"Error releasing bmpYellowBall");
    if (!CleanMem(&bmpPurpleBall))ErrLog(L"Error releasing bmpPurpleBall");

    if (!CleanMem(&bmpMidBlueEgg))ErrLog(L"Error releasing bmpMidBlueEgg");
    if (!CleanMem(&bmpMidRedEgg))ErrLog(L"Error releasing bmpMidRedEgg");
    if (!CleanMem(&bmpMidGreenEgg))ErrLog(L"Error releasing bmpMidGreenEgg");
    if (!CleanMem(&bmpMidYellowEgg))ErrLog(L"Error releasing bmpMidYellowEgg");
    if (!CleanMem(&bmpMidPurpleEgg))ErrLog(L"Error releasing bmpMidPurpleEgg");
    if (!CleanMem(&bmpMidBlueBall))ErrLog(L"Error releasing bmpMidBlueBall");
    if (!CleanMem(&bmpMidRedBall))ErrLog(L"Error releasing bmpMidRedBall");
    if (!CleanMem(&bmpMidGreenBall))ErrLog(L"Error releasing bmpMidGreenBall");
    if (!CleanMem(&bmpMidYellowBall))ErrLog(L"Error releasing bmpMidYellowBall");
    if (!CleanMem(&bmpMidPurpleBall))ErrLog(L"Error releasing bmpMidPurpleBall");

    if (!CleanMem(&bmpSmallBlueEgg))ErrLog(L"Error releasing bmpSmallBlueEgg");
    if (!CleanMem(&bmpSmallRedEgg))ErrLog(L"Error releasing bmpSmallRedEgg");
    if (!CleanMem(&bmpSmallGreenEgg))ErrLog(L"Error releasing bmpSmallGreenEgg");
    if (!CleanMem(&bmpSmallYellowEgg))ErrLog(L"Error releasing bmpSmallYellowEgg");
    if (!CleanMem(&bmpSmallPurpleEgg))ErrLog(L"Error releasing bmpSmallPurpleEgg");
    if (!CleanMem(&bmpSmallBlueBall))ErrLog(L"Error releasing bmpSmallBlueBall");
    if (!CleanMem(&bmpSmallRedBall))ErrLog(L"Error releasing bmpSmallRedBall");
    if (!CleanMem(&bmpSmallGreenBall))ErrLog(L"Error releasing bmpSmallGreenBall");
    if (!CleanMem(&bmpSmallYellowBall))ErrLog(L"Error releasing bmpSmallYellowBall");
    if (!CleanMem(&bmpSmallPurpleBall))ErrLog(L"Error releasing bmpSmallPurpleBall");

    for (int i = 0; i < 3; i++)
        if (!CleanMem(&bmpCatapultL[i]))ErrLog(L"Error releasing CatapultL");
    for (int i = 0; i < 3; i++)
        if (!CleanMem(&bmpCatapultR[i]))ErrLog(L"Error releasing CatapultR");
}
void ErrExit(int error)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(error), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
    ReleaseResources();
    std::remove(tmp_file);
    exit(1);
}

void GameOver()
{
    PlaySound(NULL, NULL, NULL);
    KillTimer(bHwnd, bTimer);

    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}
void InitGame()
{
    wcscpy_s(current_player, L"THE WARRIOR");
    name_set = false;
    level = 1;
    score = 0;
    mins = 0;
    secs = 0;
    intruders = 2 + level;

    if (Catapult)
    {
        Catapult->Release();
        Catapult = nullptr;
    }
    if (!vEnemies.empty())
    {
        for (int i = 0; i < vEnemies.size(); i++)
            vEnemies[i]->Release();
    }
    vEnemies.clear();
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
        return (INT_PTR)(TRUE);
        break;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            if (GetDlgItemText(hwnd, IDC_NAME, current_player, 15) < 1)
            {
                wcscpy_s(current_player, L"THE WARRIOR");
                if (sound)MessageBeep(MB_ICONEXCLAMATION);
                MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            EndDialog(hwnd, IDOK);
        }
        break;
    }

    return (INT_PTR)FALSE;
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        srand((unsigned int)(time(0)));
        SetTimer(hwnd, bTimer, 1000, NULL);

        bBar = CreateMenu();
        bMain = CreateMenu();
        bStore = CreateMenu();

        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

        AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
        AppendMenu(bMain, MF_STRING, mLevel, L"Турбо режим");
        AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bMain, MF_STRING, mExit, L"Изход");

        AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
        AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
        AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");
        SetMenu(hwnd, bBar);
        InitGame();
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBox(hwnd, L"Ако излезеш, ще загубиш тази игра !\n\nНаистина ли излизаш ?",
            L"Изход !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(50, 50, 50)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_TIMER:
        if (pause)break;
        secs++;
        mins = secs / 60;
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }
            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1TxtR.left && cur_pos.x <= b1TxtR.right)
                {
                    if (!b1Hglt && !name_set)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                    }
                    if (b2Hglt || b3Hglt)
                    {
                        b2Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b2TxtR.left && cur_pos.x <= b2TxtR.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b2Hglt = true;
                    }
                    if (b1Hglt || b3Hglt)
                    {
                        b1Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b3TxtR.left && cur_pos.x <= b3TxtR.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b3Hglt = true;
                    }
                    if (b1Hglt || b2Hglt)
                    {
                        b1Hglt = false;
                        b2Hglt = false;
                    }
                }
                SetCursor(outCursor);
                return true;
            }
            else
            {
                if (b1Hglt || b2Hglt || b3Hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b1Hglt = false;
                    b2Hglt = false;
                    b3Hglt = false;
                }
                SetCursor(mainCursor);
                return true;
            }
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }
            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
                SetCursor(mainCursor);
                return true;
            }
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)MessageBeep(MB_ICONEXCLAMATION);
            if (MessageBox(hwnd, L"Ако рестартираш, ще загубиш тази игра !\n\nНаистина ли рестартираш ?",
                L"Рестарт !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

        case mLevel:
            pause = true;
            if (sound)MessageBeep(MB_ICONEXCLAMATION);
            if (MessageBox(hwnd, L"Наистина ли минаваш на следващото ниво ?",
                L"Турбо режим !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;


        }
        break;

    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void CreateResources()
{
    int result = 0;
    CheckFile(Ltmp_file, &result);
    if (result == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream tmp(Ltmp_file);
        tmp << L"Game started at " << std::chrono::system_clock::now();
        tmp.close();
    }
    
    int window_x = (int)((int)(GetSystemMetrics(SM_CXSCREEN) / 2) - (int)(scr_width / 2));
    if (GetSystemMetrics(SM_CXSCREEN) < window_x + scr_width || GetSystemMetrics(SM_CYSCREEN) < 810)ErrExit(eScreen);

    mainIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 255, 255, LR_LOADFROMFILE));
    if (!mainIcon)ErrExit(eIcon);
    mainCursor = LoadCursorFromFile(L".\\res\\main.ani");
    outCursor = LoadCursorFromFile(L".\\res\\out.ani");
    if (!mainCursor || !outCursor)ErrExit(eCursor);

    bWin.lpszClassName = bWinClassName;
    bWin.hInstance = bIns;
    bWin.lpfnWndProc = &WinProc;
    bWin.hbrBackground = CreateSolidBrush(RGB(50,50,50));
    bWin.hIcon = mainIcon;
    bWin.hCursor = mainCursor;
    bWin.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWin))ErrExit(eClass);

    bHwnd = CreateWindowW(bWinClassName, L"ИЗВЪНЗЕМНИТЕ АТАКУВАТ !", WS_CAPTION | WS_SYSMENU, window_x, 10, (int)(scr_width),
        (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else ShowWindow(bHwnd, SW_SHOWDEFAULT);

    HRESULT hr = S_OK;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating iFactory");
        ErrExit(eD2D);
    }

    if (iFactory && bHwnd)
        hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(bHwnd, D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating Hwnd render target");
        ErrExit(eD2D);
    }

    D2D1_GRADIENT_STOP gStops[2] = { 0 };
    ID2D1GradientStopCollection* gCollection = nullptr;

    gStops[0].position = 0;
    gStops[0].color = D2D1::ColorF(D2D1::ColorF::BlanchedAlmond);
    gStops[1].position = 1.0f;
    gStops[1].color = D2D1::ColorF(D2D1::ColorF::Brown);

    if (Draw)
    {
        hr = Draw->CreateGradientStopCollection(gStops, 2, &gCollection);
        if (hr != S_OK)
        {
            ErrLog(L"Error creating Gradient stop collection");
            ErrExit(eD2D);
        }

        if (gCollection)
            hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(scr_width / 2, 25.0f),
                D2D1::Point2F(0, 0), scr_width / 2, 25.0f), gCollection, &ButBckgBrush);
        if (hr != S_OK)
        {
            ErrLog(L"Error creating ButBckgBrush");
            ErrExit(eD2D);
        }
        CleanMem(&gCollection);

        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkBlue), &TxtBrush);
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &HgltBrush);
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &InactBrush);

        if (hr != S_OK)
        {
            ErrLog(L"Error creating SolidColorBrushes");
            ErrExit(eD2D);
        }

    }

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&iWriteFactory));
    if (hr != S_OK)
    {
        ErrLog(L"Error creating iWriteFactory");
        ErrExit(eD2D);
    }

    if (iWriteFactory)
    {
        hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_BLACK,
            DWRITE_FONT_STYLE_OBLIQUE, DWRITE_FONT_STRETCH_NORMAL, 20, L"", &nrmText);
        if (hr != S_OK)
        {
            ErrLog(L"Error creating nrmTextFormat");
            ErrExit(eD2D);
        }

        hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_BLACK,
            DWRITE_FONT_STYLE_OBLIQUE, DWRITE_FONT_STRETCH_EXTRA_EXPANDED, 64, L"", &bigText);
        if (hr != S_OK)
        {
            ErrLog(L"Error creating nrmTextFormat");
            ErrExit(eD2D);
        }
    }

    bmpField = Load(L".\\res\\img\\field.png", Draw);
    if (!bmpField)
    {
        ErrLog(L"Error loading bmpField");
        ErrExit(eD2D);
    }

    bmpAxe = Load(L".\\res\\img\\axe.png", Draw);
    if (!bmpAxe)
    {
        ErrLog(L"Error loading bmpAxe");
        ErrExit(eD2D);
    }

    bmpBlueBall = Load(L".\\res\\img\\balls\\BallBlue.png", Draw);
    if (!bmpBlueBall)
    {
        ErrLog(L"Error loading bmpBlueBall");
        ErrExit(eD2D);
    }
    bmpRedBall = Load(L".\\res\\img\\balls\\BallRed.png", Draw);
    if (!bmpRedBall)
    {
        ErrLog(L"Error loading bmpRedBall");
        ErrExit(eD2D);
    }
    bmpGreenBall = Load(L".\\res\\img\\balls\\BallGreen.png", Draw);
    if (!bmpGreenBall)
    {
        ErrLog(L"Error loading bmpGreenBall");
        ErrExit(eD2D);
    }
    bmpYellowBall = Load(L".\\res\\img\\balls\\BallYellow.png", Draw);
    if (!bmpYellowBall)
    {
        ErrLog(L"Error loading bmpYellowBall");
        ErrExit(eD2D);
    }
    bmpPurpleBall = Load(L".\\res\\img\\balls\\BallPurple.png", Draw);
    if (!bmpPurpleBall)
    {
        ErrLog(L"Error loading bmpPurpleBall");
        ErrExit(eD2D);
    }

    bmpMidBlueBall = Load(L".\\res\\img\\balls\\MidBallBlue.png", Draw);
    if (!bmpMidBlueBall)
    {
        ErrLog(L"Error loading bmpMidBlueBall");
        ErrExit(eD2D);
    }
    bmpMidRedBall = Load(L".\\res\\img\\balls\\MidBallRed.png", Draw);
    if (!bmpMidRedBall)
    {
        ErrLog(L"Error loading bmpMidRedBall");
        ErrExit(eD2D);
    }
    bmpMidGreenBall = Load(L".\\res\\img\\balls\\MidBallGreen.png", Draw);
    if (!bmpMidGreenBall)
    {
        ErrLog(L"Error loading bmpMidGreenBall");
        ErrExit(eD2D);
    }
    bmpMidYellowBall = Load(L".\\res\\img\\balls\\MidBallYellow.png", Draw);
    if (!bmpMidYellowBall)
    {
        ErrLog(L"Error loading bmpMidYellowBall");
        ErrExit(eD2D);
    }
    bmpMidPurpleBall = Load(L".\\res\\img\\balls\\MidBallPurple.png", Draw);
    if (!bmpMidPurpleBall)
    {
        ErrLog(L"Error loading bmpMidPurpleBall");
        ErrExit(eD2D);
    }

    bmpSmallBlueBall = Load(L".\\res\\img\\balls\\SmBallBlue.png", Draw);
    if (!bmpSmallBlueBall)
    {
        ErrLog(L"Error loading bmpSmallBlueBall");
        ErrExit(eD2D);
    }
    bmpSmallRedBall = Load(L".\\res\\img\\balls\\SmBallRed.png", Draw);
    if (!bmpSmallRedBall)
    {
        ErrLog(L"Error loading bmpSmallRedBall");
        ErrExit(eD2D);
    }
    bmpSmallGreenBall = Load(L".\\res\\img\\balls\\SmBallGreen.png", Draw);
    if (!bmpSmallGreenBall)
    {
        ErrLog(L"Error loading bmpSmallGreenBall");
        ErrExit(eD2D);
    }
    bmpSmallYellowBall = Load(L".\\res\\img\\balls\\SmBallYellow.png", Draw);
    if (!bmpSmallYellowBall)
    {
        ErrLog(L"Error loading bmpSmallYellowBall");
        ErrExit(eD2D);
    }
    bmpSmallPurpleBall = Load(L".\\res\\img\\balls\\SmBallPurple.png", Draw);
    if (!bmpSmallPurpleBall)
    {
        ErrLog(L"Error loading bmpSmallPurpleBall");
        ErrExit(eD2D);
    }
    
    /////////////////////////////////////////////////////////////////////////////

    bmpBlueEgg = Load(L".\\res\\img\\balls\\EggBlue.png", Draw);
    if (!bmpBlueEgg)
    {
        ErrLog(L"Error loading bmpBlueEgg");
        ErrExit(eD2D);
    }
    bmpRedEgg = Load(L".\\res\\img\\balls\\EggRed.png", Draw);
    if (!bmpRedEgg)
    {
        ErrLog(L"Error loading bmpRedEgg");
        ErrExit(eD2D);
    }
    bmpGreenEgg = Load(L".\\res\\img\\balls\\EggGreen.png", Draw);
    if (!bmpGreenEgg)
    {
        ErrLog(L"Error loading bmpGreenEgg");
        ErrExit(eD2D);
    }
    bmpYellowEgg = Load(L".\\res\\img\\balls\\EggYellow.png", Draw);
    if (!bmpYellowEgg)
    {
        ErrLog(L"Error loading bmpYellowEgg");
        ErrExit(eD2D);
    }
    bmpPurpleEgg = Load(L".\\res\\img\\balls\\EggPurple.png", Draw);
    if (!bmpPurpleEgg)
    {
        ErrLog(L"Error loading bmpPurpleEgg");
        ErrExit(eD2D);
    }

    bmpMidBlueEgg = Load(L".\\res\\img\\balls\\MidEggBlue.png", Draw);
    if (!bmpMidBlueEgg)
    {
        ErrLog(L"Error loading bmpMidBlueEgg");
        ErrExit(eD2D);
    }
    bmpMidRedEgg = Load(L".\\res\\img\\balls\\MidEggRed.png", Draw);
    if (!bmpMidRedEgg)
    {
        ErrLog(L"Error loading bmpMidRedEgg");
        ErrExit(eD2D);
    }
    bmpMidGreenEgg = Load(L".\\res\\img\\balls\\MidEggGreen.png", Draw);
    if (!bmpMidGreenEgg)
    {
        ErrLog(L"Error loading bmpMidGreenEgg");
        ErrExit(eD2D);
    }
    bmpMidYellowEgg = Load(L".\\res\\img\\balls\\MidEggYellow.png", Draw);
    if (!bmpMidYellowEgg)
    {
        ErrLog(L"Error loading bmpMidYellowEgg");
        ErrExit(eD2D);
    }
    bmpMidPurpleEgg = Load(L".\\res\\img\\balls\\MidEggPurple.png", Draw);
    if (!bmpMidPurpleEgg)
    {
        ErrLog(L"Error loading bmpMidPurpleEgg");
        ErrExit(eD2D);
    }

    bmpSmallBlueEgg = Load(L".\\res\\img\\balls\\SmEggBlue.png", Draw);
    if (!bmpSmallBlueEgg)
    {
        ErrLog(L"Error loading bmpSmallBlueEgg");
        ErrExit(eD2D);
    }
    bmpSmallRedEgg = Load(L".\\res\\img\\balls\\SmEggRed.png", Draw);
    if (!bmpSmallRedEgg)
    {
        ErrLog(L"Error loading bmpSmallRedEgg");
        ErrExit(eD2D);
    }
    bmpSmallGreenEgg = Load(L".\\res\\img\\balls\\SmEggGreen.png", Draw);
    if (!bmpSmallGreenEgg)
    {
        ErrLog(L"Error loading bmpSmallGreenEgg");
        ErrExit(eD2D);
    }
    bmpSmallYellowEgg = Load(L".\\res\\img\\balls\\SmEggYellow.png", Draw);
    if (!bmpSmallYellowEgg)
    {
        ErrLog(L"Error loading bmpSmallYellowEgg");
        ErrExit(eD2D);
    }
    bmpSmallPurpleEgg = Load(L".\\res\\img\\balls\\SmEggPurple.png", Draw);
    if (!bmpSmallPurpleEgg)
    {
        ErrLog(L"Error loading bmpSmallPurpleEgg");
        ErrExit(eD2D);
    }

    for (int i = 0; i < 3; i++)
    {
        wchar_t name[50]= L".\\res\\img\\catapultL\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");
        bmpCatapultL[i] = Load(name, Draw);
        if (!bmpCatapultL[i])
        {
            ErrLog(L"Error loading bmpCatapultL");
            ErrExit(eD2D);
        }
    }

    for (int i = 0; i < 3; i++)
    {
        wchar_t name[50] = L".\\res\\img\\catapultR\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");
        bmpCatapultR[i] = Load(name, Draw);
        if (!bmpCatapultR[i])
        {
            ErrLog(L"Error loading bmpCatapultR");
            ErrExit(eD2D);
        }
    }

    D2D1_RECT_F topR = { scr_width / 2 - 200.0f,-50.0f,scr_width,50.0f };
    D2D1_RECT_F bottomR = { scr_width / 2 - 150.0f,scr_height + 50.0f,scr_width,scr_height + 150.0f };
    bool top_ok = false;
    bool bottom_ok = false;

    wchar_t up_txt[26] = L"НАПАДЕНИЕ НА ИЗВЪНЗЕМНИ !";
    wchar_t down_txt[14] = L"dev. Daniel !";


    mciSendString(L"play .\\res\\snd\\intro.wav", NULL, NULL, NULL);
    while (!top_ok || !bottom_ok)
    {
        if (topR.bottom <= scr_height / 2 - 50.0f)
        {
            topR.top += 3.0f;
            topR.bottom += 3.0f;
        }
        else top_ok = true;

        if (bottomR.bottom >= scr_height / 2 + 150.0f)
        {
            bottomR.top -= 3.0f;
            bottomR.bottom -= 3.0f;
        }
        else bottom_ok = true;

        if (bigText && TxtBrush)
        {
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::Azure));
            Draw->DrawTextW(up_txt, 26, bigText, topR, TxtBrush);
            Draw->DrawTextW(down_txt, 14, bigText, bottomR, TxtBrush);
            Draw->EndDraw();
        }
    }
    Sleep(1500);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)ErrExit(eClass);

    CreateResources();

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessageW(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            if (bigText && TxtBrush)
            {
                Draw->BeginDraw();
                Draw->Clear(D2D1::ColorF(D2D1::ColorF::Azure));
                Draw->DrawTextW(L"ПАУЗА", 6, bigText, D2D1::RectF(scr_width / 2 - 50.0f, scr_height / 2 - 50.0f, scr_width, scr_height),
                    TxtBrush);
                Draw->EndDraw();
            }
            continue;
        }
        ///////////////////////////////////////////////////////

        //EVILS ************************

        if (intruders > 0)
        {
            if (rand() % 300 == 66)
            {
                if (rand() % 2 == 0)
                    vEnemies.push_back(dll::Factory(types::ball, (float)(rand() % (int)(scr_width - 80)), 60.0f));
                else
                    vEnemies.push_back(dll::Factory(types::egg, (float)(rand() % (int)(scr_width - 80)), 60.0f));
                intruders--;
            }
        }
        if (!vEnemies.empty())
        {
            for (std::vector<dll::Object>::iterator evil = vEnemies.begin(); evil < vEnemies.end(); evil++)
            {
                (*evil)->Move((float)(level));
            }
        }





        //DRAW THINGS *******************************************

        if (Draw && nrmText && TxtBrush && InactBrush && HgltBrush && ButBckgBrush)
        {
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::Azure));
            Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), ButBckgBrush);
            if (name_set)
                Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtR, InactBrush);
            else
            {
                if (b1Hglt)
                    Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtR, HgltBrush);
                else
                    Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtR, TxtBrush);
            }
            if (b2Hglt)
                Draw->DrawTextW(L"ЗВУЦИ ON /OFF", 14, nrmText, b2TxtR, HgltBrush);
            else
                Draw->DrawTextW(L"ЗВУЦИ ON /OFF", 14, nrmText, b2TxtR, TxtBrush);
            if (b3Hglt)
                Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtR, HgltBrush);
            else
                Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtR, TxtBrush);

        }

        Draw->DrawBitmap(bmpField, D2D1::RectF(0, 50.0f, scr_width, scr_height - 100.0f));
        //////////////////////////////////////////////////////////

        //EVILS ***************************************

        for (std::vector<dll::Object>::iterator evil = vEnemies.begin(); evil < vEnemies.end(); evil++)
        {
            switch ((*evil)->GetType())
            {
            case types::egg:
                if ((*evil)->size == sizes::big)
                {
                    if ((*evil)->color == colors::blue)
                        Draw->DrawBitmap(bmpBlueEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::red)
                        Draw->DrawBitmap(bmpRedEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::yellow)
                        Draw->DrawBitmap(bmpYellowEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::green)
                        Draw->DrawBitmap(bmpGreenEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::purple)
                        Draw->DrawBitmap(bmpPurpleEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                }
                else if ((*evil)->size == sizes::middle)
                {
                    if ((*evil)->color == colors::blue)
                        Draw->DrawBitmap(bmpMidBlueEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::red)
                        Draw->DrawBitmap(bmpMidRedEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::yellow)
                        Draw->DrawBitmap(bmpMidYellowEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::green)
                        Draw->DrawBitmap(bmpMidGreenEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::purple)
                        Draw->DrawBitmap(bmpMidPurpleEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                }
                else if ((*evil)->size == sizes::small_ball)
                {
                    if ((*evil)->color == colors::blue)
                        Draw->DrawBitmap(bmpSmallBlueEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::red)
                        Draw->DrawBitmap(bmpSmallRedEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::yellow)
                        Draw->DrawBitmap(bmpSmallYellowEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::green)
                        Draw->DrawBitmap(bmpSmallGreenEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::purple)
                        Draw->DrawBitmap(bmpSmallPurpleEgg, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                }
                break;

            case types::ball:
                if ((*evil)->size == sizes::big)
                {
                    if ((*evil)->color == colors::blue)
                        Draw->DrawBitmap(bmpBlueBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::red)
                        Draw->DrawBitmap(bmpRedBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::yellow)
                        Draw->DrawBitmap(bmpYellowBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::green)
                        Draw->DrawBitmap(bmpGreenBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::purple)
                        Draw->DrawBitmap(bmpPurpleBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                }
                else if ((*evil)->size == sizes::middle)
                {
                    if ((*evil)->color == colors::blue)
                        Draw->DrawBitmap(bmpMidBlueBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::red)
                        Draw->DrawBitmap(bmpMidRedBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::yellow)
                        Draw->DrawBitmap(bmpMidYellowBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::green)
                        Draw->DrawBitmap(bmpMidGreenBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::purple)
                        Draw->DrawBitmap(bmpMidPurpleBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                }
                else if ((*evil)->size == sizes::small_ball)
                {
                    if ((*evil)->color == colors::blue)
                        Draw->DrawBitmap(bmpSmallBlueBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::red)
                        Draw->DrawBitmap(bmpSmallRedBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::yellow)
                        Draw->DrawBitmap(bmpSmallYellowBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::green)
                        Draw->DrawBitmap(bmpSmallGreenBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    if ((*evil)->color == colors::purple)
                        Draw->DrawBitmap(bmpSmallPurpleBall, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                }
                break;
            }
        }



        /////////////////////////////////////////////////////////////
        Draw->EndDraw();

    }

    std::remove(tmp_file);
    ReleaseResources();
    return (int) bMsg.wParam;
}