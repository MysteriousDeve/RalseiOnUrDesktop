#pragma once

#include <irrKlang/irrKlang.h>
#include "framework.h"
#include "Subwindow.h"
using namespace irrklang;
using namespace std;

class Audio2D : public Subwindow
{
private:
	double internalTime = 0;

	ISoundEngine* engine;
	vector<ISound*> currentSounds;
	const double panningScale = 0.15;

public:

	Audio2D() : Subwindow()
	{
		engine = createIrrKlangDevice();
	}

	void PlaySound(string soundPath)
	{
		ISound* sound = engine->play2D(soundPath.c_str(), false, true, true);

		Vector2 globalPos = GetGlobalPosition();
		double hrange = Platform::topParent->GetDimension().x / 2.0;
		sound->setPan(-(globalPos.x - hrange) / hrange * panningScale);
		sound->setIsPaused(false);

		currentSounds.push_back(sound);

		// sound = engine->play2D(soundPath.c_str());
		//Vector2 globalPos = GetOwner()->pos;
		//string str = 
		//	to_string(globalPos.x) + ", " + 
		//	to_string(globalPos.y);
		// MessageBoxA(NULL, str.c_str(), "Hello World!", MB_OK | MB_ICONINFORMATION);
	}

	void Preload(string soundPath)
	{
		engine->addSoundSourceFromFile(soundPath.c_str(), ESM_AUTO_DETECT, true);
	}

	void Update(double dt) override
	{
		internalTime += dt;
		Vector2 globalPos = GetGlobalPosition();

		// Manage sfx panning
		for (int i = 0; i < currentSounds.size(); i++)
		{
			if (currentSounds[i] && !currentSounds[i]->isFinished())
			{
				double hrange = Platform::topParent->GetDimension().x / 2.0;
				currentSounds[i]->setPan(-(globalPos.x - hrange) / hrange * panningScale);
				// currentSounds[i]->setIsPaused(false);
			}
			else
			{
				if (currentSounds[i]) currentSounds[i]->drop();
				currentSounds.erase(currentSounds.begin() + i);
				i--;
			}
		}
	}
};
