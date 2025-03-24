#pragma once
#include <gdiplus.h>
#include <gdiplusheaders.h>

struct Vector2Int
{
	int x, y;
	Vector2Int(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2Int(POINT pt)
	{
		this->x = pt.x;
		this->y = pt.y;
	}

	Vector2Int()
	{

	}

	operator POINT()
	{
		POINT p;
		p.x = x;
		p.y = y;
		return p;
	}

	Vector2Int operator+(Vector2Int other)
	{
		return Vector2Int(x + other.x, y + other.y);
	}

	Vector2Int operator-(Vector2Int other)
	{
		return Vector2Int(x - other.x, y - other.y);
	}

	Vector2Int operator*(int other)
	{
		return Vector2Int(x * other, y * other);
	}

	Vector2Int operator/(int other)
	{
		return Vector2Int(x / other, y / other);
	}
};



struct Vector2
{
	double x, y;
	Vector2(double x, double y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2(POINT pt)
	{
		this->x = pt.x;
		this->y = pt.y;
	}

	Vector2(Vector2Int vec)
	{
		this->x = vec.x;
		this->y = vec.y;
	}

	Vector2()
	{

	}

	operator POINT()
	{
		POINT p;
		p.x = (int)x;
		p.y = (int)y;
		return p;
	}

	operator POINTF()
	{
		POINTF p;
		p.x = x;
		p.y = y;
		return p;
	}

	operator Gdiplus::PointF()
	{
		return Gdiplus::PointF{ (Gdiplus::REAL)x, (Gdiplus::REAL)y };
	}

	Vector2 operator+(Vector2 other)
	{
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 operator-(Vector2 other)
	{
		return Vector2(x - other.x, y - other.y);
	}

	Vector2 operator*(double other)
	{
		return Vector2(x * other, y * other);
	}

	Vector2 operator/(double other)
	{
		return Vector2(x / other, y / other);
	}

	Vector2 operator+=(Vector2 other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vector2 operator-=(Vector2 other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vector2 operator*=(double other)
	{
		x *= other;
		y *= other;
		return *this;
	}

	Vector2 operator/=(double other)
	{
		x /= other;
		y /= other;
		return *this;
	}

	Vector2 operator*=(Vector2 other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vector2 operator/=(Vector2 other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	double dot(Vector2 other)
	{
		return x * other.x + y * other.y;
	}
};
