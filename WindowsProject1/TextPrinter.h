#pragma once

#include <atlstr.h>
#include <irrKlang/irrKlang.h>
using namespace irrklang;

class TextPrinter
{
private:
	const double rate = 0.05;
	double internalTime = 0;
	CString str;
	CString strCopy;
	CString current;
	ISoundEngine* engine;
public:
	bool isPlaying = false;

	TextPrinter(CString str, bool isPlaying = true)
	{
		this->str = str;
		this->isPlaying = isPlaying;
		Reset();
		engine = createIrrKlangDevice();
	}

	void Update(double dt)
	{
		if (!isPlaying) return;

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
					engine->play2D("sound\\snd_txtral_ch1.wav");
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