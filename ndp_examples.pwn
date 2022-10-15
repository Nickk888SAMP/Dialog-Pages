#include <a_samp>

#define YSI_NO_OPTIMISATION_MESSAGE
#define YSI_NO_VERSION_CHECK
#define YSI_NO_CACHE_MESSAGE
#define YSI_NO_MODE_CACHE

#define MAX_DIALOG_ITEMS 		150
#define NDP_AUTO_REMOVE_NEW_LINE false

#define NDP_USE_LOGGER
#include <ndialog-pages>
#include <zcmd>

new ndp_e_str[128];

/* Commands */
CMD:ndptest1(playerid, params[]) // DIALOG_STYLE_LIST
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
	ShowPlayerDialogPages(playerid, "NDP_Test", DIALOG_STYLE_LIST, "{FFFFFF}Test Dialog Name {FF00FF}NDP_Test", "Button 1", "Button 2", 15);
	return 1;
}

CMD:ndptest2(playerid, params[]) // DIALOG_STYLE_TABLIST
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
	ShowPlayerDialogPages(playerid, "NDP_Test", DIALOG_STYLE_TABLIST, "{FFFFFF}Test Dialog Name {FF00FF}NDP_Test", "Button 1", "Button 2", 27);
	return 1;
}

CMD:ndptest3(playerid, params[]) // DIALOG_STYLE_TABLIST_HEADERS
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
	ShowPlayerDialogPages(playerid, "NDP_Test", DIALOG_STYLE_TABLIST_HEADERS, "{FFFFFF}Test Dialog Name {FF00FF}NDP_Test", "Button 1", "Button 2", 32);
	return 1;
}

/* Callback */

DialogPages:NDP_Test(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 1;
	
	format(ndp_e_str, sizeof ndp_e_str, "[NDialog-Pages] You have selected listitem ID: {666666}%i{FFFFFF}, listitem's text: {666666}%s", listitem, inputtext);
	SendClientMessage(playerid, -1, ndp_e_str);
	print(ndp_e_str);
	return 1;
}
