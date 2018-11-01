#if defined _TimeZoneDatabase_included
  #endinput
#endif
#define _TimeZoneDatabase_included



/**
 *	Проверка что строка содержит валидное название таймзоны.
 * @param sTimezoneName			Строка содиржащая название таймзоны.
 *	-
 * @return	Возвращает true если таймзона валидна.
*/
native bool TZD_IsValidTimezone(const char[] sTimezoneName);

/**
 *	Получает таймзону сервера в sBuf.
 * @param sTimezoneName			Строка в которую будет записано название таймзоны.
 * @param iMaxlength			Длина строки.
 *	-
 * @return	Возвращает true если таймзону удалось получить.
*/
native bool TZD_GetServerTimezone(char[] sBuf, int iMaxlength);

/**
 *	Получает текущий LocalTimestamp указанный таймзоны.
 * @param sTimezoneName			Строка содиржащая название таймзоны.
 *	-
 * @return	Возвращает LocalTimestamp в случаи успеха.
 *	-
 * @error Название таймзоны не валидное.
*/
native int TZD_GetTimezoneCurrentLocalTime(const char[] sTimezoneName);

/**
 *	Получает указанный LocalTimestamp для указанной таймзоны.
 * @param sTimezoneNameOut		Строка содиржащая название таймзоны для которой будет получен LocalTimestamp.
 * @param iLocalTimestamp		LocalTimestamp.
 * @param sTimezoneNameIn		Строка содиржащая название таймзоны в которой передан LocalTimestamp.
 *	-
 * @return	Возвращает LocalTimestamp в случаи успеха.
 *	-
 * @error Название таймзоны не валидное или iLocalTimestamp меньше 0.
*/
native int TZD_GetTimezoneLocalTime(const char[] sTimezoneNameOut, int iLocalTimestamp, const char[] sTimezoneNameIn);

/**
 *	Является копией FormatTime, но использует gmtime.
 * @param sBuf					Строка в которую будет записано отформатированное время.
 * @param iMaxlength			Длина строки.
 * @param sFormat				Строка содержащая правила форматирования времени.
 * @param iLocalTimestamp		LocalTimestamp.
 *	-
 * @return	Возвращает true в случаи успеха.
 *	-
 * @error iLocalTimestamp меньше 0.
*/
native bool TZD_FormatTime(char[] sBuf, int iMaxlength, const char[] sFormat, int iLocalTimestamp);



public Extension __ext_TimeZoneDatabase = 
{
	name = "TimeZoneDatabase",
	file = "tzd.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public __ext_TimeZoneDatabase_SetNTVOptional()
{
	MarkNativeAsOptional("TZD_IsValidTimezone");
	MarkNativeAsOptional("TZD_GetServerTimezone");
	MarkNativeAsOptional("TZD_GetTimezoneCurrentLocalTime");
	MarkNativeAsOptional("TZD_GetTimezoneLocalTime");
	MarkNativeAsOptional("TZD_FormatTime");
}
#endif