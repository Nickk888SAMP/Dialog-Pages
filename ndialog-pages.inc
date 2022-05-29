/**********************************************************************************************************************************

Script: NDialog-Pages
Type: Include
Version: 2.5
Author: Nickk888
Date: 29.05.2022
Repository: https://github.com/Nickk888SAMP/Dialog-Pages

Natives:
AddDialogListitem(playerid, const itemstring[]);
AddDialogListitem(playerid, const itemstring[], va_args<>); // Only works when y_va is included
ClearDialogListitems(playerid);
ShowPlayerDialog(playerid, dialogid, style, caption[], #, button1[], button2[], items_per_page = 0, nextbutton[] = "{FF0000}>>>", backbutton[] = "{FF0000}<<<");

**********************************************************************************************************************************/

/************************************************************
                        INCLUDES
*************************************************************/

#tryinclude 							<YSI_Coding\y_hooks> //By Y_Less | Thank you, great include <3
#tryinclude 							<YSI_Coding\y_va> //By Y_Less | Thank you, great include <3
//#include 								<logger> //Uncomment to include logger, useful for debugging.

/************************************************************
                        DEFINITIONS
*************************************************************/

#if !defined MAX_DIALOG_ITEMS
	#define MAX_DIALOG_ITEMS 			500 //Max dialog items, you can change this if you want, you can also redefine it using #define MAX_DIALOG_ITEMS your_amount BEFORE you include this!
#endif

/************************************************************
                        INTERNALS
*************************************************************/

#define DIALOG_MAX_LISTITEM_SIZE 		256 					//The cell size of one listitem, Don't change this!
#define DIALOG_MAX_INFO_SIZE 			4096 					//The cell size of the whole dialog, Don't change this!
#define DIALOG_STRING_PUFFER_SIZE		132 					//The margin for the dialog len size so it won't cut off the string and bug everything, Don't change this!
#define INVALID_LISTITEM 				(0xFFFF) 			//The definition for an Invalid Listitem, don't change this if you don't have to, don't go above -1
#define NDP_AUTO						(MAX_DIALOG_ITEMS) 		//Be so kind and don't change this :D

/************************************************************
                        ENUMERATORS
*************************************************************/

enum ndialogpages_data
{
	bool:ndp_dialogopened,
	bool:ndp_isstylepage,
	ndp_dialogitemscount,
	ndp_amountperpage,
	ndp_endindex[MAX_DIALOG_ITEMS],
	ndp_itemsonpage,
	ndp_page,
	ndp_caption[64],
	ndp_button1[64],
	ndp_button2[64],
	ndp_nextbutton[DIALOG_MAX_LISTITEM_SIZE],
	ndp_backbutton[DIALOG_MAX_LISTITEM_SIZE],
	ndp_style,
	ndp_dialogid
};
new NDP_P[MAX_PLAYERS][ndialogpages_data];

// This stores all the items for the dialog
new NDP_DialogInfo[MAX_PLAYERS][MAX_DIALOG_ITEMS][DIALOG_MAX_LISTITEM_SIZE];

// This is used as a temporary item string.
new NDP_DialogString[DIALOG_MAX_INFO_SIZE];

/************************************************************
                        FUNCTIONS
*************************************************************/

/* Clears the Dialog listitems cache */
stock ClearDialogListitems(playerid)
{
	NDP_P[playerid][ndp_dialogopened] = false;
	NDP_P[playerid][ndp_dialogitemscount] = 0;
	//Logger for Debug purpose
	#if defined _logger_included
	Logger_Log("ndialog-pages", 
		Logger_S("function", "ClearDialogListitems"),
		Logger_I("playerid", playerid));
	#endif
	return 1;
}

/* Adds an item to the dialog items cache */
#if defined _INC_y_va
	stock AddDialogListitem(playerid, const itemstr[], va_args<>)
#else
	stock AddDialogListitem(playerid, const itemstr[])
#endif
{
	#if defined _INC_y_va
		new distr[145];
		va_format(distr, sizeof (distr), itemstr, va_start<2>);
	#endif

	// Try's to reset the items if a dialog has been already opened.
	if(NDP_P[playerid][ndp_dialogopened])
		ClearDialogListitems(playerid);
	
	// Checks if there is still room to add an item into memory.
	if(NDP_P[playerid][ndp_dialogitemscount] < MAX_DIALOG_ITEMS)
	{
		new index = NDP_P[playerid][ndp_dialogitemscount];
		#if defined _INC_y_va
			format(NDP_DialogInfo[playerid][index], DIALOG_MAX_LISTITEM_SIZE, distr);
		#else
			format(NDP_DialogInfo[playerid][index], DIALOG_MAX_LISTITEM_SIZE, itemstr);
		#endif
		// Checks if there is any "\n" in the itemstring and deletes it because it won't be needed and it can cause problems...
		new ntd_f;
		for(;;)
		{
			ntd_f = strfind(NDP_DialogInfo[playerid][index], "\n", true);
			if(ntd_f == -1)
				break;
			strdel(NDP_DialogInfo[playerid][index], ntd_f, ntd_f + 2);
		}
		// Counts the items that has been added.
		NDP_P[playerid][ndp_dialogitemscount]++;
		// Logger for Debug purpose
		#if defined _logger_included
		Logger_Log("ndialog-pages",
			Logger_S("function", "AddDialogListitem"),
			Logger_I("playerid", playerid),
			Logger_I("index", index),
			Logger_S("data", NDP_DialogInfo[playerid][index]));
		#endif
		return 1;
	}
	return 0;
}

/************************************************************
                	INTERNAL FUNCTIONS
*************************************************************/

forward NDP_ShowPlayerDialog(playerid, dialogid, style, const caption[], const info[], const button1[], const button2[], items_per_page = 0, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<");
stock NDP_ShowPlayerDialog(playerid, dialogid, style, const caption[], const info[], const button1[], const button2[], items_per_page = 0, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<")
{
	NDP_P[playerid][ndp_isstylepage] = false;
	if(items_per_page > 0)
	{
		// Saving all data into the enumerator
		NDP_P[playerid][ndp_dialogid] = dialogid;
		NDP_P[playerid][ndp_style] = style;
		NDP_P[playerid][ndp_page] = 0;
		NDP_P[playerid][ndp_amountperpage] = items_per_page;
		NDP_P[playerid][ndp_endindex] = 0;
		NDP_P[playerid][ndp_isstylepage] = true;
		
		format(NDP_P[playerid][ndp_button1], 64, button1);
		format(NDP_P[playerid][ndp_button2], 64, button2);
		format(NDP_P[playerid][ndp_caption], 64, caption);
		format(NDP_P[playerid][ndp_nextbutton], DIALOG_MAX_LISTITEM_SIZE, nextbutton);
		format(NDP_P[playerid][ndp_backbutton], DIALOG_MAX_LISTITEM_SIZE, backbutton);
		
		// Calculating the pages
		NDP_CalculateListitemsPerPage(playerid);
		
		// Show
		NDP_P[playerid][ndp_dialogopened] = true;
		NDP_ShowDialogPage(playerid, NDP_P[playerid][ndp_page]);
	}
	else ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2); //Default
	return 1;
}

//This is an internal function, look complicated huh? It kinda is :D
//In a nutshell it calculates how many items will fit into ONE page using the LEN size of all the items and buttons etc.
stock NDP_CalculateListitemsPerPage(playerid)
{
	new ndp_len, ndp_pagelist, ndp_counter;
	new npd_buttonslen = (strlen(NDP_P[playerid][ndp_nextbutton]) + strlen(NDP_P[playerid][ndp_backbutton]) + 4);

	#if defined _logger_included
	Logger_Log("ndialog-pages",
		Logger_S("function", "NDP_CalculateListitemsPerPage Loop"),
		Logger_I("ndp_amountperpage", NDP_P[playerid][ndp_amountperpage]),
		Logger_I("ndp_dialogitemscount", NDP_P[playerid][ndp_dialogitemscount]),
		Logger_I("npd_buttonslen", npd_buttonslen));
	#endif

	for(new i = (NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS ? 1 : 0); i < NDP_P[playerid][ndp_dialogitemscount]; i++)
	{
		ndp_len += (strlen(NDP_DialogInfo[playerid][(i < (NDP_P[playerid][ndp_dialogitemscount] - 1)) ? (i + 1) : (i)]) + 2);
		if((ndp_counter >= NDP_P[playerid][ndp_amountperpage]) || ((ndp_len + npd_buttonslen) >= (DIALOG_MAX_INFO_SIZE - DIALOG_STRING_PUFFER_SIZE)))
		{
			#if defined _logger_included
			Logger_Log("ndialog-pages",
				Logger_S("function", "NDP_CalculateListitemsPerPage Loop Counter"),
				Logger_I("ndp_pagelist", ndp_pagelist),
				Logger_I("iterator", i),
				Logger_I("ndp_counter", ndp_counter));
			#endif

			ndp_pagelist++, 
			ndp_len = 0, 
			ndp_counter = 0,
			i--;
		}
		else ndp_counter++;
		NDP_P[playerid][ndp_endindex][ndp_pagelist] = (i + 1);

		#if defined _logger_included
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_CalculateListitemsPerPage Loop"),
			Logger_I("ndp_pagelist", NDP_P[playerid][ndp_endindex][ndp_pagelist]),
			Logger_I("iterator", i),
			Logger_I("ndp_counter", ndp_counter));
		#endif
	}
	NDP_P[playerid][ndp_endindex][ndp_pagelist + (NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS ? 1 : 0)] = NDP_P[playerid][ndp_dialogitemscount];

	#if defined _logger_included
	Logger_Log("ndialog-pages",
		Logger_S("function", "NDP_CalculateListitemsPerPage After Loop"),
		Logger_I("ndp_pagelist", NDP_P[playerid][ndp_endindex][ndp_pagelist]));
	#endif
	return 1;
}

// Another internal function, this will generate the dialog page.
stock NDP_ShowDialogPage(playerid, ndppage)
{
	// Getting the Start and End Listitem Index of the current page
	new startindex = (ndppage > 0) ? (NDP_P[playerid][ndp_endindex][ndppage - 1]) : (0);
	new endindex = NDP_P[playerid][ndp_endindex][ndppage];

	#if defined _logger_included
	Logger_Log("ndialog-pages",
		Logger_S("function", "NDP_ShowDialogPage"),
		Logger_I("playerid", playerid),
		Logger_I("page", ndppage),
		Logger_I("startindex", startindex),
		Logger_I("endindex", endindex));
	#endif

	// Setting the values
	NDP_DialogString = "";
	NDP_P[playerid][ndp_itemsonpage] = 0;
	NDP_P[playerid][ndp_page] = ndppage;

	// Generate the Header if needed
	if(NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS && NDP_P[playerid][ndp_itemsonpage] == 0)
	{
		strcat(NDP_DialogString, NDP_DialogInfo[playerid][0]);
		strcat(NDP_DialogString, "\n");

		#if defined _logger_included
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_ShowDialogPage"),
			Logger_I("index", 0),
			Logger_S("listitemadded", NDP_DialogInfo[playerid][0]));
		#endif
	}

	// Generate the list
	for(new i = (NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS && ndppage == 0 ? startindex + 1 : startindex) ; i < endindex; i++)
	{
		// Generate each listitem
		strcat(NDP_DialogString, NDP_DialogInfo[playerid][i]);
		strcat(NDP_DialogString, "\n");

		#if defined _logger_included
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_ShowDialogPage"),
			Logger_I("index", i),
			Logger_S("listitemadded", NDP_DialogInfo[playerid][i]));
		#endif

		// Keeping track on how much listitems are on the current page
		NDP_P[playerid][ndp_itemsonpage]++;
	}

	#if defined _logger_included
	Logger_Log("ndialog-pages",
		Logger_S("function", "NDP_ShowDialogPage"),
		Logger_I("itemsonpage", NDP_P[playerid][ndp_itemsonpage]));
	#endif

	// Generate Button "Next"
	if(endindex != 0 && endindex < (NDP_P[playerid][ndp_dialogitemscount]))
	{
		strcat(NDP_DialogString, "\n");
		strcat(NDP_DialogString, NDP_P[playerid][ndp_nextbutton]);

		#if defined _logger_included
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_ShowDialogPage"),
			Logger_S("button created", "next"));
		#endif
	}
	// Generate Button "Back"
	if(startindex != 0)
	{
		strcat(NDP_DialogString, "\n");
		strcat(NDP_DialogString, NDP_P[playerid][ndp_backbutton]);

		#if defined _logger_included
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_ShowDialogPage"),
			Logger_S("button created", "back"));
		#endif
	}
	// Show Dialog
	if(NDP_P[playerid][ndp_itemsonpage] > 0 || NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS)
	{
		#if defined _logger_included
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_ShowDialogPage"),
			Logger_I("showingdialogforplayer", playerid));
		#endif

		ShowPlayerDialog(playerid, NDP_P[playerid][ndp_dialogid], NDP_P[playerid][ndp_style], NDP_P[playerid][ndp_caption], NDP_DialogString, NDP_P[playerid][ndp_button1], NDP_P[playerid][ndp_button2]);
		return 1;
	}
	return 0;
}

/************************************************************
						CALLBACKS
*************************************************************/

#if defined _INC_y_hooks
	hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
#else
	public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
#endif
{
	if(dialogid == NDP_P[playerid][ndp_dialogid] && NDP_P[playerid][ndp_isstylepage] && NDP_P[playerid][ndp_dialogopened])
	{
		new page = NDP_P[playerid][ndp_page];
		new startindex = (page > 0) ? (NDP_P[playerid][ndp_endindex][page - 1]) : (0);
		new ndp_tmplistitem = startindex + listitem;
		#if defined _logger_included
		Logger_Log("ndialog-pages",
			Logger_S("callback", "OnDialogResponse"),
			Logger_I("ndp_dialogitemscount", NDP_P[playerid][ndp_dialogitemscount]),
			Logger_I("page", page),
			Logger_I("startindex", startindex),
			Logger_I("ndp_tmplistitem", ndp_tmplistitem));
		#endif
		if(NDP_P[playerid][ndp_dialogitemscount] > (NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS ? 1 : 0))
		{
			if(ndp_tmplistitem < NDP_P[playerid][ndp_dialogitemscount])
			{
				//Button "Next"
				if(listitem == (NDP_P[playerid][ndp_itemsonpage])) 
				{
					#if defined _logger_included
					Logger_Log("ndialog-pages",
						Logger_S("callback", "OnDialogResponse"),
						Logger_S("button clicked", "next"));
					#endif
					NDP_P[playerid][ndp_page]++;
					if(response) NDP_ShowDialogPage(playerid, NDP_P[playerid][ndp_page]);
					listitem = INVALID_LISTITEM;
				}
				//Button "Back"
				else if(listitem == (NDP_P[playerid][ndp_itemsonpage] + 1)) 
				{
					#if defined _logger_included
					Logger_Log("ndialog-pages",
						Logger_S("callback", "OnDialogResponse"),
						Logger_S("button clicked", "back"));
					#endif
					NDP_P[playerid][ndp_page]--;
					if(response) NDP_ShowDialogPage(playerid, NDP_P[playerid][ndp_page]);
					listitem = INVALID_LISTITEM;
				}
				//Normal item
				else 
				{
					listitem = ndp_tmplistitem - ((NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS && page > 0) ? (1) : (0)); 
					#if defined _logger_included
					Logger_Log("ndialog-pages",
						Logger_S("callback", "OnDialogResponse"),
						Logger_S("button clicked", "item"),
						Logger_I("listitem", listitem));
					#endif
				}
			}
			else if(NDP_P[playerid][ndp_dialogitemscount] > 0)
			{
				//Also button "Back", but on the last page
				#if defined _logger_included
				Logger_Log("ndialog-pages",
					Logger_S("callback", "OnDialogResponse"),
					Logger_S("button clicked", "back - last page"));
				#endif
				NDP_P[playerid][ndp_page]--;
				if(response) NDP_ShowDialogPage(playerid, NDP_P[playerid][ndp_page]);
				listitem = INVALID_LISTITEM;
			}
		}
		else listitem = INVALID_LISTITEM;
	}
	return 0;
}

/************************************************************
                	_ALS_ HOOKING
*************************************************************/

#if !defined _INC_y_hooks
	#if defined _ALS_OnDialogResponse
		#undef OnDialogResponse
	#else
		#define _ALS_OnDialogResponse
	#endif

	#define OnDialogResponse NDP_OnDialogResponse
#endif

// ALS Hooking
#if defined _ALS_ShowPlayerDialog
    #undef ShowPlayerDialog
#else
    #define _ALS_ShowPlayerDialog
#endif
#define ShowPlayerDialog NDP_ShowPlayerDialog

//