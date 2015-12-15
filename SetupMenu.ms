
-- macro script to toggle Fabric rendering
macroScript FabricRendering 
category:"FabricEngine"
internalCategory:"FabricEngine"
tooltip:"Enable Fabric Viewport Rendering"
buttontext:"Viewport Render"
autoUndoEnabled:false
(
    on execute do Fabric.Rendering = not Fabric.Rendering
    on isChecked return Fabric.Rendering
)

-- macro script to toggle Fabric manipulation
macroScript FabricManipulation
category:"FabricEngine"
internalCategory:"FabricEngine"
tooltip:"Enable Fabric Manipulation"
buttontext:"Manipulation"
autoUndoEnabled:false
(
    on execute do Fabric.Manipulation = not Fabric.Manipulation
    on isChecked return Fabric.Manipulation
)

-- macro script to load a Fabric preset
macroScript LoadFabricPreset
category:"FabricEngine"
internalCategory:"FabricEngine"
tooltip:"Load a Fabric Canvas Preset File"
buttontext:"Load Preset"
autoUndoEnabled:false
(
  f = getOpenFileName caption:"Select a canvas Preset File:" types:"Fabric Canvas Preset(*.canvas)|*.canvas"
  if f != undefined then
  (
    Fabric.LoadFromFile f
  )
)

-- macro script to open the help for Fabric. 
macroScript FabricHelp 
category:"FabricEngine" 
internalCategory:"FabricEngine"
autoUndoEnabled:false
(
  shellLaunch "http://docs.fabric-engine.com/FabricEngine/latest/HTML/" ""
)
__FE_MENU_NAME = "Fabric"

fn findFabricEngineMenuItem = (
  local mainMenuBar = menuMan.getMainMenuBar()
  res = false
  for i = 1 to mainMenuBar.numItems() do (
    item = mainMenuBar.getItem i
    if (item.getTitle() == __FE_MENU_NAME) do ( res = item )
  )
  res
)

-- Add Fabric commadns to the menu bar if they don't already exist
fn RegisterFabricEngineMenu = (
  -- This example adds a new sub-menu to MAX's main menu bar. -- It adds the menu just before the "Help" menu.
  if findFabricEngineMenuItem() == false do
  (
    -- Get the main menu bar
    local mainMenuBar = menuMan.getMainMenuBar()
    -- Create a new menu
    local FabricMenu = menuMan.createMenu __FE_MENU_NAME

    local toggleRenderingAction = menuMan.createActionItem "FabricRendering" "FabricEngine"
    --add the ActionItem to the menu
    FabricMenu.addItem toggleRenderingAction -1

    local toggleManipulationAction = menuMan.createActionItem "FabricManipulation" "FabricEngine"
    --add the ActionItem to the menu
    FabricMenu.addItem toggleManipulationAction -1

    local sepItem = menuMan.createSeparatorItem()
    FabricMenu.addItem sepItem -1

    local loadFabricPresetAction = menuMan.createActionItem "LoadFabricPreset" "FabricEngine"
    --add the ActionItem to the menu
    FabricMenu.addItem loadFabricPresetAction -1

    local sepItem = menuMan.createSeparatorItem()
    FabricMenu.addItem sepItem -1

    local FabricHelpAction = menuMan.createActionItem "FabricHelp" "FabricEngine"
    --add the ActionItem to the menu
    FabricMenu.addItem FabricHelpAction -1


    -- Create a new menu item with the menu as it's sub-menu
    local FabricSubMenuItem = menuMan.createSubMenuItem __FE_MENU_NAME FabricMenu
    -- compute the index of the next-to-last menu item in the main menu bar
    local subMenuIndex = mainMenuBar.numItems() - 1
    -- Add the sub-menu just at the second to last slot
    mainMenuBar.addItem FabricSubMenuItem subMenuIndex
    
    -- redraw the menu bar with the new item
    menuMan.updateMenuBar()
  )
)

fn DeRegisterFabricEngineMenu = (
  local FabricMenuItem = findFabricEngineMenuItem()
  if (FabricMenuItem != false) do (
    -- Get the main menu bar
    local mainMenuBar = menuMan.getMainMenuBar()
    -- Remove menu item
    mainMenuBar.removeItem FabricMenuItem
    -- Create a new menu
    local FabricMenu = menuMan.findMenu __FE_MENU_NAME
    -- Remove this menu
    if (FabricMenu != undefined) do ( menuMan.unRegisterMenu FabricMenu )
    menuMan.updateMenuBar()
  )
  -- Force the release of any remaining Fabric happenings
  Fabric.DestroyClient force:true
)

-- When this script executes, we automatically add our menu
-- to max if it doesn't already exist
RegisterFabricEngineMenu()

-- Register a on-shutdown event to remove our menu's.
-- We don't want them appearing in max when this
-- script hasn't been executed
callbacks.addScript #preSystemShutdown "DeRegisterFabricMenu()" id:#FabricRemoveMenu

-- Our default initial state is Fabric cleaned up and rendering on
fn InitFabricToDefault = (
	Fabric.DestroyClient force:true
	Fabric.Rendering = true
)
callbacks.addScript #systemPostReset "InitFabricToDefault()" id:#FabricDefaultVptRenderOn1
callbacks.addScript #systemPostNew "InitFabricToDefault()" id:#FabricDefaultVptRenderOn2
callbacks.addScript #postSystemStartup "InitFabricToDefault()" id:#FabricDefaultVptRenderOn