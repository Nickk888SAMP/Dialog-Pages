# Nickk's Dialog Pages
<img src="/preview.gif" width="400" height="400">

Nickk's Dialog Pages adds the possibility to created paged dialog lists.

It will basically calculate how many items will fit into one page and generate the `Next` button
if there are too many. It will also add a `Back` button if you go to the second page etc.
It will calculate the `listitem`"in such way, like a normal list would work, so you don't need to
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

Shows the dialog (Legacy, Deprecated)
```pawn
ShowPlayerDialog(playerid, dialogid, style, const caption[], #, const button1[], const button2[], items_per_page = 0, const nextbutton[] = "{FF0000}>>>", const backbutton[] = "{FF0000}<<<");
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

```pawn
DialogPages:DialogName(playerid, response, listitem)
{
	new dstr[128];
	format(dstr, sizeof dstr, "{FFFF00}[NDialog-Pages] {FFFFFF}You have selected listitem ID: {FFFF00}%i", listitem);
	SendClientMessage(playerid, -1, ndp_e_str);
	return 1;
}
```

## Example
[Example Script](https://github.com/Nickk888SAMP/Dialog-Pages/blob/master/ndp_examples.pwn)
