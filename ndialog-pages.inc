/**********************************************************************************************************************************
!!!!! Please don't change the authors name! Every script is a rarity! So please be so kind and DON'T remove this license! !!!!!
**********************************************************************************************************************************

Script: NDialog-Pages
Type: Include
Version: 2.0
Author: Nickk888
Date: 03.06.2020

Social Media:
YouTube: https://www.youtube.com/channel/UCvjbpjJXET1QS9Wa0_XVCuw
Facebook: https://www.facebook.com/Nickk888FP/

___Description___
NDialog-Pages is basically an addon that adds 3 additional options to the "ShowPlayerDialog" function.
It will add a feature so that you can split the dialog list into multiple pages!

It will basically calculate how many items will fit into one page and generate the "Next" button
if there are too many. It will also add a "Back" button if you go to the second page etc.
It will calculate the "listitem" in such way, like a normal list would work, so you don't need to
make some calculation yourself, everything is automatic and it will return the correct "listitem".

It is VERY easy to use, because everything is fully automatic.

___Functions___
AddDialogListitem(playerid, const itemstring[]);
ClearDialogListitems(playerid);
ShowPlayerDialog(playerid, dialogid, style, caption[], #, button1[], button2[], items_per_page = 0, nextbutton[] = "{FF0000}>>>", backbutton[] = "{FF0000}<<<");

___How to Use___
Before showing the dialog for the player, you need to add the items into the dialog using the "AddDialogListitem" function without "\n"!
After you added all the items (Using a loop or manually) you need to use the "ShowPlayerDialog" function.
The "info" parameter will not be used so you can just put an empty string or a # inside it.
To make the dialog split the items into multiple pages, you will need to first set the "items_per_page" parameter > 0.
"items_per_page" will tell the dialog, how many items will fit into ONE page!
Set it to 10, it will show you 10 items plus the "Next" and "Back" button.
You can also modify the buttons yourself, just set the "nextbutton" and or "backbutton" parameter, but that's optional.
Also check if "listitem" is NOT "INVALID_LISTITEM", it will be invalid if the player will press the "Next" or "Back" button, so please negate it!

After the dialog has been showed to the player, using the "AddDialogListitem" function will completely reset the items and it will add the new item from index 0 >
You can also FORCE reseting the dialog lister using the "ClearDialogListitems" function if you need to.

**********************************************************************************************************************************
!!!!! Please don't change the authors name! Every script is a rarity! So please be so kind and DON'T remove this license! !!!!!
**********************************************************************************************************************************/

#include <YSI_Coding\y_hooks> //By Y_Less | Thank you, great include <3

//Defines
#if !defined MAX_DIALOG_ITEMS
	#define MAX_DIALOG_ITEMS 		1000 //Max dialog items, you can change this if you want.
#endif

//Internals
#define DIALOG_MAX_LISTITEM_SIZE 	256 //The cell size of one listitem, Don't change this!
#define DIALOG_MAX_INFO_SIZE 		4096 //The cell size of the whole dialog, Don't change this!
#define DIALOG_STRING_PUFFER_SIZE	132 //The margin for the dialog len size so it won't cut off the string and bug everything, Don't change this!
#define INVALID_LISTITEM 			(0xFFFF) //The definition for an Invalid Listitem, don't change this if you don't have to, don't go above -1
#define NDP_AUTO					(MAX_DIALOG_ITEMS) //Be so kind and don't change this :D

//Enumerators
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

//This stores all the items for the dialog
new NDP_DialogInfo[MAX_PLAYERS][MAX_DIALOG_ITEMS][DIALOG_MAX_LISTITEM_SIZE];
//This is used as a temporary item string.
new NDP_DialogString[DIALOG_MAX_INFO_SIZE];

//Functions/Stocks
stock ClearDialogListitems(playerid)
{
	NDP_P[playerid][ndp_dialogopened] = false;
	NDP_P[playerid][ndp_dialogitemscount] = 0;
	return 1;
}

//Adds Listitem to dialog
stock AddDialogListitem(playerid, const itemstring[])
{
	//Try's to reset the items if a dialog has been already opened.
	if(NDP_P[playerid][ndp_dialogopened])
		ClearDialogListitems(playerid);
	//Checks if there is still room to add an item into memory.
	if(NDP_P[playerid][ndp_dialogitemscount] < MAX_DIALOG_ITEMS)
	{
		new index = NDP_P[playerid][ndp_dialogitemscount];
		format(NDP_DialogInfo[playerid][index], DIALOG_MAX_LISTITEM_SIZE, itemstring);
		//Checks if there is any "\n" in the itemstring and deletes it because it won't be needed and it can cause problems...
		new ntd_f;
		for(;;)
		{
			ntd_f = strfind(NDP_DialogInfo[playerid][index], "\n", true);
			if(ntd_f == -1)
				break;
			strdel(NDP_DialogInfo[playerid][index], ntd_f, ntd_f + 2);
		}
		//Counts the items that has been added.
		NDP_P[playerid][ndp_dialogitemscount]++;
		return 1;
	}
	return 0;
}

forward ndialog_pages_ShowPlayerDialog(playerid, dialogid, style, const caption[], const info[], const button1[], const button2[], items_per_page = 0, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<");
stock ndialog_pages_ShowPlayerDialog(playerid, dialogid, style, const caption[], const info[], const button1[], const button2[], items_per_page = 0, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<")
{
	NDP_P[playerid][ndp_isstylepage] = false;
	if(items_per_page > 0)
	{
		if(style == DIALOG_STYLE_INPUT || style == DIALOG_STYLE_PASSWORD ||style == DIALOG_STYLE_MSGBOX) 
		{
			print("[ndialog-pages] Dialog style is invalid, pages will only work properly on lists.");
			return ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2); //Default
		}

		//Saving all data into the enumerator
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
		
		//Calculating the pages
		NDP_CalculateListitemsPerPage(playerid);
		//Show
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
	for(new i = ((NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS) ? (1) : (0)); i < NDP_P[playerid][ndp_dialogitemscount]; i++)
	{
		ndp_len += (strlen(NDP_DialogInfo[playerid][(i < (NDP_P[playerid][ndp_dialogitemscount] - 1)) ? (i + 1) : (i)]) + 2);
		if((ndp_counter > NDP_P[playerid][ndp_amountperpage]) || ((ndp_len + npd_buttonslen) >= (DIALOG_MAX_INFO_SIZE - DIALOG_STRING_PUFFER_SIZE)))
			ndp_pagelist++, 
			ndp_len = 0, 
			ndp_counter = 0,
			i--;
		ndp_counter++;
		NDP_P[playerid][ndp_endindex][ndp_pagelist] = i;
		
	}
	NDP_P[playerid][ndp_endindex][ndp_pagelist] = NDP_P[playerid][ndp_dialogitemscount];
	return 1;
}

//Another internal function, this will generate the dialog page.
stock NDP_ShowDialogPage(playerid, ndppage)
{
	//Getting the Start and End Listitem Index of the current page
	new startindex = (ndppage > 0) ? (NDP_P[playerid][ndp_endindex][ndppage - 1]) : (0);
	new endindex = NDP_P[playerid][ndp_endindex][ndppage];
	//Setting the values
	NDP_DialogString = "";
	NDP_P[playerid][ndp_itemsonpage] = 0;
	NDP_P[playerid][ndp_page] = ndppage;
	//Generate the list
	for(new i = startindex; i < endindex; i++)
	{
		//Generate the Header if needed
		if(NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS && NDP_P[playerid][ndp_itemsonpage] == 0)
		{
			strcat(NDP_DialogString, NDP_DialogInfo[playerid][0]);
			strcat(NDP_DialogString, "\n");
			if(i == 0)
				i++;
		}
		//Generate each listitem
		if(NDP_P[playerid][ndp_itemsonpage] != 0) 
			strcat(NDP_DialogString, "\n");
		strcat(NDP_DialogString, NDP_DialogInfo[playerid][i]);
		//Keeping track on how much listitems are on the current page
		NDP_P[playerid][ndp_itemsonpage]++;
	}
	//Generate Button "Next"
	if(endindex < (NDP_P[playerid][ndp_dialogitemscount]))
	{
		strcat(NDP_DialogString, "\n");
		strcat(NDP_DialogString, NDP_P[playerid][ndp_nextbutton]);
	}
	//Generate Button "Back"
	if(startindex != 0)
	{
		strcat(NDP_DialogString, "\n");
		strcat(NDP_DialogString, NDP_P[playerid][ndp_backbutton]);
	}
	//Show Dialog
	if(NDP_P[playerid][ndp_itemsonpage] > 0)
	{
		ShowPlayerDialog(playerid, NDP_P[playerid][ndp_dialogid], NDP_P[playerid][ndp_style], NDP_P[playerid][ndp_caption], NDP_DialogString, NDP_P[playerid][ndp_button1], NDP_P[playerid][ndp_button2]);
		return 1;
	}
	return 0;
}

//Callback hooking
hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if(dialogid == NDP_P[playerid][ndp_dialogid] && NDP_P[playerid][ndp_isstylepage] && NDP_P[playerid][ndp_dialogopened])
	{
		new page = NDP_P[playerid][ndp_page];
		new startindex = (page > 0) ? (NDP_P[playerid][ndp_endindex][page - 1]) : (0);
		new ndp_tmplistitem = startindex + listitem;
		if(ndp_tmplistitem < NDP_P[playerid][ndp_dialogitemscount])
		{
			//Button "Next"
			if(listitem == (NDP_P[playerid][ndp_itemsonpage])) 
			{
				NDP_P[playerid][ndp_page]++;
				if(response) NDP_ShowDialogPage(playerid, NDP_P[playerid][ndp_page]);
				listitem = INVALID_LISTITEM;
			}
			//Button "Back"
			else if(listitem == (NDP_P[playerid][ndp_itemsonpage] + 1)) 
			{
				NDP_P[playerid][ndp_page]--;
				if(response) NDP_ShowDialogPage(playerid, NDP_P[playerid][ndp_page]);
				listitem = INVALID_LISTITEM;
			}
			//Normal item
			else listitem = ndp_tmplistitem - ((NDP_P[playerid][ndp_style] == DIALOG_STYLE_TABLIST_HEADERS && page > 0) ? (1) : (0)); 
		}
		else
		{
			//Also button "Back", but on the last page
			NDP_P[playerid][ndp_page]--;
			if(response) NDP_ShowDialogPage(playerid, NDP_P[playerid][ndp_page]);
			listitem = INVALID_LISTITEM;
		}
	}
	return 0;
}

// ALS Hooking
#if defined _ALS_ShowPlayerDialog
    #undef ShowPlayerDialog
#else
    #define _ALS_ShowPlayerDialog
#endif
#define ShowPlayerDialog ndialog_pages_ShowPlayerDialog

//
