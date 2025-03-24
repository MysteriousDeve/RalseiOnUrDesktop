#pragma once

#include "framework.h"
#include "Subwindow.h"
#include "TextPrinter.h"
#include "AnimationClip.h"
#include "Audio2D.h"
#include "StateMachine.h"
#include "LoopbackCapture.h"

using namespace std;

enum RalseiState
{
	Front,
	Left,
	Back,
	Right,
	Shock,
	Fell,
	Music,
	Custom
};

enum RalseiMode
{
	ModeNone,
	ModeIdle,
	ModeRemoveHat
};

class Ralsei : public Subwindow
{
private:
	AnimationClip* frontClip[2];
	AnimationClip* backClip[2];
	AnimationClip* leftClip[2];
	AnimationClip* rightClip[2];
	AnimationClip* shockClip[2];
	AnimationClip* fellClip[2];
	AnimationClip* frontBlushClip[2];
	shared_ptr<AnimationClip> danceClip[2];
	AnimationClip* removeHat;
	bool hatRemoved = false;

	double internalTime = 0;
	AnimationFrame gg = AnimationFrame();

	// Physics parameter
	double gravity = 1600;
	double damping = 1;
	double lowerBound = 1000;
	double fallShockVelThreshold = 1600;
	double hitVelThreshold = 1600; // *splat*
	double groundFriction = 8;
	double groundRestitution = 0.2;
	bool lockX = false;

	// Char state parameter
	RalseiState state = RalseiState::Front;
	double shockTime = 3;
	double idleTime = 4;

	// Sprite parameter
	double leftLim = -100, rightLim = 2120;
	double pivotx = 0.5, pivoty = 1;
	double ppx = 0.5, ppy = 1;
	Vector2 offset = {};
	double scale = 5;

	// Char variable
	double val_veldiffy = 0;
	double velx = 0, vely = 0, vxo = 0, vyo = 0;
	bool isSpeaking;

	// Subwindows storage
	shared_ptr<Audio2D> engine;
	shared_ptr<TextPrinter> textPrinter;
	shared_ptr<LoopbackCapture> loopbackCapture;

	// State machine
	StateMachine ralseiState;
	void InitState();
public:	
	// Char variable public
	bool isHolding = false;
	bool isFalling = false;
	bool hasSaidHi = false;

	Ralsei() : Subwindow()
	{
		name = "Le Goat";

		frontClip[0] = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseid({}).png", 0.2, 1);
		leftClip[0] = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseil({}).png", 0.2, 4);
		backClip[0] = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseiu({}).png", 0.2, 1);
		rightClip[0] = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseir({}).png", 0.2, 4);
		shockClip[0] = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralsei_shock_overworld({}).png", 0.2, 1);
		fellClip[0] = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralsei_fell({}).png", 0.2, 1);
		frontBlushClip[0] = new AnimationClip(L"deltarune-sprites\\ralsei\\spr_ralseid_blush({}).png", 0.2, 1);

		// Ralsei ch2
		frontClip[1] = new AnimationClip(L"deltarune-sprites\\ralsei-ch2\\spr_ralsei_walk_down_{}.png", 0.2, 1);
		leftClip[1] = new AnimationClip(L"deltarune-sprites\\ralsei-ch2\\spr_ralsei_walk_left_{}.png", 0.2, 4);
		backClip[1] = new AnimationClip(L"deltarune-sprites\\ralsei-ch2\\spr_ralsei_walk_up_{}.png", 0.2, 1);
		rightClip[1] = new AnimationClip(L"deltarune-sprites\\ralsei-ch2\\spr_ralsei_walk_right_{}.png", 0.2, 4);
		shockClip[1] = new AnimationClip(L"deltarune-sprites\\ralsei-ch2\\spr_ralsei_surprised_down_{}.png", 0.2, 1);
		fellClip[1] = new AnimationClip(
			L"deltarune-sprites\\ralsei-ch2\\spr_cutscene_10_ralsei_splat_{}.png", 
			0.2, 1, Loop, Vector2{ 0, 30 }
		);

		pos = { 300, 600 };
		vxo = velx; vyo = vely;
		isSpeaking = false;

		// Dance clip def
		// ch1
		auto d1 = make_shared<AnimationClip>(0.2, Loop, Vector2{ 0, 0 });
		for (int i = 0; i < 8; i++)
		{
			d1->AddFrameMulti(L"deltarune-sprites\\ralsei\\spr_ralseib_sing({}).png", 3);
		}
		for (int i = 0; i < 3; i++)
		{
			d1->AddFrameMulti(L"deltarune-sprites\\ralsei\\spr_ralseib_battleintro({}).png", 9);
		}
		danceClip[0] = d1;

		// ch2
		auto d2_pose1 = Image::FromFile(L"deltarune-sprites\\ralsei-ch2\\spr_ralsei_pose_0.png");
		auto d2_pose1_flip = Image::FromFile(L"deltarune-sprites\\ralsei-ch2\\spr_ralsei_pose_0.png");
		d2_pose1_flip->RotateFlip(RotateNoneFlipX);

		auto d2 = make_shared<AnimationClip>(0.2, Loop, Vector2{ 0, 0 });
		for (int i = 0; i < 4; i++)
		{
			d2->AddFrame(d2_pose1);
			d2->AddFrame(d2_pose1);
			d2->AddFrame(d2_pose1_flip);
			d2->AddFrame(d2_pose1_flip);
		}
		d2->AddFrameMulti(L"deltarune-sprites\\ralsei-ch2\\Battle\\spr_ralsei_dance_{}.png", 8);
		d2->AddFrameMulti(L"deltarune-sprites\\ralsei-ch2\\Battle\\spr_ralsei_dance_{}.png", 8);
		danceClip[1] = d2;
		InitState();
	}

	void Ready() override
	{
		engine = std::make_shared<Audio2D>();
		AddChild(engine);

		engine->Preload("sound\\snd_ralsei_yell.wav");
		engine->Preload("sound\\snd_splat.wav");

		textPrinter = std::make_shared<TextPrinter>(
			L"Hello! I'm Ralsei.|||Shall we be friend along\nthe way?",
			false, 0.08, "sound\\snd_txtral_ch1.wav", 1
		);
		textPrinter->SetAudioEngine2D(engine);
		AddChild(textPrinter);

		loopbackCapture = make_shared<LoopbackCapture>();
	}

	void SetMode(RalseiMode mode)
	{
		switch (mode)
		{
		case ModeIdle: internalTime = idleTime; break;
		default: break;
		}
	}

	void SetConvo(CString str)
	{
		textPrinter->SetNew(str);
		textPrinter->isPlaying = true;
		isSpeaking = true;
		velx = 0;
	}

	void SetRemoveHat(bool hatRemoved)
	{
		this->hatRemoved = hatRemoved;
		SetMode(RalseiMode::ModeRemoveHat);
	}

	void UpdatePhysics(double dt)
	{
		// Store velocity
		vxo = velx; vyo = vely;

		if (lockX) velx = 0;

		// Damping
		velx -= velx * damping * dt;
		if (vely < 0) vely -= vely * damping * dt;

		// Update velocity x
		pos.x += velx * dt;
		if (pos.x < leftLim)
		{
			pos.x += rightLim - leftLim;
		}
		if (pos.x > rightLim)
		{
			pos.x -= rightLim - leftLim;
		}

		// Update velocity y
		double prev_vel = 0;

		vely += gravity * dt * (state == RalseiState::Fell ? 2 : 1);
		pos.y += vely * dt;
		if (pos.y >= lowerBound)
		{
			pos.y = lowerBound;
			prev_vel = vely;

			if (state == RalseiState::Fell || state == RalseiState::Shock)
			{
				vely = -vely * groundRestitution; // Bounce him when hitting the ground
				velx -= velx * dt * groundFriction; // Friction from hitting the ground
			}
			else
			{
				vely = 0;
				velx = 0;
			}
		}
		val_veldiffy = vely - vyo;

		if (val_veldiffy < -hitVelThreshold)
		{
			vely = -prev_vel * groundRestitution;
		}
	}

	double linger = 0;
	bool heard = false;
	void IdleMode(double dt)
	{
		float sysvol = loopbackCapture->GetAudioVolumeAvg();
		if (sysvol > 0.001)
		{
			if (linger < 2) linger += dt * 2;
		}
		else if (linger >= -2) linger -= dt;

		if (sysvol > 0.1 || linger > (heard ? 0 : 1))
		{
			if (!heard)
			{
				internalTime = 0;
				heard = true;
				linger = 2;
			}
			state = RalseiState::Music;
			danceClip[hatRemoved]->Animate(dt);
		}
		else 
		{
			if (heard)
			{
				internalTime = 0;
				heard = false;
			}
			state = (RalseiState)((int)(internalTime / 2.0) % 4);
			switch (state)
			{
			case Left: leftClip[hatRemoved]->Animate(dt); pos.x -= 100 * dt; break;
			case Right: rightClip[hatRemoved]->Animate(dt); pos.x += 100 * dt; break;
			default: break;
			}
		}
	}

	void Update(double dt) override
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
			if (vely >= fallShockVelThreshold)
			{
				if (internalTime >= 0)
				{
					if (!isFalling)
					{
						isFalling = true;
						engine->PlaySound("sound\\snd_ralsei_yell.wav");
					}
					state = RalseiState::Shock;
					internalTime = 0;
				}
			}
			else
			{
				isFalling = false;
				state = RalseiState::Front;
				if (isTouchingGround())
				{
					if (internalTime < 0)
					{
						state = RalseiState::Fell;
					}

					if (val_veldiffy < -2500)
					{
						internalTime = -shockTime;
						state = RalseiState::Fell;
						engine->PlaySound("sound\\snd_splat.wav");
					}

					if (internalTime >= idleTime) IdleMode(dt);
				}
			}

			// keep him in fell mode
			if (internalTime < 0 && !isTouchingGround())
			{
				internalTime = -shockTime;
				state = RalseiState::Fell;
			}

			// speaking check
			if (isTouchingGround() && !isSpeaking && internalTime >= 0)
			{
				if (!hasSaidHi)
				{
					isSpeaking = true;
					textPrinter->Reset();
					textPrinter->isPlaying = true;
					hasSaidHi = true;
					velx = 0;
				}
			}
			if (IsSpeaking()) internalTime = 0;
		}
	}

	bool isTouchingGround()
	{
		return pos.y - lowerBound > -0.5;
	}

	void SetVelocity(double vx, double vy)
	{
		velx = vx; vely = vy;
	}

	Gdiplus::Image* GetSprite()
	{
		switch (state)
		{
		case Front: gg = frontClip[hatRemoved]->GetCurrent(); break;
		case Left: gg = leftClip[hatRemoved]->GetCurrent(); break;
		case Back: gg = backClip[hatRemoved]->GetCurrent(); break;
		case Right: gg = rightClip[hatRemoved]->GetCurrent(); break;
		case Shock: gg = shockClip[hatRemoved]->GetCurrent(); break;
		case Fell: gg = fellClip[hatRemoved]->GetCurrent(); break;
		case Music: gg = danceClip[hatRemoved]->GetCurrent(); break;
		default: gg = AnimationFrame(); break;
		}
		if (gg.frame)
		{
			ppx = gg.frame->GetWidth();
			ppy = gg.frame->GetHeight();
			offset = gg.offset;
		}
		return gg.frame;
	}

	Vector2 GetRenderPosition()
	{
		return Vector2(pos.x - pivotx * ppx * scale + offset.x, pos.y - pivoty * ppy * scale + offset.y);
	}

	Vector2 GetRenderDimension()
	{
		return Vector2(ppx * scale, ppy * scale);
	}

	bool IsSpeaking()
	{
		return isSpeaking && !textPrinter->IsTimeout();
	}

	CString GetCurrentSpeech()
	{
		return textPrinter->GetCurrent();
	}
};
