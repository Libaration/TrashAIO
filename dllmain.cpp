#include "../plugin_sdk/plugin_sdk.hpp"
#include "Leblanc.h"
#include "Helpers.h"

PLUGIN_NAME(AIO_NAME);
SUPPORTED_CHAMPIONS(champion_id::Leblanc);

PLUGIN_API bool on_sdk_load(plugin_sdk_core* plugin_sdk_good)
{
	DECLARE_GLOBALS(plugin_sdk_good);

	switch (myhero->get_champion())
	{
		case champion_id::Leblanc:
			leblanc::load();
			return true;
		default:
			break;
	}
	return false;
}

PLUGIN_API void on_sdk_unload()
{
	switch (myhero->get_champion())
	{
		case champion_id::Leblanc:
			leblanc::unload();
			break;
		default:
			break;
	}
}
