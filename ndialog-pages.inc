/**********************************************************************************************************************************

Script: NDialog-Pages
Type: Include
Version: 3.1.0
Author: Nickk888
Date: 21.08.2022
Repository: https://github.com/Nickk888SAMP/Dialog-Pages

Natives:
AddDialogListitem(playerid, const itemstring[]);
AddDialogListitem(playerid, const itemstring[], va_args<>); // Only works when y_va is included
ClearDialogListitems(playerid);
ShowPlayerDialogPages(playerid, const function[], style, const caption[], const button1[], const button2[], items_per_page, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<");

**********************************************************************************************************************************/

/************************************************************
                        INCLUDES
*************************************************************/

#tryinclude 								<YSI_Coding\y_hooks> //By Y_Less | Thank you, great include <3
#tryinclude 								<YSI_Coding\y_va> //By Y_Less | Thank you, great include <3
#tryinclude									<YSI_Coding\y_inline> //By Y_Less | Support added by Tiaansu
#tryinclude 								<vector> //By BigETI | Support added by wiger3
#if defined NDP_USE_LOGGER	
	#include 								<logger> //Uncomment to include logger, useful for debugging.
#endif

/************************************************************
                        FORWARDS
*************************************************************/

forward ShowPlayerDialogPages(playerid, const function[], style, const caption[], const button1[], const button2[], items_per_page, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<");
forward OnDialogPagesPerformed(playerid, const dialog[], response, bool:success, bool:pagechange);

/************************************************************
                        DEFINITIONS
*************************************************************/

// Disables or Enables automatic removal of "\n" inside list items. Disable this if you know you have no \n added to your listitem.
#if !defined NDP_AUTO_REMOVE_NEW_LINE
	#define NDP_AUTO_REMOVE_NEW_LINE	true						
#endif

// Max dialog items, you can change this if you want, you can also redefine it using #define MAX_DIALOG_ITEMS your_amount BEFORE you include this!
#if !defined __VECTOR_INCLUDED__
	#if !defined MAX_DIALOG_ITEMS										
		#define MAX_DIALOG_ITEMS 			500 						
	#endif
#endif

#define DialogPages:%0(%1) forward ndpD_%0(%1); public ndpD_%0(%1)

/************************************************************
                        INTERNALS
*************************************************************/

// Only change when you know what you are doing!
#define DIALOG_MAX_LISTITEM_SIZE 		256 						// The cell size of one listitem
#define DIALOG_MAX_INFO_SIZE 			4096 						// The cell size of the whole dialog
#define DIALOG_STRING_PUFFER_SIZE		132 						// The margin for the dialog len size so it won't cut off the string and bug everything.
#define INVALID_LISTITEM 				(0xFFFF) 				// The definition for an Invalid Listitem, don't go above -1
//#define NDP_AUTO						(MAX_DIALOG_ITEMS) 			// Defines the size of the automatic detection definition of list items amount per page.

#if YSI_VERSION_MAJOR == 5 && defined _INC_y_inline
	static stock
		NDP_DialogName[64] = "Inline_DialogPages",
		Func:NDP_InlineCB<iis>;
#endif

/************************************************************
                        ENUMERATORS
*************************************************************/

static enum
{
	NDP_DIALOG_TYPE_DIALOG
};

#if defined __VECTOR_INCLUDED__
	static enum E_DIALOGPAGES_DATA
	{
		bool:ndp_dialogOpened,
		ndp_dialogItemsCount,
		ndp_amountPerPage,
		Vector:ndp_endIndex,
		ndp_itemsonpage,
		ndp_page,
		ndp_caption[64],
		ndp_button1[64],
		ndp_button2[64],
		ndp_nextButton[DIALOG_MAX_LISTITEM_SIZE],
		ndp_backButton[DIALOG_MAX_LISTITEM_SIZE],
		ndp_style,
		ndp_dialogid,
		ndp_type,
		ndp_dialogName[32]
	};
#else
	static enum E_DIALOGPAGES_DATA
	{
		bool:ndp_dialogOpened,
		ndp_dialogItemsCount,
		ndp_amountPerPage,
		ndp_endIndex[MAX_DIALOG_ITEMS],
		ndp_itemsonpage,
		ndp_page,
		ndp_caption[64],
		ndp_button1[64],
		ndp_button2[64],
		ndp_nextButton[DIALOG_MAX_LISTITEM_SIZE],
		ndp_backButton[DIALOG_MAX_LISTITEM_SIZE],
		ndp_style,
		ndp_dialogid,
		ndp_type,
		ndp_dialogName[32]
	};
#endif
static NDP_P[MAX_PLAYERS][E_DIALOGPAGES_DATA];

// This stores all the items for the dialog
#if defined __VECTOR_INCLUDED__
	static Vector:NDP_DialogInfo[MAX_PLAYERS];
#else
	static NDP_DialogInfo[MAX_PLAYERS][MAX_DIALOG_ITEMS][DIALOG_MAX_LISTITEM_SIZE];
#endif

// This is used as a temporary item string.
static NDP_DialogString[DIALOG_MAX_INFO_SIZE];

/************************************************************
                        FUNCTIONS
*************************************************************/

/* Clears the Dialog listitems cache */
stock ClearDialogListitems(playerid)
{
	NDP_P[playerid][ndp_dialogOpened] = false;
	NDP_P[playerid][ndp_dialogItemsCount] = 0;
	NDP_P[playerid][ndp_caption] = 0;
	NDP_P[playerid][ndp_button1] = 0;
	NDP_P[playerid][ndp_button2] = 0;
	NDP_P[playerid][ndp_nextButton] = 0;
	NDP_P[playerid][ndp_backButton] = 0;
	NDP_P[playerid][ndp_dialogName][0] = EOS;
	#if defined __VECTOR_INCLUDED__
		VECTOR_clear(NDP_DialogInfo[playerid]);
	#endif
	//Logger for Debug purpose
	#if defined NDP_USE_LOGGER
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
	// Try's to reset the items if a dialog has been already opened.
	if(NDP_P[playerid][ndp_dialogOpened])
		ClearDialogListitems(playerid);
	
	#if !defined __VECTOR_INCLUDED__
	// Checks if there is still room to add an item into memory.
	if(NDP_P[playerid][ndp_dialogItemsCount] < MAX_DIALOG_ITEMS)
	{
	#endif
	#if !defined __VECTOR_INCLUDED__ || defined NDP_USE_LOGGER || NDP_AUTO_REMOVE_NEW_LINE
		new index = NDP_P[playerid][ndp_dialogItemsCount];
	#endif
	#if defined _INC_y_va
		new distr[145];
		va_format(distr, sizeof (distr), itemstr, va_start<2>);
		#if defined __VECTOR_INCLUDED__
			VECTOR_push_back_str(NDP_DialogInfo[playerid], distr);
		#else
			format(NDP_DialogInfo[playerid][index], DIALOG_MAX_LISTITEM_SIZE, distr);
		#endif
	#else
		#if defined __VECTOR_INCLUDED__
			VECTOR_push_back_str(NDP_DialogInfo[playerid], itemstr);
		#else
			format(NDP_DialogInfo[playerid][index], DIALOG_MAX_LISTITEM_SIZE, itemstr);
		#endif
	#endif
	#if NDP_AUTO_REMOVE_NEW_LINE == true
		// Checks if there is any "\n" in the itemstring and deletes it because it won't be needed and it can cause problems...
		new ntd_f;
		#if defined __VECTOR_INCLUDED__
			new ndp_data[DIALOG_MAX_LISTITEM_SIZE];
			VECTOR_get_str(NDP_DialogInfo[playerid], index, ndp_data, DIALOG_MAX_LISTITEM_SIZE);
			for(;;)
			{
				ntd_f = strfind(ndp_data, "\n", true);
				if(ntd_f == -1)
					break;
				strdel(ndp_data, ntd_f, ntd_f + 2);
			}
			VECTOR_set_str(NDP_DialogInfo[playerid], index, ndp_data);
		#else
			for(;;)
			{
				ntd_f = strfind(NDP_DialogInfo[playerid][index], "\n", true);
				if(ntd_f == -1)
					break;
				strdel(NDP_DialogInfo[playerid][index], ntd_f, ntd_f + 2);
			}
		#endif
	#endif
	// Counts the items that has been added.
	NDP_P[playerid][ndp_dialogItemsCount]++;
	// Logger for Debug purpose
	#if defined NDP_USE_LOGGER
		#if defined __VECTOR_INCLUDED__
		#if !defined ndp_data
			new ndp_data[DIALOG_MAX_LISTITEM_SIZE];
			VECTOR_get_str(NDP_DialogInfo[playerid], index, ndp_data, DIALOG_MAX_LISTITEM_SIZE);
		#endif
		Logger_Log("ndialog-pages",
			Logger_S("function", "AddDialogListitem"),
			Logger_I("playerid", playerid),
			Logger_I("index", index),
			Logger_S("data", ndp_data));
		#else
		Logger_Log("ndialog-pages",
			Logger_S("function", "AddDialogListitem"),
			Logger_I("playerid", playerid),
			Logger_I("index", index),
			Logger_S("data", NDP_DialogInfo[playerid][index]));
		#endif
	#endif
	return 1;
	#if !defined __VECTOR_INCLUDED__
	}
	return 0;
	#endif
}

/* Opens a Dialog with pages. Items added with the "AddDialogListitem" function. Ensure that the "items_per_page" variable is above 0! */
stock ShowPlayerDialogPages(playerid, const function[], style, const caption[], const button1[], const button2[], items_per_page, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<")
{
	return NDP_DialogInvoke(playerid, NDP_DIALOG_TYPE_DIALOG, function, 32701, style, caption, button1, button2, items_per_page, nextbutton, backbutton);
}

/* Same as ShowPlayerDialogPages but with inline support. */
#if YSI_VERSION_MAJOR == 5 && defined _INC_y_inline
	stock ShowInlineDialogPages(playerid, Func:cb<iis>, style, const caption[], const button1[], const button2[], items_per_page, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<")
	{
		if (ShowPlayerDialogPages(playerid, NDP_DialogName, style, caption, button1, button2, items_per_page, nextbutton, backbutton))
		{
			Indirect_Claim(cb);
			NDP_InlineCB = cb;
		}
		return 1;
	}
#endif

/************************************************************
                	INTERNAL FUNCTIONS
*************************************************************/

/* Prepares and Invokes/Opens the Dialog */
static stock NDP_DialogInvoke(playerid, type, const function[], dialogid, style, const caption[], const button1[], const button2[], items_per_page, const nextbutton[], const backbutton[])
{
	#if defined NDP_USE_LOGGER
	Logger_Log("ndialog-pages",
		Logger_S("function", "NDP_DialogInvoke Called."),
		Logger_I("playerid", playerid),
		Logger_I("type", type),
		Logger_S("function", function),
		Logger_I("dialogid", dialogid),
		Logger_S("caption", caption),
		Logger_S("button1", button1),
		Logger_S("button2", button2),
		Logger_I("items_per_page", items_per_page),
		Logger_S("nextbutton", nextbutton),
		Logger_S("backbutton", backbutton));
	#endif

	// Set ndp_dialogOpened to false in case if the dialog List couldn't be opened so it won't do anything after.
	NDP_P[playerid][ndp_dialogOpened] = false;

	// Checks if items_per_page is set above 0
	if(items_per_page > 0)
	{
		// Saving all data into the enumerator
		NDP_P[playerid][ndp_dialogid] = dialogid;
		NDP_P[playerid][ndp_style] = style;
		NDP_P[playerid][ndp_page] = 0;
		NDP_P[playerid][ndp_amountPerPage] = items_per_page;
		#if defined __VECTOR_INCLUDED__
			VECTOR_clear(NDP_P[playerid][ndp_endIndex]);
		#else
			NDP_P[playerid][ndp_endIndex] = 0;
		#endif
		NDP_P[playerid][ndp_type] = type;
		NDP_P[playerid][ndp_dialogName][0] = EOS;
		
		// Packs the function name into ndp_dialogName
		new bool: functionIsPacked = ispacked(function);
		if (functionIsPacked)
			strunpack(NDP_P[playerid][ndp_dialogName], function);
		else
			strcat(NDP_P[playerid][ndp_dialogName], function);
		
		// Saving all strings into the corresponding variables
		format(NDP_P[playerid][ndp_button1], 64, button1);
		format(NDP_P[playerid][ndp_button2], 64, button2);
		format(NDP_P[playerid][ndp_caption], 64, caption);
		format(NDP_P[playerid][ndp_nextButton], DIALOG_MAX_LISTITEM_SIZE, nextbutton);
		format(NDP_P[playerid][ndp_backButton], DIALOG_MAX_LISTITEM_SIZE, backbutton);
		
		// Calculating the pages
		NDP_CalculateListitemsPerPage(playerid);
		
		// Try to Show/Open the dialog
		NDP_P[playerid][ndp_dialogOpened] = true;
		return NDP_ShowDialogPage(playerid, NDP_P[playerid][ndp_page]);
	}
	ClearDialogListitems(playerid);
	return 0;
}

/* Calculates the amount of items per page in the Dialog.*/
static stock NDP_CalculateListitemsPerPage(playerid)
{
	new ndp_len, ndp_pagelist, ndp_counter;
	new npd_buttonslen = (strlen(NDP_P[playerid][ndp_nextButton]) + strlen(NDP_P[playerid][ndp_backButton]) + 4);

	#if defined NDP_USE_LOGGER
	Logger_Log("ndialog-pages",
		Logger_S("function", "NDP_CalculateListitemsPerPage Loop"),
		Logger_I("ndp_amountPerPage", NDP_P[playerid][ndp_amountPerPage]),
		Logger_I("ndp_dialogItemsCount", NDP_P[playerid][ndp_dialogItemsCount]),
		Logger_I("npd_buttonslen", npd_buttonslen));
	#endif

	for(new i = (NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS ? 1 : 0); i < NDP_P[playerid][ndp_dialogItemsCount]; i++)
	{
		#if defined __VECTOR_INCLUDED__
			new ndp_data[DIALOG_MAX_LISTITEM_SIZE];
			VECTOR_get_str(NDP_DialogInfo[playerid], (i < (NDP_P[playerid][ndp_dialogItemsCount] - 1)) ? (i + 1) : (i), ndp_data, DIALOG_MAX_LISTITEM_SIZE);
			ndp_len += (strlen(ndp_data) + 2);
		#else
			ndp_len += (strlen(NDP_DialogInfo[playerid][(i < (NDP_P[playerid][ndp_dialogItemsCount] - 1)) ? (i + 1) : (i)]) + 2);
		#endif
		if((ndp_counter >= NDP_P[playerid][ndp_amountPerPage]) || ((ndp_len + npd_buttonslen) >= (DIALOG_MAX_INFO_SIZE - DIALOG_STRING_PUFFER_SIZE)))
		{
			#if defined NDP_USE_LOGGER
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
		#if defined __VECTOR_INCLUDED__
			if(VECTOR_size(NDP_P[playerid][ndp_endIndex]) <= ndp_pagelist)
				VECTOR_resize(NDP_P[playerid][ndp_endIndex], ndp_pagelist+1);
			VECTOR_set_val(NDP_P[playerid][ndp_endIndex], ndp_pagelist, (i + 1));
		#else
			NDP_P[playerid][ndp_endIndex][ndp_pagelist] = (i + 1);
		#endif

		#if defined NDP_USE_LOGGER
			#if defined __VECTOR_INCLUDED__
			Logger_Log("ndialog-pages",
				Logger_S("function", "NDP_CalculateListitemsPerPage Loop"),
				Logger_I("ndp_pagelist", VECTOR_get_val(NDP_P[playerid][ndp_endIndex], ndp_pagelist)),
				Logger_I("iterator", i),
				Logger_I("ndp_counter", ndp_counter));
			#else
			Logger_Log("ndialog-pages",
				Logger_S("function", "NDP_CalculateListitemsPerPage Loop"),
				Logger_I("ndp_pagelist", NDP_P[playerid][ndp_endIndex][ndp_pagelist]),
				Logger_I("iterator", i),
				Logger_I("ndp_counter", ndp_counter));
			#endif
		#endif
	}
	#if defined __VECTOR_INCLUDED__
		new index = ndp_pagelist + (NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS ? 1 : 0);
		if(VECTOR_size(NDP_P[playerid][ndp_endIndex]) <= index)
			VECTOR_resize(NDP_P[playerid][ndp_endIndex], index+1);
		VECTOR_set_val(NDP_P[playerid][ndp_endIndex], index, NDP_P[playerid][ndp_dialogItemsCount]);
	#else
		NDP_P[playerid][ndp_endIndex][ndp_pagelist + (NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS ? 1 : 0)] = NDP_P[playerid][ndp_dialogItemsCount];
	#endif

	#if defined NDP_USE_LOGGER
		#if defined __VECTOR_INCLUDED__
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_CalculateListitemsPerPage After Loop"),
			Logger_I("ndp_pagelist", VECTOR_get_val(NDP_P[playerid][ndp_endIndex], ndp_pagelist)));
		#else
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_CalculateListitemsPerPage After Loop"),
			Logger_I("ndp_pagelist", NDP_P[playerid][ndp_endIndex][ndp_pagelist]));
		#endif
	#endif
	return 1;
}

/* Internal function, this will generate the dialog page. ! DON'T USE IT ! */
static stock NDP_ShowDialogPage(playerid, ndppage)
{
	// Getting the Start and End Listitem Index of the current page
	#if defined __VECTOR_INCLUDED__
		new startindex = (ndppage > 0) ? (VECTOR_get_val(NDP_P[playerid][ndp_endIndex], ndppage - 1)) : (0);
		new endindex = VECTOR_get_val(NDP_P[playerid][ndp_endIndex], ndppage);
	#else
		new startindex = (ndppage > 0) ? (NDP_P[playerid][ndp_endIndex][ndppage - 1]) : (0);
		new endindex = NDP_P[playerid][ndp_endIndex][ndppage];
	#endif

	#if defined NDP_USE_LOGGER
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

	#if defined __VECTOR_INCLUDED__
		new ndp_data[DIALOG_MAX_LISTITEM_SIZE];
	#endif
	// Generate the Header if needed
	if(NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS && NDP_P[playerid][ndp_itemsonpage] == 0)
	{
		#if defined __VECTOR_INCLUDED__
			VECTOR_get_str(NDP_DialogInfo[playerid], 0, ndp_data);
			strcat(NDP_DialogString, ndp_data);
		#else
			strcat(NDP_DialogString, NDP_DialogInfo[playerid][0]);
		#endif
		strcat(NDP_DialogString, "\n");

		#if defined NDP_USE_LOGGER
			#if defined __VECTOR_INCLUDED__
			Logger_Log("ndialog-pages",
				Logger_S("function", "NDP_ShowDialogPage"),
				Logger_I("index", 0),
				Logger_S("listitemadded", ndp_data));
			#else
			Logger_Log("ndialog-pages",
				Logger_S("function", "NDP_ShowDialogPage"),
				Logger_I("index", 0),
				Logger_S("listitemadded", NDP_DialogInfo[playerid][0]));
			#endif
		#endif
	}

	// Generate the list
	for(new i = (NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS && ndppage == 0 ? startindex + 1 : startindex) ; i < endindex; i++)
	{
		// Generate each listitem
		#if defined __VECTOR_INCLUDED__
			VECTOR_get_str(NDP_DialogInfo[playerid], i, ndp_data);
			strcat(NDP_DialogString, ndp_data);
		#else
			strcat(NDP_DialogString, NDP_DialogInfo[playerid][i]);
		#endif
		strcat(NDP_DialogString, "\n");

		#if defined NDP_USE_LOGGER
			#if defined __VECTOR_INCLUDED__
			Logger_Log("ndialog-pages",
				Logger_S("function", "NDP_ShowDialogPage"),
				Logger_I("index", i),
				Logger_S("listitemadded", ndp_data));
			#else
			Logger_Log("ndialog-pages",
				Logger_S("function", "NDP_ShowDialogPage"),
				Logger_I("index", i),
				Logger_S("listitemadded", NDP_DialogInfo[playerid][i]));
			#endif
		#endif

		// Keeping track on how much listitems are on the current page
		NDP_P[playerid][ndp_itemsonpage]++;
	}

	#if defined NDP_USE_LOGGER
	Logger_Log("ndialog-pages",
		Logger_S("function", "NDP_ShowDialogPage"),
		Logger_I("itemsonpage", NDP_P[playerid][ndp_itemsonpage]));
	#endif

	// Generate Button "Next"
	if(endindex != 0 && endindex < (NDP_P[playerid][ndp_dialogItemsCount]))
	{
		strcat(NDP_DialogString, "\n");
		strcat(NDP_DialogString, NDP_P[playerid][ndp_nextButton]);

		#if defined NDP_USE_LOGGER
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_ShowDialogPage"),
			Logger_S("button created", "next"));
		#endif
	}
	// Generate Button "Back"
	if(startindex != 0)
	{
		strcat(NDP_DialogString, "\n");
		strcat(NDP_DialogString, NDP_P[playerid][ndp_backButton]);

		#if defined NDP_USE_LOGGER
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_ShowDialogPage"),
			Logger_S("button created", "back"));
		#endif
	}
	// Show Dialog
	if(NDP_P[playerid][ndp_itemsonpage] > 0 || NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS)
	{
		#if defined NDP_USE_LOGGER
		Logger_Log("ndialog-pages",
			Logger_S("function", "NDP_ShowDialogPage"),
			Logger_I("showingdialogforplayer", playerid));
		#endif
		return ShowPlayerDialog(playerid, NDP_P[playerid][ndp_dialogid], NDP_P[playerid][ndp_style], NDP_P[playerid][ndp_caption], NDP_DialogString, NDP_P[playerid][ndp_button1], NDP_P[playerid][ndp_button2]);
	}
	return 0;
}

/* Internal function. This function processes the response of the dialog. ! DON'T USE IT ! */
static stock NDP_ProcessDialogResponse(playerid, dialogid, response, listitem, inputtext[], dialogtype)
{
	// This function is here in case there will be any new features in the future added.
	switch(dialogtype)
	{
		case NDP_DIALOG_TYPE_DIALOG: // Normal Dialog
		{
			dialogid = 32701;
			new ndp_d_str[32], ndp_d_dnu[32];

			// Sanitize inputs.
			for (new i = 0, l = strlen(inputtext); i < l; i ++)
			{
				if (inputtext[i] == '%')
				{
					inputtext[i] = '#';
				}
			}

			// Create the function string
			strcat(ndp_d_dnu, NDP_P[playerid][ndp_dialogName]);
			strcat(ndp_d_str, "ndpD_");
			strcat(ndp_d_str, ndp_d_dnu);

			// Logger for debugging
			#if defined NDP_USE_LOGGER
			Logger_Log("ndialog-pages",
				Logger_S("callback", "OnDialogResponse - type: NDP_DIALOG_TYPE_DIALOG"),
				Logger_I("funcidx(ndp_d_str)", funcidx(ndp_d_str)),
				Logger_I("listitem", listitem),
				Logger_S("ndp_d_str", ndp_d_str),
				Logger_S("ndp_dialogName", ndp_d_dnu));
			#endif

			if(listitem != INVALID_LISTITEM)
			{
				// Call the function
				CallLocalFunction(ndp_d_str, "ddds", playerid, response, listitem, inputtext);
				#if defined NDP_USE_LOGGER
				Logger_Log("ndialog-pages",
					Logger_S("callback", "OnDialogResponse - CallLocalFunction"));
				#endif
			}
		}
	}
	return dialogid;
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
	if(dialogid == NDP_P[playerid][ndp_dialogid] && NDP_P[playerid][ndp_dialogOpened])
	{
		new cur_page = NDP_P[playerid][ndp_page];
		#if defined __VECTOR_INCLUDED__
			new startindex = (cur_page > 0) ? (VECTOR_get_val(NDP_P[playerid][ndp_endIndex], cur_page - 1)) : (0);
		#else
			new startindex = (cur_page > 0) ? (NDP_P[playerid][ndp_endIndex][cur_page - 1]) : (0);
		#endif
		new ndp_tmplistitem = startindex + listitem;
		#if defined NDP_USE_LOGGER
		Logger_Log("ndialog-pages",
			Logger_S("callback", "OnDialogResponse"),
			Logger_I("ndp_dialogItemsCount", NDP_P[playerid][ndp_dialogItemsCount]),
			Logger_I("page", cur_page),
			Logger_I("startindex", startindex),
			Logger_I("ndp_tmplistitem", ndp_tmplistitem));
		#endif
		if(NDP_P[playerid][ndp_dialogItemsCount] > (NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS ? 1 : 0))
		{
			if(ndp_tmplistitem < NDP_P[playerid][ndp_dialogItemsCount])
			{
				//Button "Next"
				if(listitem == (NDP_P[playerid][ndp_itemsonpage])) 
				{
					#if defined NDP_USE_LOGGER
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
					#if defined NDP_USE_LOGGER
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
					listitem = ndp_tmplistitem - ((NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS && cur_page > 0) ? (1) : (0)); 
					#if defined NDP_USE_LOGGER
					Logger_Log("ndialog-pages",
						Logger_S("callback", "OnDialogResponse"),
						Logger_S("button clicked", "item"),
						Logger_I("listitem", listitem),
						Logger_I("ndp_type", NDP_P[playerid][ndp_type]),
						Logger_S("ndp_dialogName", NDP_P[playerid][ndp_dialogName]));
					#endif
				}
			}
			else if(NDP_P[playerid][ndp_dialogItemsCount] > 0)
			{
				//Also button "Back", but on the last page
				#if defined NDP_USE_LOGGER
				Logger_Log("ndialog-pages",
					Logger_S("callback", "OnDialogResponse"),
					Logger_S("button clicked", "back - last page"));
				#endif
				NDP_P[playerid][ndp_page]--;
				if(response) NDP_ShowDialogPage(playerid, NDP_P[playerid][ndp_page]);
				listitem = INVALID_LISTITEM;
			}
		}
		else 
		{
			listitem = INVALID_LISTITEM;
		}
		dialogid = NDP_ProcessDialogResponse(playerid, dialogid, response, listitem, inputtext, NDP_P[playerid][ndp_type]);
	}
	return 0;
}

#if YSI_VERSION_MAJOR == 5 && defined _INC_y_inline
	DialogPages:Inline_DialogPages(playerid, response, listitem, string:inputtext[])
	{
		new Func:cb<iis> = NDP_InlineCB;
		NDP_InlineCB = Func:0<iis>;

		@.cb(response, listitem, inputtext);

		Indirect_Release(cb);

		return 1;
	}
#endif

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
