#include "extension.h"
#include "date/tz.h"
#include <string.h>

using namespace date;
using namespace std::chrono;

TZD g_TZD;
SMEXT_LINK(&g_TZD);

bool TZD::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	char path[PLATFORM_MAX_PATH];
	g_pSM->BuildPath(Path_SM, path, sizeof(path), "data/tzdata");
	set_install(path);
	
	//init db
	try
	{
		reload_tzdb();
	}
	catch (std::exception &e)
	{
		strncpy(error, e.what(), maxlength);
		return false;
	}
	
	g_pShareSys->AddNatives(myself, g_ExtensionNatives);
	g_pShareSys->RegisterLibrary(myself, "TimeZoneDatabase");
	
	return true;
}

static cell_t TZD_IsValidTimezone(IPluginContext *pContext, const cell_t *params)
{
	char *tz_name;
	pContext->LocalToString(params[1], &tz_name);
	return IsValidTimezone(tz_name);
}

static cell_t TZD_GetServerTimezone(IPluginContext *pContext, const cell_t *params)
{
	const char *tz_name = "";
	
	try
	{
		tz_name = current_zone()->name().c_str();
	}
    catch (std::exception &)
    {
        return false;
    }
	
	pContext->StringToLocal(params[1], params[2], tz_name);
	
	return true;
}

static cell_t TZD_GetTimezoneCurrentLocalTime(IPluginContext *pContext, const cell_t *params)
{
	char *tz_name;
	pContext->LocalToString(params[1], &tz_name);
	
	if(!IsValidTimezone(tz_name))
	{
		return pContext->ThrowNativeError("Timezone '%s' not found", tz_name);
	}
	
	#ifdef _WIN32
	auto сurrent_time = system_clock::now();
	#else
	//system_clock::now() - /game/bin/libstdc++.so.6: version `GLIBCXX_3.4.19' not found.
	//Поэтому использую system_clock::from_time_t(time(NULL))
	auto сurrent_time = system_clock::from_time_t(time(NULL));
	#endif
	auto zone = make_zoned(tz_name, сurrent_time);
	
	return (cell_t)(floor<seconds>(zone.get_local_time()).time_since_epoch().count());
}

static cell_t TZD_GetTimezoneLocalTime(IPluginContext *pContext, const cell_t *params)
{
	char *tz_out_name, *tz_in_name;
	pContext->LocalToString(params[1], &tz_out_name);
	pContext->LocalToString(params[3], &tz_in_name);
	
	if(!IsValidTimezone(tz_out_name))
	{
		return pContext->ThrowNativeError("Timezone out '%s' not found", tz_out_name);
	}
	
	if(params[2] < 0)
	{
		return pContext->ThrowNativeError("Timestamp must be positive");
	}
	
	if(!IsValidTimezone(tz_in_name))
	{
		return pContext->ThrowNativeError("Timezone in '%s' not found", tz_in_name);
	}
	
	auto in_zone = make_zoned(tz_in_name, local_seconds{seconds{params[2]}});
	auto zone = make_zoned(tz_out_name, in_zone);
	
	return (cell_t)(floor<seconds>(zone.get_local_time()).time_since_epoch().count());
}

//Взято https://github.com/alliedmodders/sourcemod/blob/master/extensions/sdktools/vhelpers.cpp#L495
#if defined SUBPLATFORM_SECURECRT
void _ignore_invalid_parameter(
	const wchar_t * expression,
	const wchar_t * function, 
	const wchar_t * file,
	unsigned int line,
	uintptr_t pReserved
	)
{
	/* Wow we don't care, thanks Microsoft. */
}
#endif

//Взято https://github.com/alliedmodders/sourcemod/blob/master/core/logic/smn_core.cpp#L189
static cell_t TZD_FormatTime(IPluginContext *pContext, const cell_t *params)
{
	char *format, *buffer;
	pContext->LocalToString(params[1], &buffer);
	pContext->LocalToString(params[3], &format);
	
	if(params[2] < 0)
	{
		return pContext->ThrowNativeError("Timestamp must be positive");
	}
	
#if defined SUBPLATFORM_SECURECRT
	_invalid_parameter_handler handler = _set_invalid_parameter_handler(_ignore_invalid_parameter);
#endif
	
	time_t t = (time_t)params[4];
	size_t written = strftime(buffer, params[2], format, gmtime(&t));
	
#if defined SUBPLATFORM_SECURECRT
	_set_invalid_parameter_handler(handler);
#endif
	
	if (format[0] != '\0' && !written)
	{
		return 0;
	}
	
	return 1;
}



bool IsValidTimezone(const char* tz_name)
{
	try
	{
		make_zoned(tz_name);
	}
	catch (std::exception &)
	{
		return false;
	}
	return true;
}

const sp_nativeinfo_t g_ExtensionNatives[] =
{
	{ "TZD_IsValidTimezone",				TZD_IsValidTimezone },
	{ "TZD_GetServerTimezone",				TZD_GetServerTimezone },
	{ "TZD_GetTimezoneCurrentLocalTime",	TZD_GetTimezoneCurrentLocalTime },
	{ "TZD_GetTimezoneLocalTime",			TZD_GetTimezoneLocalTime },
	{ "TZD_FormatTime",						TZD_FormatTime },
	{ NULL,									NULL }
};