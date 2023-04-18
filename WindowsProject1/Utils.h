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
