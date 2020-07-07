#pragma once
#include <iostream>

class GameObject;

class Component
{
public:
	std::string mName;
	GameObject *mGameObject;
	bool mIsEnabled;
public:
	Component();
	Component(std::string &pName);
	virtual ~Component() {}

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Destroy() {}

	virtual void FixedUpdate() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
};