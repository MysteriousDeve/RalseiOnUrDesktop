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
