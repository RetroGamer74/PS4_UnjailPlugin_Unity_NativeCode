
#include <kernel.h>

#define PRX_EXPORT extern "C" __declspec (dllexport)

#include "pluginunjail.h"

extern "C" {
#include "unjail.h"
}


#include <assert.h>
#define Assert assert

#include "PlayerInterface/UnityPrxPlugin.h"
#include "PlayerInterface/UnityEventQueue.h"
#include <system_service.h>



// ============================================================
// This event is triggered for SCE_SYSTEM_SERVICE_EVENT_ENTITLEMENT_UPDATE.
struct OnEntitlementUpdate { SceSystemServiceEvent params; };
REGISTER_EVENT_ID(0xDE76F015C0DE4BE8ULL, 0x9046B1153C877E39ULL, OnEntitlementUpdate)

void OnEntitlementUpdateHandler(const OnEntitlementUpdate &eventData)
{
	Assert(eventData.params.eventType == SCE_SYSTEM_SERVICE_EVENT_ENTITLEMENT_UPDATE);

	printf("SCE_SYSTEM_SERVICE_EVENT_ENTITLEMENT_UPDATE\n");
}

UnityEventQueue::StaticFunctionEventHandler<OnEntitlementUpdate>	g_OnEntitlementUpdateHandler(OnEntitlementUpdateHandler);


// ============================================================
// This event is triggered for SCE_SYSTEM_SERVICE_EVENT_APP_LAUNCH_LINK.
struct OnLaunchLink { SceSystemServiceEvent params; };
REGISTER_EVENT_ID(0x477AFB5C1CA045D6ULL, 0x95E9C61B8365A66AULL, OnLaunchLink)

void OnLaunchLinkHandler(const OnLaunchLink &eventData)
{
	Assert(eventData.params.eventType == SCE_SYSTEM_SERVICE_EVENT_APP_LAUNCH_LINK);

	uint32_t size = eventData.params.data.appLaunchLink.size;
	const uint8_t *  data = eventData.params.data.appLaunchLink.arg;

	printf("SCE_SYSTEM_SERVICE_EVENT_APP_LAUNCH_LINK\n");
}

UnityEventQueue::StaticFunctionEventHandler<OnLaunchLink>	g_OnLaunchLinkHandler(OnLaunchLinkHandler);




extern "C" int module_start(size_t sz, const void* arg)
{
	if (!ProcessPrxPluginArgs(sz, arg, "libPS4Unjail"))
	{
		// Failed.
		return -1;
	}

	// An example of handling system service events
	UnityEventQueue::IEventQueue* eventQueue = GetRuntimeInterface<UnityEventQueue::IEventQueue>(PRX_PLUGIN_IFACE_ID_GLOBAL_EVENT_QUEUE);
	eventQueue->AddHandler(&g_OnEntitlementUpdateHandler);
	eventQueue->AddHandler(&g_OnLaunchLinkHandler);

	return 0;
}

PRX_EXPORT int GetInteger()
{
	return 6;
}

PRX_EXPORT const char* GetString()
{
	return "Hello";
}

PRX_EXPORT int AddTwoIntegers(int i1, int i2)
{
	return i1 + i2;
}

PRX_EXPORT float AddTwoFloats(float f1, float f2)
{
	return f1 + f2;
}

PRX_EXPORT int FreeUnjail(void)
{
	struct thread td;
	return Sys::kexec((void *)&unjail455, &td);

}

PRX_EXPORT int GetPid(void)
{
	return Sys::getpid();

}

PRX_EXPORT int GetUid(void)
{
	return Sys::getuid();

}


struct ReturnedStructure
{
	int number;
	const char* text;
};

PRX_EXPORT bool ReturnAStructure(ReturnedStructure* data)
{
	static char sText[] = "Hello";
	data->number = 23;
	data->text = sText;

	return true;
}
