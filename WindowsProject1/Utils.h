#pragma once
#include <Windows.h>
#include <WinUser.h>
#include <shlwapi.h>
#include <algorithm>
#include <chrono>
#include <math.h>

#include <ObjIdl.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#include <gdipluspen.h>
#include <gdiplusbrush.h>
#include <atlstr.h>
#include <memory>

#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0




using namespace Gdiplus;
using namespace std;

int Width = 500;
int Height = 500;

HFONT hFont;

inline void DrawFineRect(Graphics* g, Brush* brush, Rect rect)
{
    PointF points[5];
    points[0] = PointF(rect.X, rect.Y);
    points[1] = points[0]; // fix rect problem
    points[2] = points[0] + PointF(rect.Width, 0);
    points[3] = points[0] + PointF(rect.Width, rect.Height);
    points[4] = points[0] + PointF(0, rect.Height);
    g->FillPolygon(brush, points, 5);
}

inline void DrawFineRect(Graphics* g, Brush* brush, RectF rect)
{
    PointF points[5];
    points[0] = PointF(rect.X, rect.Y);
    points[1] = points[0]; // fix rect problem
    points[2] = points[0] + PointF(rect.Width, 0);
    points[3] = points[0] + PointF(rect.Width, rect.Height);
    points[4] = points[0] + PointF(0, rect.Height);
    g->FillPolygon(brush, points, 5);
}

CString convo[]
{
    "About me?|||I'm Ralsei, the prince\nof the Dark!|||(Erm, is just that all\nabout me?)",
    "About the dark world...|||It is a world which\nis powered by a Dark\nFountain.|||And the Dark Fountain is a\ngeyser that give the Dark\nWorld its own form.|||...\n(Did I miss anything?)",
    "Once upon a time, a LEGEND\nwas whispered among\nshadows.||||It was a LEGEND of HOPE.\nIt was a LEGEND of DREAMS.|||It was a LEGEND of LIGHT.\nIt was a LEGEND of DARK.|||This is the legend of...|||..............\n(Oh no I forgot it)",
    "The SOUL?|||Well, it is the highest\nculmination of your being.|||In battle, the SOUL can\nbe hit by enemy attacks,\nso watch out for them.|||The SOUL also has the\npower to seal the Fountain.",
    "Then do it!\nSchool is important!|||I'm not going to talk\nto you unless you\nfinished everything!|||(Wait, did you ask for\nmy help or something?)",
};
int convoIndex = -1;

bool isDeviceCharging()
{
    SYSTEM_POWER_STATUS status;
    GetSystemPowerStatus(&status);
    return status.BatteryFlag & (BATTERY_FLAG_CHARGING | BATTERY_FLAG_NO_BATTERY);
}

namespace Utils
{
    class Font
    {
    private:
        HFONT font;
        int height;
        string name;
    public:
        Font() {}

        Font(int height, string name)
        {
            font = CreateFontA(
                height,
                0,
                0,
                0,
                FW_NORMAL,
                FALSE,
                FALSE,
                FALSE,
                DEFAULT_CHARSET,
                OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY,
                DEFAULT_PITCH,
                name.c_str()
            );
            this->height = height;
            this->name = name;
        }

        HFONT GetFont()
        {
            return font;
        }

        int GetHeight()
        {
            return height;
        }

        string GetName()
        {
            return name;
        }
    };
}
