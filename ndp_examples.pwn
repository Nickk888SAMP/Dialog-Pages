#include <a_samp>
#include <zcmd>
#define YSI_NO_OPTIMISATION_MESSAGE
#define YSI_NO_VERSION_CHECK
#define YSI_NO_CACHE_MESSAGE
#define YSI_NO_MODE_CACHE

#define MAX_DIALOG_ITEMS 		150
#define NDP_AUTO_REMOVE_NEW_LINE false
#define NDP_USE_LOGGER
#include <ndialog-pages>

new ndp_e_str[128];

CMD:ndptest1(playerid, params[])
{
	for(new i; i < MAX_DIALOG_ITEMS; i++)
	{
		#if defined _INC_y_va
			AddDialogListitem(playerid, "{FFFFFF}List Item {FF00FF}%i", i);
		#else
			format(ndp_e_str, sizeof ndp_e_str, "{FFFFFF}List Item {FF00FF}%i", i);
			AddDialogListitem(playerid, ndp_e_str);
		#endif
	}
	ShowPlayerDialog(playerid, 586, DIALOG_STYLE_LIST, "{FFFFFF}Test Dialog ID {FF00FF}586", #, "Button 1", "Button 2", 15);
	return 1;
}

CMD:ndptest2(playerid, params[])
{
	for(new i; i < MAX_DIALOG_ITEMS; i++)
	{
		#if defined _INC_y_va
			AddDialogListitem(playerid, "{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i", i, i, i, i);
		#else
			format(ndp_e_str, sizeof ndp_e_str, "{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i", i, i, i, i);
			AddDialogListitem(playerid, ndp_e_str);
		#endif
	}
	ShowPlayerDialog(playerid, 586, DIALOG_STYLE_TABLIST, "{FFFFFF}Test Dialog ID {FF00FF}586", #, "Button 1", "Button 2", 27);
	return 1;
}

CMD:ndptest3(playerid, params[])
{
	AddDialogListitem(playerid, "{FFFFFF}Column 1\t{FF00FF}Column 2\t{FFFF00}Column 3\t{00FFFF}Column 4");
	for(new i; i < MAX_DIALOG_ITEMS; i++)
	{
		#if defined _INC_y_va
			AddDialogListitem(playerid, "{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i", i, i, i, i);
		#else
			format(ndp_e_str, sizeof ndp_e_str, "{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i", i, i, i, i);
			AddDialogListitem(playerid, ndp_e_str);
		#endif
	}
	ShowPlayerDialog(playerid, 586, DIALOG_STYLE_TABLIST_HEADERS, "{FFFFFF}Test Dialog ID {FF00FF}586", #, "Button 1", "Button 2", 32);
	return 1;
}

CMD:ndptest4(playerid, params[])
{
	ClearDialogListitems(playerid);
	for(new i; i < MAX_DIALOG_ITEMS; i++)
	{
		#if defined _INC_y_va
			AddDialogListitem(playerid, "{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i", i, i, i, i);
		#else
			format(ndp_e_str, sizeof ndp_e_str, "{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i\t{FFFFFF}List Item {FF00FF}%i", i, i, i, i);
			AddDialogListitem(playerid, ndp_e_str);
		#endif
	}
	ShowPlayerDialogPages(playerid, "NDP_Test", DIALOG_STYLE_LIST, "{FFFFFF}Test Dialog ID {FF00FF}586 {FFFFFF}easyDialog", "Button 1", "Button 2", 15);
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if(dialogid == 586 && response)
	{
		if(listitem != INVALID_LISTITEM)
		{
			format(ndp_e_str, sizeof ndp_e_str, "{FFFF00}[NDialog-Pages] {FFFFFF}You have selected listitem ID: {FFFF00}%i", listitem);
			SendClientMessage(playerid, -1, ndp_e_str);
		}
	}
	return 0;
}

DialogPages:NDP_Test(playerid, response, listitem, inputtext[])
{
	Logger_Log("D_NDP_Test Called.", 
		Logger_I("playerid", playerid),
		Logger_I("response", response),
		Logger_I("listitem", listitem),
		Logger_S("inputtext", inputtext));

	format(ndp_e_str, sizeof ndp_e_str, "{FFFF00}[NDialog-Pages] {FFFFFF}You have selected listitem ID: {FFFF00}%i", listitem);
	SendClientMessage(playerid, -1, ndp_e_str);
	return 1;
}

public OnDialogPagesPerformed(playerid, const dialog[], response, bool:success, bool:pagechange)
{
	Logger_Log("OnDialogPagesPerformed Called.", 
		Logger_I("playerid", playerid),
		Logger_S("dialog", dialog),
		Logger_I("response", response),
		Logger_B("success", success),
		Logger_B("pagechange", pagechange));
	return 1;
}