#pragma once
#include "framework.h"
#include <map>

class StateLink
{
public:
	string nextState;
	std::function<bool()> condition;
	unsigned short priority;

	StateLink() {}

	StateLink(string nextState, std::function<bool()> condition, unsigned short priority)
	{
		this->nextState = nextState;
		this->condition = condition;
		this->priority = priority;
	}
};

class StateNode
{
private:
	friend class StateMachine;
	vector<shared_ptr<StateLink>> links;

	shared_ptr<StateLink> Transit()
	{
		int currentPriority = -1;
		shared_ptr<StateLink> currentLink;
		for (auto l = links.begin(); l != links.end(); l++)
		{
			if (currentPriority >= (*l)->priority)
			{
				continue;
			}
			if ((*l)->condition())
			{
				currentLink = *l;
				currentPriority = (*l)->priority;
			}
		}
		return currentLink;
	}

public:
	void AddLink(string nextState, std::function<bool()> condition, unsigned short priority = 0)
	{
		links.push_back(make_shared<StateLink>(nextState, condition, priority));
	}
	virtual void Ready() {};
	virtual void Update(double dt) {};
}; class LambdaStateNode; // forward decl



class StateMachine
{
private:
	std::map<string, StateNode> states;
	string currentState;

public:
	void AddState(string name, StateNode state)
	{
		states[name] = state;
	}

	void AddLink(string fromState, string nextState, std::function<bool()> condition, unsigned short priority = 0)
	{
		states[fromState].AddLink(nextState, condition, priority);
	}

	void MoveState(string fromState, string nextState)
	{
		if (fromState == currentState)
		{
			Begin(nextState);
		}
	}

	void MoveState(vector<string> fromStates, string nextState)
	{
		for (auto st = fromStates.begin(); st != fromStates.end(); st++)
		{
			if (*st == currentState)
			{
				Begin(nextState);
			}
		}
	}

	void Begin(string name)
	{
		currentState = name;
		states[currentState].Ready();
	}

	void Update(double dt)
	{
		if (states.find(currentState) != states.end())
		{
			states[currentState].Update(dt);
			shared_ptr<StateLink> nextState = states[currentState].Transit();
			if (nextState)
			{
				currentState = nextState->nextState;
				states[currentState].Ready();
			}
		}
		else throw exception("Something is wrong with you\n'cause this state DOES NOT EXIST!!!!!!!!");
	}
};


class LambdaStateNode : public StateNode
{
protected:
	std::function<void()> m_ready;
	std::function<void(double)> m_update;

public:
	void Ready() override
	{
		m_ready();
	}

	void Update(double dt) override
	{
		m_update(dt);
	}

	LambdaStateNode(std::function<void()> ready, std::function<void(double)> update = [](double dt){}) : StateNode()
	{
		m_ready = ready;
		m_update = update;
	}
};