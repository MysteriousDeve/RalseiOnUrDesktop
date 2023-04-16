#pragma once

#include <Windows.h>
#include <atlstr.h>
#include <Gdiplus.h>
#include <algorithm>
#include <math.h>
#include <string>
#include <vector>
#include <fileapi.h>
#include "TextPrinter.h"

using namespace std;

enum ClipLoopMode
{
	Hold,
	Loop,
	None
};

struct Vector2
{
	double x, y;
	Vector2(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
};

enum RalseiState
{
	Front,
	Left,
	Back,
	Right,
	Shock,
	Fell
};


class AnimationClip
{
private:
	wstring clipName;
	vector<Gdiplus::Image*> images;
	double stepTime;
	int count;
	ClipLoopMode loopMode;

	double currentTime;
	int index = 0;
	bool isPlaying = true;
public:
	AnimationClip(wstring clipName, double stepTime, int count, ClipLoopMode loopMode = Loop)
	{
		this->clipName = clipName;
		this->stepTime = stepTime;
		this->count = count;
		this->loopMode = loopMode;
		Load();
		Reset();
	}

	void Load()
	{
		images.clear();
		for (int i = 0; i < count; i++)
		{
			wstring newstr = clipName + L"(" + to_wstring(i) + L").png";
			const wchar_t* cname = newstr.c_str();
			images.push_back(Gdiplus::Image::FromFile(cname));
		}
	}

	void Animate(double dt)
	{
		currentTime += dt;
		if (currentTime >= stepTime)
		{
			currentTime -= stepTime;
			index++;
			if (isPlaying && index >= images.size())
			{
				switch (loopMode)
				{
				case Loop: index = 0; break;
				case None: isPlaying = false; index = 0; break;
				case Hold: isPlaying = false; index--; break;
				default: break;
				}
			}
		}
	}

	Gdiplus::Image* GetCurrent()
	{
		return images[index];
	}

	void Reset()
	{
		isPlaying = true;
		index = 0;
		currentTime = 0;
	}
};




class Ralsei
{
private:
	AnimationClip* frontClip;
	AnimationClip* backClip;
	AnimationClip* leftClip;
	AnimationClip* rightClip;
	AnimationClip* shockClip;
	AnimationClip* fellClip;

	RalseiState state = RalseiState::Front;
	double internalTime = 0;
	Gdiplus::Image* gg = NULL;

	double gravity = 800;
	double pivotx = 0.5, pivoty = 1;
	double ppx = 0.5, ppy = 1;
	double damping = 6;

	double lowerBound = 1000;
	double fallShockVelThreshold = 750;
	double shockTime = 3;
	double leftLim = -300, rightLim = 2320;

	double val_veldiffy = 0;
	TextPrinter textPrinter = { L"Hello :D" };

	bool isSpeaking;
public:	
	double velx = 0, vely = 0, vxo = 0, vyo = 0;
	double x = 300, y = 100;
	double scale = 5;
	bool isHolding = false;

	Ralsei()
	{
		frontClip = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseid", 0.2, 1);
		leftClip = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseil", 0.2, 4);
		backClip = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseiu", 0.2, 1);
		rightClip = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseir", 0.2, 4);
		shockClip = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralsei_shock_overworld", 0.2, 1);
		fellClip = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralsei_fell", 0.2, 1);

		vxo = velx; vyo = vely;
	}

	void UpdatePhysics(double dt)
	{
		// Store velocity
		vxo = velx; vyo = vely;

		// Damping
		velx -= velx * damping * dt;
		if (vely < 0) vely -= vely * damping * dt;

		// Update velocity x
		x += velx * dt;
		if (x < leftLim)
		{
			x += rightLim - leftLim;
		}
		if (x > rightLim)
		{
			x -= rightLim - leftLim;
		}

		// Update velocity y
		vely += gravity * dt;
		y += vely * dt;
		if (y > lowerBound)
		{
			y = lowerBound;
			vely = 0;
		}
		val_veldiffy =  vely - vyo;
	}
	void IdleMode(double dt)
	{
		state = (RalseiState)((int)(internalTime / 2.0) % 4);
		switch (state)
		{
		case Left: leftClip->Animate(dt); x -= 100 * dt; break;
		case Right: rightClip->Animate(dt); x += 100 * dt; break;
		default: break;
		}
	}
	void Update(double dt)
	{
		internalTime += dt;
		UpdatePhysics(dt);
		if (isHolding)
		{
			isSpeaking = false;
			if (internalTime >= 0) state = RalseiState::Front;
		}
		else
		{
			if (!isSpeaking)
			{
				isSpeaking = true;
				textPrinter.Reset();
			}
			if (vely >= fallShockVelThreshold)
			{
				if (internalTime >= 0)
				{
					state = RalseiState::Shock;
					internalTime = 0;
				}
			}
			else
			{
				state = RalseiState::Front;
				if (isTouchingGround())
				{
					if (internalTime < 0) state = RalseiState::Fell;
					else if (val_veldiffy < -1500)
					{
						internalTime = -shockTime;
						state = RalseiState::Fell;
						PlaySound(L"sound\\snd_splat.wav", NULL, SND_FILENAME | SND_NOSTOP | SND_ASYNC);
					}
					if (internalTime >= 8) IdleMode(dt);
				}
			}
			if (internalTime < 0 && !isTouchingGround())
			{
				internalTime = -shockTime;
				state = RalseiState::Fell;
			}
		}
		textPrinter.Update(dt);
	}

	bool isTouchingGround()
	{
		return y - lowerBound > -0.5;
	}

	void SetVelocity(double vx, double vy)
	{
		velx = vx; vely = vy;
	}

	Gdiplus::Image* GetSprite()
	{
		switch (state)
		{
		case Front: gg = frontClip->GetCurrent(); break;
		case Left: gg = leftClip->GetCurrent(); break;
		case Back: gg = backClip->GetCurrent(); break;
		case Right: gg = rightClip->GetCurrent(); break;
		case Shock: gg = shockClip->GetCurrent(); break;
		case Fell: gg = fellClip->GetCurrent(); break;
		default: gg = NULL; break;
		}
		ppx = gg->GetWidth();
		ppy = gg->GetHeight();
		return gg;
	}

	Vector2 GetRenderPosition()
	{
		return Vector2(x - pivotx * ppx * scale, y - pivoty * ppy * scale);
	}

	Vector2 GetRenderDimension()
	{
		return Vector2(ppx * scale, ppy * scale);
	}

	bool IsSpeaking()
	{
		return isSpeaking;
	}

	CString GetCurrentSpeech()
	{
		return textPrinter.GetCurrent();
	}
};