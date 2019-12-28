#include <stdlib.h>

#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "MemLeaks.h"

// This is needed here because SDL redefines main function
// do not add any other libraries here, instead put them in their modules
#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )

//Includes brofiler
#include "brofiler/Brofiler.h"
#pragma comment (lib, "brofiler/ProfilerCore32.lib")

//Game general state
enum MainState
{
	CREATE = 1,
	AWAKE,
	START,
	LOOP,
	CLEAN,
	FAIL,
	EXIT
};

//Pointer to application
j1App* App = NULL;
bool exitGame = false;
bool existSaveFile = false;


//Main game function
int main(int argc, char* args[])
{
	LOG("Engine starting ... %d");

	//Used to report memory leaks
	ReportMemoryLeaks();

	MainState state = MainState::CREATE;
	int result = EXIT_FAILURE;

	//Game loop
	while(state != EXIT)
	{
		switch(state)
		{

			// Allocate the engine --------------------------------------------
			case CREATE:
			LOG("CREATION PHASE ===============================");

			App = new j1App(argc, args);

			if(App != NULL)
				state = AWAKE;
			else
				state = FAIL;

			break;

			// Awake all modules -----------------------------------------------
			case AWAKE:
			LOG("AWAKE PHASE ===============================");
			if(App->Awake() == true)
				state = START;
			else
			{
				LOG("ERROR: Awake failed");
				state = FAIL;
			}

			break;

			// Call all modules before first frame  ----------------------------
			case START:
			LOG("START PHASE ===============================");
			if(App->Start() == true)
			{
				state = LOOP;
				LOG("UPDATE PHASE ===============================");
			}
			else
			{
				state = FAIL;
				LOG("ERROR: Start failed");
			}
			break;

			// Loop all modules until we are asked to leave ---------------------
			case LOOP:
			{BROFILER_FRAME("FRAME");
			if (App->Update() == false || exitGame)
				state = CLEAN;
			break;
			}
			// Cleanup allocated memory -----------------------------------------
			case CLEAN:
			LOG("CLEANUP PHASE ===============================");
			if(App->CleanUp() == true)
			{
				RELEASE(App);
				result = EXIT_SUCCESS;
				state = EXIT;
			}
			else
				state = FAIL;

			break;

			// Exit with errors and shame ---------------------------------------
			case FAIL:
			LOG("Exiting with errors :(");
			result = EXIT_FAILURE;
			state = EXIT;
			break;
		}
	}

	LOG("... Bye! :)\n");

	// Dump memory leaks
	return result;
}