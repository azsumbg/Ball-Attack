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
#define snd_file L".\\res\\snd\\main.wav"

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
bool now_shooting = false;

int level = 1;
int mins = 0;
int secs = 0;
int score = 0;
int lifes = 150;
int intruders = 0;

float target_x = 0;
float target_y = 0;

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

ID2D1SolidColorBrush* LifeBrush = nullptr;
ID2D1SolidColorBrush* HurtBrush = nullptr;
ID2D1SolidColorBrush* DangerBrush = nullptr;

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
ID2D1Bitmap* bmpTools = nullptr;

ID2D1Bitmap* bmpCatapultL[3] = { nullptr };
ID2D1Bitmap* bmpCatapultR[3] = { nullptr };

ID2D1Bitmap* bmpExplosion[24] = { nullptr };
///////////////////////////////////////////////////////////////////

struct EXPLOSION
{
    float x = 0;
    float y = 0;
    float ex = 0;
    float ey = 0;
    int frame = 0;
    types victim = types::no_type;
};

dll::Object Catapult = nullptr;
std::vector<dll::Object> vEnemies;
std::vector<dll::Object> vAxes;

std::vector<EXPLOSION>vExplosions;

dll::ATOM* ToolSet = nullptr;
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
    if (!CleanMem(&InactBrush))ErrLog(L"Error releasing InactBrush");
    if (!CleanMem(&LifeBrush))ErrLog(L"Error releasing LifeBrush");
    if (!CleanMem(&HurtBrush))ErrLog(L"Error releasing HurtBrush");
    if (!CleanMem(&DangerBrush))ErrLog(L"Error releasing DangerBrush");
    
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
    if (!CleanMem(&bmpTools))ErrLog(L"Error releasing bmpTools");

    for (int i = 0; i < 3; i++)
        if (!CleanMem(&bmpCatapultL[i]))ErrLog(L"Error releasing CatapultL");
    for (int i = 0; i < 3; i++)
        if (!CleanMem(&bmpCatapultR[i]))ErrLog(L"Error releasing CatapultR");
    for (int i = 0; i < 24; i++)
        if (!CleanMem(&bmpExplosion[i]))ErrLog(L"Error releasing Explosion");
}
void ErrExit(int error)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(error), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
    ReleaseResources();
    std::remove(tmp_file);
    exit(1);
}
BOOL CheckRecord()
{
    if (score < 1)return no_record;

    int result = 0;
    CheckFile(record_file, &result);
    if (result == FILE_NOT_EXIST)
    {
        std::wofstream rec(record_file);
        rec << score << std::endl;
        for (int i = 0; i < 16; i++)rec << static_cast<int>(current_player[i]) << std::endl;
        rec.close();
        return first_record;
    }

    std::wifstream check(record_file);
    check >> result;
    check.close();

    if (result < score)
    {
        std::wofstream rec(record_file);
        rec << score << std::endl;
        for (int i = 0; i < 16; i++)rec << static_cast<int>(current_player[i]) << std::endl;
        rec.close();
        return record;
    }

    return no_record;
}
void GameOver()
{
    PlaySound(NULL, NULL, NULL);
    KillTimer(bHwnd, bTimer);

    wchar_t final_txt[31] = L"ИЗВЪНЗЕМНИТЕ ЗАВЛАДЯХА СВЕТА !";
    int size = 31;

    switch (CheckRecord())
    {
    case no_record:
        if (sound)PlaySound(L".\\res\\snd\\loose.wav", NULL, SND_ASYNC);
        break;

    case first_record:
        if (sound)PlaySound(L".\\res\\snd\\record.wav", NULL, SND_ASYNC);
        wcscpy_s(final_txt, L"ПЪРВИ РЕКОРД НА ИГРАТА !");
        size = 25;
        break;
        
    case record:
        if (sound)PlaySound(L".\\res\\snd\\record.wav", NULL, SND_ASYNC);
        wcscpy_s(final_txt, L"СВЕТОВЕН РЕКОРД НА ИГРАТА !");
        size = 28;
        break;

    }

    if (bigText && TxtBrush)
    {
        Draw->BeginDraw();
        Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkCyan));
        Draw->DrawTextW(final_txt, size, bigText, D2D1::RectF(50.0f, scr_height / 2 - 100.0f,
            scr_width, scr_height), TxtBrush);
        Draw->EndDraw();
    }
    Sleep(6800);

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
    secs = 240;
    intruders = 1 + level;
    lifes = 150;

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
    if (!vAxes.empty())
    {
        for (int i = 0; i < vAxes.size(); i++)
            vAxes[i]->Release();
    }
    vAxes.clear();
    if (ToolSet)
    {
        delete ToolSet;
        ToolSet = nullptr;
    }

    vExplosions.clear();

    Catapult = dll::Factory(types::catapult, scr_width / 2 - 100.0f, scr_height - 185.0f);


}
void HallofFame()
{
    int result = 0;
    CheckFile(record_file, &result);
    if (result == FILE_NOT_EXIST)
    {
        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
        MessageBox(bHwnd, L"Все още няма рекорд на играта !\n\nПостарай се повече !",
            L"Липсва файл", MB_OK | MB_APPLMODAL | MB_ICONASTERISK);
        return;
    }

    wchar_t record_txt[100] = L"НАЙ-ВЕЛИК ГЕРОЙ: ";
    wchar_t saved_player[16] = L"\0";
    wchar_t add[5] = L"\0";
    int size = 0;

    std::wifstream rec(record_file);
    rec >> result;
    for (int i = 0; i < 16; i++)
    {
        int letter = 0;
        rec >> letter;
        saved_player[i] = static_cast<wchar_t>(letter);
    }
    rec.close();

    wcscat_s(record_txt, saved_player);
    wcscat_s(record_txt, L"\nСВЕТОВЕН РЕКОРД: ");
    wsprintf(add, L"%d", result);
    wcscat_s(record_txt, add);

    for (int i = 0; i < 100; i++)
    {
        if (record_txt[i] != '\0')size++;
        else break;
    }
    
    if (sound)mciSendString(L"play .\\res\\snd\\showrec.wav", NULL, NULL, NULL);
    if (bigText && TxtBrush)
    {
        Draw->BeginDraw();
        Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkCyan));
        Draw->DrawTextW(record_txt, size, bigText, D2D1::RectF(50.0f, scr_height / 2 - 100.0f,
            scr_width, scr_height), TxtBrush);
        Draw->EndDraw();
    }
    Sleep(4000);
}
void NextLevel()
{
    if (sound)mciSendString(L"play .\\res\\snd\\levelup.wav", NULL, NULL, NULL);

    int control = 0;
    int repeat_count = 0;
    
    if (bigText && TxtBrush)
    {
        while (repeat_count <= 5)
        {
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkCyan));
            if (control % 10 == 0)
            {
                repeat_count++;
                Draw->DrawTextW(L"НИВОТО ПРЕМИНАТО !", 19, bigText, D2D1::RectF(300.0f, scr_height / 2 - 100.0f,
                    scr_width, scr_height), TxtBrush);
                Draw->EndDraw();
                Sleep(250);
            }
            control++;
            Draw->EndDraw();
        }
    }

    level++;
    secs = 240;
    intruders = 1 + level;

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
    if (!vAxes.empty())
    {
        for (int i = 0; i < vAxes.size(); i++)
            vAxes[i]->Release();
    }
    vAxes.clear();
    if (ToolSet)
    {
        delete ToolSet;
        ToolSet = nullptr;
    }

    vExplosions.clear();

    Catapult = dll::Factory(types::catapult, scr_width / 2 - 100.0f, scr_height - 185.0f);
}
void SaveGame()
{
    int result = 0;

    CheckFile(save_file, &result);
    if (result == FILE_EXIST)
    {
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBox(bHwnd, L"Съществува предишна записана игра !\n\nДа я презапиша ли ?",
            L"Презапис !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            return;
        }
    }

    std::wofstream save(save_file);

    save << score << std::endl;
    save << level << std::endl;
    save << secs << std::endl;
    save << intruders << std::endl;
    save << lifes << std::endl;
    for (int i = 0; i < 16; i++)save << static_cast<int>(current_player[i]) << std::endl;
    save << name_set << std::endl;

    if(!Catapult)save << -1 << std::endl;
    else save << Catapult->x << std::endl;

    save << vEnemies.size() << std::endl;
    if (vEnemies.size() > 0)
    {
        for (int i = 0; i < vEnemies.size(); i++)
        {
            save << vEnemies[i]->x << std::endl;
            save << vEnemies[i]->y << std::endl;
            save << static_cast<int>(vEnemies[i]->GetType()) << std::endl;
            save << static_cast<int>(vEnemies[i]->size) << std::endl;
        }
    }

    save.close();

    if (sound)mciSendString(L"play .\\res\\snd\\save.wav", NULL, NULL, NULL); 
    MessageBox(bHwnd, L"Играта е запазена !", L"Запис !", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
}
void LoadGame()
{
    int result = 0;

    CheckFile(save_file, &result);
    if (result == FILE_EXIST)
    {
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBox(bHwnd, L"Ако продължиш, ще загубиш тази игра !\n\nДа я презапиша ли ?",
            L"Презапис !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            return;
        }
    }
    else
    {
        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
        MessageBox(bHwnd, L"Все още няма записана игра !\n\nПостарай се повече !",
            L"Липсва файл", MB_OK | MB_APPLMODAL | MB_ICONASTERISK);
        return;
    }

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
    if (!vAxes.empty())
    {
        for (int i = 0; i < vAxes.size(); i++)
            vAxes[i]->Release();
    }
    vAxes.clear();
    if (ToolSet)
    {
        delete ToolSet;
        ToolSet = nullptr;
    }
    
    vExplosions.clear();

    std::wifstream save(save_file);

    save >> score;
    save >> level;
    save >> secs;
    save >> intruders;
    save >> lifes;
    for (int i = 0; i < 16; i++)
    {
        int letter = 0;
        save >> letter; 
        current_player[i] = static_cast<wchar_t>(letter);
    }
    save >> name_set;

    float temp_x = 0;
    
    save >> temp_x;
    if (temp_x >= 0)Catapult = dll::Factory(types::catapult, temp_x, scr_height - 185.0f);
    
    save >> result;
    if (result > 0)
    {
        for (int i = 0; i < result; i++)
        {
            float ax = 0;
            float ay = 0;
            int atype = -1;
            int asize = -1;

            save >> ax;
            save >> ay;
            save >> atype;
            save >> asize;

            vEnemies.push_back(dll::Factory(static_cast<types>(atype), ax, ay));
            if (static_cast<sizes>(asize) != sizes::big)vEnemies.back()->Transform(static_cast<sizes>(asize));
        }
    }

    save.close();

    if (sound)mciSendString(L"play .\\res\\snd\\save.wav", NULL, NULL, NULL);
    MessageBox(bHwnd, L"Играта е заредена !", L"Зареждане !", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
}
void ShowHelp()
{
    int result = 0;
    CheckFile(help_file, &result);
    if (result == FILE_NOT_EXIST)
    {
        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
        MessageBox(bHwnd, L"Няма налична помощ за играта !\n\nСвържете се с разработчика !",
            L"Липсва файл", MB_OK | MB_APPLMODAL | MB_ICONASTERISK);
        return;
    }

    wchar_t help_txt[1000] = L"\0";
    
    std::wifstream rec(help_file);
    rec >> result;
    for (int i = 0; i < result; i++)
    {
        int letter = 0;
        rec >> letter;
        help_txt[i] = static_cast<wchar_t>(letter);
    }
    rec.close();

    if (sound)mciSendString(L"play .\\res\\snd\\help.wav", NULL, NULL, NULL);

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

        Draw->DrawTextW(help_txt, result, nrmText, D2D1::RectF(150.0f, 100.0f, scr_width, scr_height), TxtBrush);
        Draw->EndDraw();
    }
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
        secs--;
        mins = secs / 60;
        if (secs <= 0)
        {
            pause = true;
            NextLevel();
            pause = false;
        }
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
            NextLevel();
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;

        case mSave:
            pause = true;
            SaveGame();
            pause = false;
            break;

        case mLoad:
            pause = true;
            LoadGame();
            pause = false;
            break;

        case mHoF:
            pause = true;
            HallofFame();
            pause = false;
            break;
        }
        break;

    case WM_LBUTTONDOWN:
        if (HIWORD(lParam) > 50)
        {
            if (!now_shooting)
            {
                now_shooting = true;
                target_x = LOWORD(lParam);
                target_y = HIWORD(lParam);
                if (sound)mciSendString(L"play .\\res\\snd\\shoot.wav", NULL, NULL, NULL);
            }
        }
        else
        {
            if (LOWORD(lParam) >= b1TxtR.left && LOWORD(lParam) <= b1TxtR.right)
            {
                if (name_set)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                    break;
                }
                else
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                    if (DialogBox(bIns, MAKEINTRESOURCE(IDD_PLAYER), hwnd, &DlgProc) == IDOK)name_set = true;
                }
                break;
            }
            if (LOWORD(lParam) >= b2TxtR.left && LOWORD(lParam) <= b2TxtR.right)
            {
                mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                if (sound)
                {
                    sound = false;
                    PlaySound(NULL, NULL, NULL);
                    break;
                }
                else
                {
                    sound = true;
                    PlaySound(snd_file, NULL, SND_ASYNC | SND_LOOP);
                    break;
                }
            }
            if (LOWORD(lParam) >= b3TxtR.left && LOWORD(lParam) <= b3TxtR.right)
            {
                if(sound) mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                if (!show_help)
                {
                    show_help = true;
                    pause = true;
                    ShowHelp();
                    break;
                }
                else
                {
                    show_help = false;
                    pause = false;
                    break;
                }
            }
        }
        break;

    case WM_KEYDOWN:
        if (Catapult)
        {
            if (wParam == VK_LEFT)
            {
                Catapult->SetDir(dirs::left);
                Catapult->Move((float)(level));
                break;
            }
            if (wParam == VK_RIGHT)
            {
                Catapult->SetDir(dirs::right);
                Catapult->Move((float)(level));
                break;
            }
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
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &LifeBrush);
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &HurtBrush);
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &DangerBrush);

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
            DWRITE_FONT_STYLE_OBLIQUE, DWRITE_FONT_STRETCH_NORMAL, 28, L"", &nrmText);
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

    bmpTools = Load(L".\\res\\img\\tools.png", Draw);
    if (!bmpTools)
    {
        ErrLog(L"Error loading bmpTools");
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

    for (int i = 0; i < 24; i++)
    {
        wchar_t name[50] = L".\\res\\img\\explosion\\";
        wchar_t add[3] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".png");
        bmpExplosion[i] = Load(name, Draw);
        if (!bmpExplosion[i])
        {
            ErrLog(L"Error loading bmpExplosion");
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

    PlaySound(NULL, NULL, NULL);
    PlaySound(snd_file, NULL, SND_ASYNC | SND_LOOP);

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
            if (rand() % 250 == 66)
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

        //AXES ************************

        if (!vAxes.empty())
        {
            for (std::vector<dll::Object>::iterator axe = vAxes.begin(); axe < vAxes.end(); axe++)
            {
                (*axe)->Move((float)(level));
                if ((*axe)->x >= scr_width || (*axe)->x <= 0 || (*axe)->y >= scr_height - 100.0f || (*axe)->y <= 50.0f)
                {
                    (*axe)->Release();
                    vAxes.erase(axe);
                    break;
                }
            }
        }

        if (!vAxes.empty() && !vEnemies.empty())
        {
            for (std::vector<dll::Object>::iterator evil = vEnemies.begin(); evil < vEnemies.end(); evil++)
            {
                bool hurt = false;
                for (std::vector<dll::Object>::iterator axe = vAxes.begin(); axe < vAxes.end(); axe++)
                {
                    if (!((*axe)->x >= (*evil)->ex || (*axe)->ex <= (*evil)->x
                        || (*axe)->y >= (*evil)->ey || (*axe)->ey <= (*evil)->y))
                    {
                        score +=  level;
                        
                        dirs new_evil_dir1 = (*evil)->GetDir();
                        dirs new_evil_dir2 = (*evil)->GetDir();
                        dirs new_evil_dir3 = (*evil)->GetDir();

                        float now_x = (*evil)->x;
                        float now_y = (*evil)->y;

                        switch (new_evil_dir1)
                        {
                        case dirs::up:
                            new_evil_dir1 = dirs::down;
                            new_evil_dir2 = dirs::down_left;
                            new_evil_dir3 = dirs::down_right;
                            break;

                        case dirs::down:
                            new_evil_dir1 = dirs::up;
                            new_evil_dir2 = dirs::up_left;
                            new_evil_dir3 = dirs::up_right;
                            break;

                        case dirs::left:
                            new_evil_dir1 = dirs::right;
                            new_evil_dir2 = dirs::up_right;
                            new_evil_dir3 = dirs::down_right;
                            break;

                        case dirs::down_left:
                            new_evil_dir1 = dirs::up_right;
                            new_evil_dir2 = dirs::up;
                            new_evil_dir3 = dirs::down_right;
                            break;

                        case dirs::down_right:
                            new_evil_dir1 = dirs::up_left;
                            new_evil_dir2 = dirs::up;
                            new_evil_dir3 = dirs::down_left;
                            break;

                        case dirs::up_left:
                            new_evil_dir1 = dirs::down_right;
                            new_evil_dir2 = dirs::up_right;
                            new_evil_dir3 = dirs::left;
                            break;

                        case dirs::up_right:
                            new_evil_dir1 = dirs::down_left;
                            new_evil_dir2 = dirs::right;
                            new_evil_dir3 = dirs::down_right;
                            break;
                        }
                        
                        switch ((*evil)->GetType())
                        {
                        case types::ball:
                            switch ((*evil)->size)
                            {
                            case sizes::big:
                                (*evil)->Transform(sizes::middle);
                                vEnemies.push_back(dll::Factory(types::ball, now_x, now_y));
                                vEnemies.back()->Transform(sizes::middle);
                                vEnemies.back()->SetDir(new_evil_dir1);
                                if (intruders < 1 + level)intruders++;
                                break;

                            case sizes::middle:
                                (*evil)->Transform(sizes::small_ball);
                                vEnemies.push_back(dll::Factory(types::ball, now_x, now_y));
                                vEnemies.back()->Transform(sizes::small_ball);
                                vEnemies.back()->SetDir(new_evil_dir1);
                                break;

                            case sizes::small_ball:
                                vExplosions.push_back(EXPLOSION{ now_x,now_y,now_x + 100.0f,now_y + 114.0f,
                                    0,types::ball });
                                if (rand() % 15 == 1 && !ToolSet)
                                    ToolSet = new dll::ATOM(now_x, now_y - 50.0f, 60.0f, 54.0f);
                                (*evil)->Release();
                                vEnemies.erase(evil);
                                score += 10 + level;
                                if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                                break;
                            }
                            break;

                        case types::egg:
                            switch ((*evil)->size)
                            {
                            case sizes::big:
                                (*evil)->Transform(sizes::middle);
                                if (intruders < 1 + level)intruders++;
                                vEnemies.push_back(dll::Factory(types::egg, now_x, now_y));
                                vEnemies.back()->Transform(sizes::middle);
                                vEnemies.back()->SetDir(new_evil_dir1);

                                vEnemies.push_back(dll::Factory(types::egg, now_x, now_y));
                                vEnemies.back()->Transform(sizes::middle);
                                vEnemies.back()->SetDir(new_evil_dir2);

                                vEnemies.push_back(dll::Factory(types::egg, now_x, now_y));
                                vEnemies.back()->Transform(sizes::middle);
                                vEnemies.back()->SetDir(new_evil_dir3);
                                break;

                            case sizes::middle:
                                (*evil)->Transform(sizes::small_ball);
                                
                                vEnemies.push_back(dll::Factory(types::egg, now_x, now_y));
                                vEnemies.back()->Transform(sizes::small_ball);
                                vEnemies.back()->SetDir(new_evil_dir1);

                                vEnemies.push_back(dll::Factory(types::egg, now_x, now_y));
                                vEnemies.back()->Transform(sizes::small_ball);
                                vEnemies.back()->SetDir(new_evil_dir2);
                                
                                vEnemies.push_back(dll::Factory(types::egg, now_x, now_y));
                                vEnemies.back()->Transform(sizes::small_ball);
                                vEnemies.back()->SetDir(new_evil_dir3);
                                break;

                            case sizes::small_ball:
                                vExplosions.push_back(EXPLOSION{ now_x,now_y,now_x + 100.0f,now_y + 114.0f,
                                    0,types::ball });
                                if (rand() % 15 == 1 && !ToolSet)
                                    ToolSet = new dll::ATOM(now_x, now_y - 50.0f, 60.0f, 54.0f);
                                (*evil)->Release();
                                vEnemies.erase(evil);
                                score += 20 + level;
                                if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                                break;
                            }
                            break;
                        }
                        
                        (*axe)->Release();
                        vAxes.erase(axe);
                        hurt = true;
                        break;
                    }
                }
                if (hurt)break;
            }
        }

        if (!vEnemies.empty() && Catapult)
        {
            for (std::vector<dll::Object>::iterator evil = vEnemies.begin(); evil < vEnemies.end(); ++evil)
            {
                if (!(Catapult->x > (*evil)->ex || Catapult->ex<(*evil)->x ||
                    Catapult->y>(*evil)->ey || Catapult->ey < (*evil)->y))
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                    vExplosions.push_back(EXPLOSION{ (*evil)->x,(*evil)->y,(*evil)->x + 100.0f,
                        (*evil)->y + 114.0f,0,types::ball });
                    lifes -= 10 * level;

                    if (lifes <= 0)
                    {
                        vExplosions.push_back(EXPLOSION{ Catapult->x,Catapult->y,Catapult->x + 100.0f,
                        Catapult->y + 114.0f,0,types::catapult });
                        if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                        Catapult->Release();
                        Catapult = nullptr;
                    }
                    if (intruders < 1 + level)intruders++;
                    (*evil)->Release();
                    vEnemies.erase(evil);

                    break;
                }
            }
        }

        if (ToolSet)
        {
            ToolSet->y += level * 0.5f;
            ToolSet->SetEdges();
            if (ToolSet->ey >= scr_height - 100.0f)
            {
                delete ToolSet;
                ToolSet = nullptr;
            }
        }

        if (ToolSet && Catapult)
        {
            if (!(ToolSet->x >= Catapult->ex || ToolSet->ex <= Catapult->x
                || ToolSet->y >= Catapult->ey || ToolSet->ey <= Catapult->y))
            {
                if (sound)mciSendString(L"play .\\res\\snd\\life.wav", NULL, NULL, NULL);
                delete ToolSet;
                ToolSet = nullptr;
                if (lifes < 150)
                {
                    if (lifes + 50 <= 150)lifes += 50;
                    else lifes = 150;
                }
                else score += 50;
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

        //STATUS ************************************************

        if (Catapult && LifeBrush && HurtBrush && DangerBrush)
        {
            if (lifes >= 80)
                Draw->DrawLine(D2D1::Point2F(Catapult->x - 20.0f, Catapult->ey + 5.0f),
                    D2D1::Point2F(Catapult->x + (float)(lifes), Catapult->ey + 5.0f), LifeBrush, 10.0f);
            else if (lifes >= 40)
                Draw->DrawLine(D2D1::Point2F(Catapult->x - 20.0f, Catapult->ey + 5.0f),
                    D2D1::Point2F(Catapult->x + (float)(lifes), Catapult->ey + 5.0f), HurtBrush, 10.0f);
            else
                Draw->DrawLine(D2D1::Point2F(Catapult->x - 20.0f, Catapult->ey + 5.0f),
                    D2D1::Point2F(Catapult->x + (float)(lifes), Catapult->ey + 5.0f), DangerBrush, 10.0f);
        }

        if (TxtBrush && nrmText)
        {
            wchar_t status[200] = L"КАПИТАН: ";
            wchar_t add[8] = L"\0";
            int txt_size = 0;

            wcscat_s(status, current_player);

            wcscat_s(status, L", ВРЕМЕ: 0");
            wsprintf(add, L"%d", mins);
            wcscat_s(status, add);
            wcscat_s(status, L" : ");

            if (secs - mins * 60 < 10) wcscat_s(status, L"0");
            wsprintf(add, L"%d", secs - mins * 60);
            wcscat_s(status, add);

            wcscat_s(status, L", НИВО: ");
            wsprintf(add, L"%d", level);
            wcscat_s(status, add);

            wcscat_s(status, L", РЕЗУЛТАТ: ");
            wsprintf(add, L"%d", score);
            wcscat_s(status, add);

            for (int i = 0; i < 200; ++i)
            {
                if (status[i] != '\0')txt_size++;
                else break;
            }
        
            Draw->DrawTextW(status, txt_size, nrmText, D2D1::RectF(10.0f, scr_height - 70.0f, scr_width, scr_height), TxtBrush);
        }

        // CATAPULT & EVILS ***************************************

        if (Catapult)
        {
            switch (Catapult->GetDir())
            {
            case dirs::left:
                if (now_shooting)
                {
                    int now_frame = Catapult->GetFrame(false);
                    Draw->DrawBitmap(bmpCatapultL[now_frame], D2D1::RectF(Catapult->x, Catapult->y, Catapult->ex, Catapult->ey));
                    if (now_frame >= 2)
                    {
                        now_shooting = false;
                        vAxes.push_back(dll::AxeFactory(Catapult->x, Catapult->ey, target_x, target_y));
                        Catapult->GetFrame(true);
                    }
                    break;
                }
                else
                    Draw->DrawBitmap(bmpCatapultL[0], D2D1::RectF(Catapult->x, Catapult->y, Catapult->ex, Catapult->ey));
                break;

            case dirs::right:
                if (now_shooting)
                {
                    int now_frame = Catapult->GetFrame(false);
                    Draw->DrawBitmap(bmpCatapultR[now_frame], D2D1::RectF(Catapult->x + 15.0f, Catapult->y, Catapult->ex, Catapult->ey));
                    if (now_frame >= 2)
                    {
                        now_shooting = false;
                        vAxes.push_back(dll::AxeFactory(Catapult->ex - 15.0f, Catapult->ey, target_x, target_y));
                        Catapult->GetFrame(true);
                    }
                    break;
                }
                else
                    Draw->DrawBitmap(bmpCatapultR[0], D2D1::RectF(Catapult->x, Catapult->y, Catapult->ex, Catapult->ey));
                break;

            case dirs::stop:
                if (now_shooting)
                {
                    int now_frame = Catapult->GetFrame(false);
                    Draw->DrawBitmap(bmpCatapultR[now_frame], D2D1::RectF(Catapult->x + 15.0f, Catapult->y, Catapult->ex, Catapult->ey));
                    if (now_frame >= 2)
                    {
                        now_shooting = false;
                        vAxes.push_back(dll::AxeFactory(Catapult->ex - 15.0f, Catapult->ey, target_x, target_y)); 
                        Catapult->GetFrame(true);
                    }
                    break;
                }
                else
                    Draw->DrawBitmap(bmpCatapultR[0], D2D1::RectF(Catapult->x, Catapult->y, Catapult->ex, Catapult->ey));
                break;
            }
        }
        
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

        if (!vAxes.empty())
        {
            for (std::vector<dll::Object>::iterator axe = vAxes.begin(); axe < vAxes.end(); axe++)
                Draw->DrawBitmap(bmpAxe, D2D1::RectF((*axe)->x, (*axe)->y, (*axe)->ex, (*axe)->ey));
        }

        if (!vExplosions.empty())
        {
            for (int i = 0; i < vExplosions.size(); ++i)
            {
                Draw->DrawBitmap(bmpExplosion[vExplosions[i].frame], D2D1::RectF(vExplosions[i].x, vExplosions[i].y,
                    vExplosions[i].ex, vExplosions[i].ey));
                vExplosions[i].frame++;
                if (vExplosions[i].frame > 23)
                {
                    if (vExplosions[i].victim == types::catapult)
                    {
                        Draw->EndDraw();
                        Sleep(500);
                        GameOver();
                        break;
                    }
                    vExplosions.erase(vExplosions.begin() + i);
                    break;
                }
            }
        }

        if (ToolSet)
            Draw->DrawBitmap(bmpTools, D2D1::RectF(ToolSet->x, ToolSet->y, ToolSet->ex, ToolSet->ey));
        /////////////////////////////////////////////////////////////
        Draw->EndDraw();

    }

    std::remove(tmp_file);
    ReleaseResources();
    return (int) bMsg.wParam;
}