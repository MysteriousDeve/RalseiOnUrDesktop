#include <Windows.h>
#include <Gdiplus.h>
#include <algorithm>
#include <math.h>
#include <string>
#include <vector>
#include <fileapi.h>

using namespace std;

class AnimationClip
{
private:
	wstring clipName;
	vector<Gdiplus::Image*> images;
	double stepTime;
	int count;
public:
	AnimationClip(wstring clipName, double stepTime, int count)
	{
		this->clipName = clipName;
		this->stepTime = stepTime;
		this->count = count;
		Load();
	}

	void Load()
	{
		images.clear();
		WIN32_FIND_DATA data;
		
		for (int i = 0; i < count; i++)
		{
			wstring newstr = clipName + L"(" + to_wstring(i) + L")";
			const wchar_t* cname = newstr.c_str();
			images.push_back(Gdiplus::Image::FromFile(cname));
		}
	}
};




























