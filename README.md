# Nickk's Dialog - Pages
![Preview1](/preview1.png)

NDialog-Pages is basically an addon that adds 3 additional options to the "`ShowPlayerDialog`" function.
It will add a feature so that you can split the dialog list into multiple pages!

It will basically calculate how many items will fit into one page and generate the "`Next`" button
if there are too many. It will also add a "`Back`" button if you go to the second page etc.
It will calculate the "`listitem`" in such way, like a normal list would work, so you don't need to
make some calculation yourself, everything is automatic and it will return the correct "`listitem`".

It is VERY easy to use, because everything is fully automatic.

### Functions
* `AddDialogListitem(playerid, const itemstring[]);`:
  * Adds an item to the dialog.
* `ClearDialogListitems(playerid);`:
  * Resets the lister for the dialog.
* `ShowPlayerDialog(playerid, dialogid, style, caption[], #, button1[], button2[], items_per_page = 0, nextbutton[] = "{FF0000}>>>", backbutton[] = "{FF0000}<<<");`:
  * Shows the dialog.

### How to Use
On top of your script put this:
```pawn
#include <ndialog-pages>
```
Before showing the dialog for the player, you need to add the items into the dialog using the "`AddDialogListitem`" function without "`\n`"!
After you added all the items (Using a loop or manually) you need to use the "`ShowPlayerDialog`" function.
The "`info[]`" parameter will not be used so you can just put an empty string or a "`#`" inside it.
To make the dialog split the items into multiple pages, you will need to first set the "`items_per_page`" parameter > 0.
"`items_per_page`" will tell the dialog, how many items will fit into ONE page!
Set it to 10, it will show you 10 items plus the "`Next`" and "`Back`" button.
You can also modify the buttons yourself, just set the "nextbutton" and or "backbutton" parameter, but that's optional.
Also check if "listitem" is NOT "`INVALID_LISTITEM`", it will be invalid if the player will press the "`Next`" or "`Back`" button, so please negate it!

After the dialog has been showed to the player, using the "`AddDialogListitem`" function will completely reset the items and it will add the new item from index 0 >
You can also FORCE reseting the dialog lister using the "`ClearDialogListitems`" function if you need to.

You can change the amount of max items in your dialog by redefining the "`MAX_DIALOG_ITEMS`" definition.
