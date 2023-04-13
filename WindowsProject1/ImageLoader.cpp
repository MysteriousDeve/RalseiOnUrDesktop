#include <Windows.h>
#include <Gdiplus.h>
#include <algorithm>
#include <math.h>
#include <string>
#include <vector>
#include <fileapi.h>

using namespace std;

enum ClipLoopMode
{
	Hold,
	Loop,
	None
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
	}

	void Load()
	{
		images.clear();
		WIN32_FIND_DATA data;
		
		for (int i = 0; i < count; i++)
		{
			wstring newstr = clipName + L"(" + to_wstring(i) + L").png";
			const wchar_t* cname = newstr.c_str();
			images.push_back(Gdiplus::Image::FromFile(cname));
		}
	}

	Gdiplus::Image* Animate(double dt)
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
		return images[index];
	}
};

class Relation
{
public:
	AnimationClip referTo;
};


class Ralsei
{
public:
	AnimationClip clip;
};

























