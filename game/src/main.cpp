
//#undef main
#include <System/System.h>

int main(int argc, char **argv)
{
	auto system = new Engine::System();
	system->Init({ 800, 450, "Test" });
	system->Start();
	return 0;
}