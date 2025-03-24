#include "Ralsei.h"
#include "StateMachine.h"

#define RS_MUSIC "music"
#define RS_TALKING "talking"
#define RS_IDLE "idle"
#define RS_FELL "fell"
#define RS_DEFAULT "default"
#define RS_HATREMOVE "hatremove"


void Ralsei::InitState()
{
	ralseiState.AddState(RS_DEFAULT, 
		LambdaStateNode(
			[this]() {
				lockX = false;
			}, 
			[](double dt) {}
		)
	);

	ralseiState.AddState(RS_HATREMOVE,
		LambdaStateNode(
			[this]() {
				lockX = false;
				state = RalseiState::Custom;
			},
			[](double dt) {
				
			}
		)
	);

	ralseiState.AddState(RS_IDLE,
		LambdaStateNode(
			[this]() {
				lockX = false;
			},
			[](double dt) 
			{

			}
		)
	);

	ralseiState.AddState(RS_FELL,
		LambdaStateNode(
			[this]() {
				lockX = false;
			},
			[](double dt)
			{

			}
		)
	);

	ralseiState.AddState(RS_MUSIC,
		LambdaStateNode(
			[this]() {
				lockX = true;
				state = RalseiState::Custom;
			},
			[](double dt) {}
		)
	);

	ralseiState.AddLink(RS_DEFAULT, RS_MUSIC, []() { return false; });

	ralseiState.Begin(RS_DEFAULT);
}

