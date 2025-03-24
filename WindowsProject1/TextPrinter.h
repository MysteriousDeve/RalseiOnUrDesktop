#pragma once

#include "framework.h"
#include "Subwindow.h"
#include <irrKlang/irrKlang.h>
#include "Audio2D.h"
using namespace irrklang;
using namespace std;

class TextPrinter : public Subwindow
{
private:
	double rate = 0.05;
	double internalTime = 0;
	int strListIndex = 0;
	double autoSkipTime = 2;
	vector<CString> strList;
	CString str;
	CString strCopy;
	CString current;
	shared_ptr<Audio2D> engine;
	string playSound;
	bool isTimeout = false;
	bool delayOnSpecialChars;
public:
	bool isPlaying = false;

	TextPrinter(CString str, bool isPlaying = true, double rate = 0.05, std::string playSound = "", double autoSkipTime = INFINITY, bool delayOnSpecialChars = true)
	{
		this->str = str;
		this->isPlaying = isPlaying;
		this->autoSkipTime = autoSkipTime;
		this->playSound = playSound;
		this->rate = rate;
		this->delayOnSpecialChars = delayOnSpecialChars;
		Reset();
	}

	void Ready() override
	{
		if (!engine)
		{
			engine = make_shared<Audio2D>();
			AddChild(engine);
		}
		engine->Preload(playSound);
	}

	void SetAudioEngine2D(shared_ptr<Audio2D> engine)
	{
		this->engine = engine;
	}

	void Update(double dt) override
	{
		if (!isPlaying) return;

		internalTime += dt;
		if (strCopy.GetLength() > 0)
		{
			if (internalTime >= rate)
			{
				while (true)
				{
					CString s = strCopy[0];
					current += s;
					strCopy.Delete(0, 1);

					if (s != CString(" ") && s != CString("\n"))
					{
						if (delayOnSpecialChars && CString("!?.,;").Find(s) != -1)
						{
							internalTime -= rate * 5;
						}
						if (playSound != "")
						{
							engine->PlaySound(playSound);
						}
						break;
					}
				}
				internalTime -= rate;
			}
		}
		if (internalTime >= autoSkipTime)
		{
			isTimeout = strListIndex >= strList.size() - 1;
			if (!isTimeout)
			{
				strListIndex++;
				InitNextDialogue();
			}
		}
		else isTimeout = false;
	}

	void SetNew(CString str)
	{
		this->str = str;
		Reset();
	}

	void InitNextDialogue()
	{
		strCopy = strList[strListIndex];
		current.Empty();
		internalTime = 0;
	}

	void Reset()
	{
		strList.clear();
		int nTokenPos = 0;
		CString strToken = str.Tokenize(_T("|||"), nTokenPos);
		while (!strToken.IsEmpty())
		{
			strList.push_back(strToken);
			strToken = str.Tokenize(_T("|||"), nTokenPos);
		}
		strListIndex = 0;
		InitNextDialogue();
	}

	CString GetCurrent()
	{
		// CString(L"[") + CString(to_string(strListIndex).c_str()) + CString(",") + CString(to_string(strList.size()).c_str()) + CString("]") + 
		return current;
	}

	bool IsTimeout()
	{
		return isTimeout;
	}
};