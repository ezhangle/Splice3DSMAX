
-- macro script to toggle splice rendering
macroScript SpliceRendering category:"Fabric Splice" autoUndoEnabled:false
(
	Splice.Rendering = not Splice.Rendering
)

-- macro script to toggle splice manipulation
macroScript SpliceManipulation category:"Fabric Splice" autoUndoEnabled:false
(
	Splice.Manipulation = not Splice.Manipulation
)

-- macro script to open the help for Splice. 
macroScript SpliceHelp category:"Fabric Splice" autoUndoEnabled:false
(
	shellLaunch "http://documentation.fabricengine.com/Splice/latest/HTML/" ""
)


-- This example adds a new sub-menu to MAX's main menu bar. -- It adds the menu just before the "Help" menu.
if menuMan.registerMenuContext 0x4f7067c0 then
(
	-- Get the main menu bar
	local mainMenuBar = menuMan.getMainMenuBar()
	-- Create a new menu
	local spliceMenu = menuMan.createMenu "Splice"

	local toggleRenderingAction = menuMan.createActionItem "SpliceRendering" "Fabric Splice"
	--add the ActionItem to the menu
	spliceMenu.addItem toggleRenderingAction -1

	local toggleManipulationAction = menuMan.createActionItem "SpliceManipulation" "Fabric Splice"
	--add the ActionItem to the menu
	spliceMenu.addItem toggleManipulationAction -1

	local sepItem = menuMan.createSeparatorItem()
	spliceMenu.addItem sepItem -1

	local spliceHelpAction = menuMan.createActionItem "SpliceHelp" "Fabric Splice"
	--add the ActionItem to the menu
	spliceMenu.addItem spliceHelpAction -1


	-- Create a new menu item with the menu as it's sub-menu
	local spliceSubMenuItem = menuMan.createSubMenuItem "Splice" spliceMenu
	-- compute the index of the next-to-last menu item in the main menu bar
	local subMenuIndex = mainMenuBar.numItems() - 1
	-- Add the sub-menu just at the second to last slot
	mainMenuBar.addItem spliceSubMenuItem subMenuIndex
	
	-- redraw the menu bar with the new item
	menuMan.updateMenuBar()
)