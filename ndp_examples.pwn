#include <a_samp>
#include <zcmd>
#define YSI_NO_OPTIMISATION_MESSAGE
#define YSI_NO_VERSION_CHECK
#define YSI_NO_CACHE_MESSAGE
#define YSI_NO_MODE_CACHE

#define MAX_DIALOG_ITEMS 		500
#include <ndialog-pages>

CMD:ndptest1(playerid, params[])
{
	new string[256];
	for(new i; i < MAX_DIALOG_ITEMS; i++)
	{
		format(string, sizeof string, "{FFFFFF}List Item {FF00FF}%i", i);
		AddDialogListitem(playerid, string);
	}
	ShowPlayerDialog(playerid, 586, DIALOG_STYLE_LIST, "{FFFFFF}Test Dialog ID {FF00FF}586", #, "Button 1", "Button 2", 25);
	return 1;
}

CMD:ndptest2(playerid, params[])
{
	new string[256];
	for(new i; i < MAX_DIALOG_ITEMS; i++)
	{
		format(string, sizeof string, "{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i", i, i, i, i);
		AddDialogListitem(playerid, string);
	}
	ShowPlayerDialog(playerid, 586, DIALOG_STYLE_TABLIST, "{FFFFFF}Test Dialog ID {FF00FF}586", #, "Button 1", "Button 2", 25);
	return 1;
}


CMD:ndptest3(playerid, params[])
{
	new string[256];
	AddDialogListitem(playerid, "{FFFFFF}Column 1\t{FF00FF}Column 2\t{FFFF00}Column 3\t{00FFFF}Column 4");
	for(new i; i < MAX_DIALOG_ITEMS; i++)
	{
		format(string, sizeof string, "{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i", i, i, i, i);
		AddDialogListitem(playerid, string);
	}
	ShowPlayerDialog(playerid, 586, DIALOG_STYLE_TABLIST_HEADERS, "{FFFFFF}Test Dialog ID {FF00FF}586", #, "Button 1", "Button 2", 25);
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	new string[256];
	if(dialogid == 586 && response)
	{
		if(listitem != INVALID_LISTITEM)
		{
			format(string, sizeof string, "{FFFFFF}[NDialog-Pages] {FFFF00}You have selected listitem ID: %i", listitem);
			SendClientMessage(playerid, -1, string);
		}
	}
	return 0;
}


