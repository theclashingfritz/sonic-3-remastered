#include "main.h"
//#include "discord_register.h"
//#include "discord_rpc.h"

int HandleAppEvents(void *userdata, SDL_Event *event) {
    switch (event->type) {
	    case SDL_APP_TERMINATING:
	        return 0;
	    case SDL_APP_LOWMEMORY:
	        return 0;
	    case SDL_WINDOWEVENT:
	        return 0;
        #if defined(PLATFORM_IOS)
            case SDL_JOYAXISMOTION:
                //printf("SDL_JOYAXISMOTION: TRUE\n");
                //fflush(stdout);
                return 0;
        #endif

        case SDL_JOYDEVICEADDED:
            return 0;

	    default:
	        return 1;
    }
}

void InitDiscord() {
    //DiscordEventHandlers handlers;
    //memset(&handlers, 0, sizeof(handlers));
    //handlers.ready = handleDiscordReady;
    //handlers.errored = handleDiscordError;
    //handlers.disconnected = handleDiscordDisconnected;
    //handlers.joinGame = handleDiscordJoinGame;
    //handlers.spectateGame = handleDiscordSpectateGame;
    //handlers.joinRequest = handleDiscordJoinRequest;

    // Discord_Initialize(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId)
    //Discord_Initialize("418562325121990661", &handlers, 1, "1234");
}

const char* _J_ = "Sonic the Hedgehog is a 15 year old blue hedgehog who lives in no particular locale on the planet Earth, instead choosing to travel the world looking for fun and adventure wherever he goes. While he may be a drifter, that doesn't mean he only cares about himself. Sonic is known for his 'attitude', which is shown through his smugness and sarcasm. Sonic sometimes likes to show off in order to 'look cool'. He is often shown posing or adjusting his gloves or shoes. He is also often noted to be impatient and sometimes have a hot temper.";

int main(int argc, char *argv[]) {
    //InitDiscord();
	SDL_SetEventFilter(HandleAppEvents, NULL);
	setvbuf(stdout, NULL, _IOFBF, 1024);
	MainApp = new Application(argc, argv);
	MainApp->Run();
    //Discord_Shutdown();
	return 0;
}
