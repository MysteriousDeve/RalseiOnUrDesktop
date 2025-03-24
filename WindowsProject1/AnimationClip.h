#pragma once
#include "framework.h"

enum ClipLoopMode
{
	Hold,
	Loop,
	None
};

struct AnimationFrame
{
	Gdiplus::Image* frame;
	Vector2 offset;
	AnimationFrame(Gdiplus::Image* frame, Vector2 offset)
	{
		this->frame = frame;
		this->offset = offset;
	}
	AnimationFrame() 
	{
		this->frame = nullptr; 
		this->offset = Vector2{ 0, 0 };
	}
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
	Vector2 offset;
public:
	AnimationClip(wstring clipName, double stepTime, int count, ClipLoopMode loopMode = Loop, Vector2 offset = { 0, 0 })
	{
		this->clipName = clipName;
		this->stepTime = stepTime;
		this->count = count;
		this->loopMode = loopMode;
		this->offset = offset;
		Load();
		Reset();
	}

	AnimationClip(double stepTime, ClipLoopMode loopMode = Loop, Vector2 offset = { 0, 0 })
	{
		this->stepTime = stepTime;
		this->count = 0;
		this->loopMode = loopMode;
		this->offset = offset;
		Reset();
	}

	void AddFrame(wstring name)
	{
		const wchar_t* cname = name.c_str();
		auto img = Gdiplus::Image::FromFile(cname);
		AddFrame(img);
	}

	void AddFrame(Gdiplus::Image* img)
	{
		images.push_back(img);
	}

	void AddFrameMulti(wstring name, int count)
	{
		for (int i = 0; i < count; i++)
		{
			wstring newstr = vformat(wstring_view(name), std::make_wformat_args(i));
			const wchar_t* cname = newstr.c_str();
			images.push_back(Gdiplus::Image::FromFile(cname));
		}
	}

	void Load()
	{
		images.clear();
		for (int i = 0; i < count; i++)
		{
			wstring newstr = vformat(wstring_view(clipName), std::make_wformat_args(i));
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

	AnimationFrame GetCurrent()
	{
		return AnimationFrame(images[index], offset);
	}

	void Reset()
	{
		isPlaying = true;
		index = 0;
		currentTime = 0;
	}
};
