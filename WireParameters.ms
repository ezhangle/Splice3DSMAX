
macroscript Popup_Menu category:"denisT" buttonText:"DRC" tooltip:"Popup RC"
(
	local form = dotnetobject "MaxCustomControls.Maxform"
	form.ShowInTaskbar = off
	form.FormBorderStyle = form.FormBorderStyle.None
	form.AllowTransparency = on
	form.Opacity = 0
	
	form.StartPosition = form.StartPosition.Manual
	form.Size = Form.MinimumSize
	form.Location = dotnetobject "System.Drawing.Point" -100 -100

	local sourcePortName = ""
	local sourcePortFabricEntity = undefined

	fn SearchForFabricClasses obj cm doFn_ = (
		res = cm
		if (getinterface obj "FabricInterface" != undefined) do (
			res = doFn_ obj res
		)
		for i = 1 to obj.numsubs do (
			SearchForFabricClasses obj[i] res doFn_
		)
		res
	)
	
	classList = #()
	fn GatherFabricClasses obj cm = ( append classList obj )
	fn targetFabEntityFilter o = 
	(
		classList = #()
		SearchForFabricClasses o false GatherFabricClasses
		res = classList.count > 0
	)
	
	fn onFabOutPortClicked s e =
	(
		sourcePortName = s.Text
		sourcePortFabricEntity = s.tag.value
		
		-- pick our destination object
		res = pickObject prompt:"Pick the target Fabric Entity" filter:targetFabEntityFilter
		if res != undefined do (
			contextMenu = BuildMenuItems #(res) AddFabricInPortsAsString
			ShowContextMenu contextMenu
		)
		form.Hide()
	)
	
	fn onFabInPortClicked s e =
	(
		local destPortName = s.Text
		local destPortFabEntity = s.tag.value
		
		-- Assume we have already picked our first port
		res = destPortFabEntity.ConnectArgs destPortName sourcePortFabricEntity sourcePortName postUI:true
		if not res do ( print ("ERROR: Could not connect src: " + sourcePortName + " -> dst: " + destPortName ) )
	)
	fn onCollapse s e = 
	(
		form.Hide()
	)
	
	fn AddFabricInPortsAsString obj cm = (
		cm = cm.menuitems.Add obj.name
		-- Add all out ports to 
		for i = 1 to obj.GetPortCount() do (
			portName = obj.GetPortName i
			if (obj.getPortType portName != #DFGPortOut) do (
				portMenuItem = cm.menuitems.Add portName
				portMenuItem.tag = dotNetMXSValue obj
				dotnet.addEventHandler portMenuItem "Click" onFabInPortClicked
			)
		)
	)
	
	fn AddFabricOutportsAsString obj cm = (
		cm = cm.menuitems.Add obj.name
		-- Add all out ports to 
		for i = 1 to obj.GetPortCount() do (
			portName = obj.GetPortName i
			if (obj.getPortType portName != #DFGPortIn) do (
				portMenuItem = cm.menuitems.Add portName
				portMenuItem.tag = dotNetMXSValue obj
				dotnet.addEventHandler portMenuItem "Click" onFabOutPortClicked
			)
		)
	)
	

	
	fn BuildMenuItems entities doFn_ =
	(
		local contextMenu = dotNetObject "System.Windows.Forms.ContextMenu"
		contextMenu.MenuItems.Clear()
		dotnet.addEventHandler contextMenu "Collapse" onCollapse
		
		for obj in entities do (
			objMenuItem = contextMenu.menuitems.Add obj.name
			SearchForFabricClasses obj objMenuItem doFn_
		)

		--form.ContextMenu = contextMenu
		contextMenu
	)
	
	fn ShowContextMenu contextMenu =
	(
			-- Show context menu under cursor
			cursorPos = (dotNetClass "System.Windows.Forms.Cursor").Position
			cursorPos.X -= (form.Left + 10)
			cursorPos.Y -= (form.Top + 15)
			contextMenu.Show form cursorPos
	)

	fn onShown s e =
	(
		-- Build context menu
		contextMenu = BuildMenuItems()
		-- Show context menu under cursor
		cursorPos = (dotNetClass "System.Windows.Forms.Cursor").Position
		cursorPos.X -= (s.Left + 10)
		cursorPos.Y -= (s.Top + 15)
		contextMenu.Show s cursorPos
	)
	--dotnet.addEventHandler form "Shown" onShown

	local opened = off

	on isChecked do return opened
	on isEnabled do return (not opened and $ != undefined)
	on execute do
	(
		opened = not opened
		if opened do 
		(
			--updateToolbarButtons()
			form.Show()
			
			contextMenu = BuildMenuItems $ AddFabricOutportsAsString
			ShowContextMenu contextMenu
			opened = off
		)
	)
)