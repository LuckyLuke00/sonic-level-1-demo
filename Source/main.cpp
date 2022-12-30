#include "pch.h"

#include <ctime>

#include "Core.h"

void StartHeapControl();
void DumpMemoryLeaks();

int main(int argc, char* argv[])
{
	srand(static_cast<unsigned int>(time(nullptr)));

	StartHeapControl();

	Core* pCore{ new Core{ Window{ "Sonic the Hedgehog - Kinoo, Lucas - 1DAE21", 424.f * 3, 240.f * 3, false } } };

	SDL_Surface* icon = IMG_Load("./Resources/Icon.png");
	SDL_SetWindowIcon(pCore->GetWindow(), icon);
	SDL_FreeSurface(icon);

	pCore->Run();
	delete pCore;

	icon = nullptr;

	DumpMemoryLeaks();
	return 0;
}

void StartHeapControl()
{
#if defined(DEBUG) | defined(_DEBUG)
	// Notify user if heap is corrupt
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Report detected leaks when the program exits
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Set a breakpoint on the specified object allocation order number
	//_CrtSetBreakAlloc( 156 );
#endif
}

void DumpMemoryLeaks()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
}
