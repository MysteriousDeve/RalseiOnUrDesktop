#pragma once

#include <atlstr.h>

class TextPrinter
{
private:
	const double rate = 0.05;
	double internalTime = 0;
	CString str;
	CString strCopy;
	CString current;
public:
	bool isHolding = false;

	TextPrinter(CString str)
	{
		this->str = str;
		Reset();
	}

	void Update(double dt)
	{
		internalTime += dt;
		if (internalTime >= rate)
		{
			while (true)
			{
				if (strCopy.GetLength() == 0) break;
				CString s = strCopy[0];
				current += s;
				strCopy.Delete(0, 1);

				if (s != L" " || s != L"\n")
				{
					break;
				}
			}
			internalTime -= rate;
		}
	}

	bool isDone()
	{
		return strCopy.GetLength() == 0;
	}

	void SetNew(CString str)
	{
		this->str = str;
		Reset();
	}

	void Reset()
	{
		strCopy = str;
		current.Empty();
	}

	CString GetCurrent()
	{
		return current;
	}
};