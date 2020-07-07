#include <System/EngineSystem.h>
#include <System/System.h>
#include <type_traits>

namespace Engine{
System::System()
:	m_impl(new SystemEngine())
{
	
};


System::~System()
{

}
void System::Init(SCreationSettings&& cs)
{
	m_impl->Init(std::forward<SCreationSettings>(cs));
}
void System::Start()
{
	m_impl->Start();
}
}