
-- macro script to toggle splice rendering
macroScript SpliceRendering 
category:"Fabric Splice"
internalCategory:"FabricSplice"
tooltip:"Enable Splice Viewport Rendering"
buttontext:"Viewport Render"
autoUndoEnabled:false
(
    on execute do  Splice.Rendering = not Splice.Rendering
    on isChecked return Splice.Rendering
)

-- macro script to toggle splice manipulation
macroScript SpliceManipulation
category:"Fabric Splice"
internalCategory:"FabricSplice"
tooltip:"Enable Splice Manipulation"
buttontext:"Manipulation"
autoUndoEnabled:false
(
    on execute do  Splice.Manipulation = not Splice.Manipulation
    on isChecked return Splice.Manipulation
)

-- macro script to load a splice preset
macroScript LoadSplicePreset
category:"Fabric Splice"
internalCategory:"FabricSplice"
tooltip:"Load a Splice Preset File"
buttontext:"Load Splice Preset"
autoUndoEnabled:false
(
  f = getOpenFileName caption:"Select a Splice Preset File:" types:"Splice Preset(*.splice)|*.splice"
  if f != undefined then
  (
    Splice.LoadFromFile f
  )
)

-- macro script to open the help for Splice. 
macroScript SpliceHelp 
category:"Fabric Splice" 
internalCategory:"FabricSplice"
autoUndoEnabled:false
(
  shellLaunch "http://documentation.fabric-engine.com/FabricEngine/latest/HTML/Splice/" ""
)
__FE_MENU_NAME = "Splice"

fn findFabricSpliceMenuItem = (
  local mainMenuBar = menuMan.getMainMenuBar()
  res = false
  for i = 1 to mainMenuBar.numItems() do (
    item = mainMenuBar.getItem i
    if (item.getTitle() == __FE_MENU_NAME) do ( res = item )
  )
  res
)

-- Add Splice commadns to the menu bar if they don't already exist
fn RegisterFabricSpliceMenu = (
  -- This example adds a new sub-menu to MAX's main menu bar. -- It adds the menu just before the "Help" menu.
  if findFabricSpliceMenuItem() == false do
  (
    -- Get the main menu bar
    local mainMenuBar = menuMan.getMainMenuBar()
    -- Create a new menu
    local spliceMenu = menuMan.createMenu __FE_MENU_NAME

    local toggleRenderingAction = menuMan.createActionItem "SpliceRendering" "FabricSplice"
    --add the ActionItem to the menu
    spliceMenu.addItem toggleRenderingAction -1

    local toggleManipulationAction = menuMan.createActionItem "SpliceManipulation" "FabricSplice"
    --add the ActionItem to the menu
    spliceMenu.addItem toggleManipulationAction -1

    local sepItem = menuMan.createSeparatorItem()
    spliceMenu.addItem sepItem -1

    local loadSplicePresetAction = menuMan.createActionItem "LoadSplicePreset" "FabricSplice"
    --add the ActionItem to the menu
    spliceMenu.addItem loadSplicePresetAction -1

    local sepItem = menuMan.createSeparatorItem()
    spliceMenu.addItem sepItem -1

    local spliceHelpAction = menuMan.createActionItem "SpliceHelp" "FabricSplice"
    --add the ActionItem to the menu
    spliceMenu.addItem spliceHelpAction -1


    -- Create a new menu item with the menu as it's sub-menu
    local spliceSubMenuItem = menuMan.createSubMenuItem __FE_MENU_NAME spliceMenu
    -- compute the index of the next-to-last menu item in the main menu bar
    local subMenuIndex = mainMenuBar.numItems() - 1
    -- Add the sub-menu just at the second to last slot
    mainMenuBar.addItem spliceSubMenuItem subMenuIndex
    
    -- redraw the menu bar with the new item
    menuMan.updateMenuBar()
  )
)

fn DeRegisterFabricSpliceMenu = (
  local spliceMenuItem = findFabricSpliceMenuItem()
  if (spliceMenuItem != false) do (
    -- Get the main menu bar
    local mainMenuBar = menuMan.getMainMenuBar()
    -- Remove menu item
    mainMenuBar.removeItem spliceMenuItem
    -- Create a new menu
    local spliceMenu = menuMan.findMenu __FE_MENU_NAME
    -- Remove this menu
    if (spliceMenu != undefined) do ( menuMan.unRegisterMenu spliceMenu )
    menuMan.updateMenuBar()
  )
)

-- When this script executes, we automatically add our menu
-- to max if it doesn't already exist
RegisterFabricSpliceMenu()

-- Register a on-shutdown event to remove our menu's.
-- We don't want them appearing in max when this
-- script hasn't been executed
callbacks.addScript #preSystemShutdown "DeRegisterSpliceMenu()" id:#SpliceRemoveMenu
