# Nickk's Dialog Pages
<img src="/preview.gif" width="400" height="400">

Nickk's Dialog Pages adds the possibility to create paged dialog lists.

It will basically calculate how many items will fit into one page and generate the `Next` button
if there are too many. It will also add a `Back` button if you go to the second page etc.
It will calculate the `listitem` in such way, like a normal list would work, so you don't need to
make some calculation yourself, everything is automatic and it will return the correct `listitem`.

It is very easy to use, because everything is fully automatic.

## Installation

Simply install to your project:

```bash
sampctl package install Nickk888SAMP/Dialog-Pages
```

Include in your code and begin using the library:

```pawn
#include <ndialog-pages>
```

## Functions
Adds an item to the dialog.
```pawn
AddDialogListitem(playerid, const itemstring[]);
```
Adds an item to the dialog with arguments (requires y_va include to work), not necessary to format string.
```pawn
AddDialogListitem(playerid, const itemstring[], va_args<>);
```
Resets the lister for the dialog.
```pawn
ClearDialogListitems(playerid);
```
Shows the dialog
```pawn
ShowPlayerDialogPages(playerid, const function[], style, const caption[], const button1[], const button2[], items_per_page, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<");
```

## How to Use
To add items to a paged dialog, use the function "```AddDialogListitem```".
* ```playerid``` - The player you want to add items to the dialog.
* ```itemstring[]``` - The item as a string without ```\n```

To show the paged dialog, use function "```ShowPlayerDialogPages```".
* ```playerid``` - The player you want to show the dialog.
* ```function[]``` - The name of the Dialog, must be the same as defined with "DialogPages:".
* ```style``` - The style of the dialog.
* ```caption[]``` - The caption of the dialog.
* ```button1[]``` - The first button of the dialog.
* ```button2[]``` - The second button of the dialog
* ```items_per_page``` - The amount of items showed on a single page.
* ```nextbutton[]``` - The "Next" button string. (Optional)
* ```backbutton[]``` - The "Back" button string. (Optional)

If you want to clear the ```Items Cache```, you can always use ```ClearDialogListitems(playerid)```, it's optional because it's always cleared when the first item has been added after a paged dialog has been showed to the player.

To get the response of the Dialog, create a new callback using "```DialogPages:Name_Of_The_Dialog(playerid, response, listitem, inputtext[])```".
* ```playerid``` - The player who responded to the Dialog.
* ```response``` - Did the player clicked Button1 or Button2.
* ```listitem``` - The selected listitem of the dialog.
* ```inputtext[]``` - The selected listitem's text as a string.

## Example
```pawn
CMD:ndptest(playerid) // Needs ZCMD
{
	new string[128];
	for(new i; i < 250; i++)
	{
		format(string, sizeof string, "{FFFFFF}List Item {FF00FF}%i", i);
		AddDialogListitem(playerid, string);
	}
	ShowPlayerDialogPages(playerid, "MyPagedDialog", DIALOG_STYLE_LIST, "{FFFFFF}Test Dialog Name {FF00FF}DialogName", "Button 1", "Button 2", 15);
	return 1;
}

DialogPages:MyPagedDialog(playerid, response, listitem, inputtext[])
{
	if(!response) return 1;
		
	new dstr[128];
	format(dstr, sizeof dstr, "[NDialog-Pages] You have selected listitem ID: {666666}%i{FFFFFF}, listitem's text: {666666}%s", listitem, inputtext);
	SendClientMessage(playerid, -1, dstr);
	return 1;
}
```

[Example Script](https://github.com/Nickk888SAMP/Dialog-Pages/blob/master/ndp_examples.pwn)
