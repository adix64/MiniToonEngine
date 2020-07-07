#pragma once
#include <memory>
#include <Config.h>
#include "CreationSettigns.h"
class SystemEngine;
namespace Engine{
class ENGINE_API System
{
public:
	System();
	~System();

	void Init(SCreationSettings&& cs);
	void Start();
protected:
	std::unique_ptr<SystemEngine> m_impl;
};
}