

macroScript WireFabricParameters category:"FabricTools" buttonText:"Wire Fabric Ports" tooltip:"Connect Fabric ports to propagate fabric values between graphs"
(
	local form = dotnetobject "MaxCustomControls.Maxform"
	form.ShowInTaskbar = off
	form.FormBorderStyle = form.FormBorderStyle.None
	form.AllowTransparency = on
	form.Opacity = 0
	
	form.StartPosition = form.StartPosition.Manual
	form.Size = Form.MinimumSize
	form.Location = dotnetobject "System.Drawing.Point" -100 -100

	local sourcePortSpec = ""
	local sourcePortName = ""
	local sourcePortFabricEntity = undefined

	--------------------------------------------------------------------------------------------
	-- ContextMenu build & show functions
	
	-- Utiltiy fn, calls doFn_ callback for every
	-- Fabric entity in the subanim hierarchy
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
	
	fn onCollapse s e = 
	(
		form.Hide()
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
	
	--------------------------------------------------------------------------------------------
	-- Callback functions for working with/building ContextmMenu 
	
	-- On selecting dest port, connect to previously selected source port
	fn onFabInPortClicked s e =
	(
		local destPortName = s.Name
		local destPortFabEntity = s.tag.value
		
		-- Assume we have already picked our first port
		res = destPortFabEntity.ConnectArgs destPortName sourcePortFabricEntity sourcePortName postUI:true
		if not res then ( print ("ERROR: Could not connect src: " + sourcePortName + " -> dst: " + destPortName ) )
		else ( print (" Successfully connected ports: '" + sourcePortName + "' -> '" + destPortName + "'") )
	)
	
	-- Add In Ports to passed ContextMenu
	fn AddFabricInPortsAsString obj cm = (
		cm = cm.menuitems.Add obj.name
		
		if (obj == sourcePortFabricEntity) then (
			cm.Enabled = false
		)
		else 
		(
			-- Add all out ports to 
			for i = 1 to obj.GetPortCount() do (
				portName = obj.GetPortName i
				if (obj.getPortType portName != #DFGPortOut) do (
					portMenuItem = cm.menuitems.Add portName
					portMenuItem.tag = dotNetMXSValue obj
					portMenuItem.Name = portName
					dotnet.addEventHandler portMenuItem "Click" onFabInPortClicked
					
					-- If this type is already connected, show it as an in-port
					portSrc = obj.GetPortMetaData portName "SrcPort"
					if (portSrc != "") do ( 
						portMenuItem.Text = "(" + portSrc + ") -> " + portMenuItem.Text
					)
					
					-- If the types don't match, then disable this option 
					portMenuItem.Enabled = obj.getPortSpec portName == sourcePortSpec
				)
			)
		)
	)
	
	-- Filter callback for filtering 
	classList = #()
	fn GatherFabricClasses obj cm = ( append classList obj )
	
	fn targetFabEntityFilter o = 
	(
		classList = #()
		SearchForFabricClasses o false GatherFabricClasses
		res = classList.count > 0
	)
	
	-- Callback for ContextMenu "Click" event.  Triggers a second
	-- Selects clicked out port, and shows context menu for selecting destination port
	fn onFabOutPortClicked s e =
	(
		sourcePortName = s.Name
		sourcePortFabricEntity = s.tag.value
		sourcePortSpec = sourcePortFabricEntity.getPortSpec sourcePortName
		nodes = refs.dependentNodes sourcePortFabricEntity.value
		res = false
		if (nodes.count > 1) then (
			lastNode = nodes[nodes.count]
			-- pick our destination object
			res = pickObject rubberBand:lastNode.pos prompt:"Pick the target Fabric Entity" filter:targetFabEntityFilter
		) else (
			-- pick our destination object
			res = pickObject prompt:"sdvsPick the target Fabric Entity" filter:targetFabEntityFilter
		)
		if res != undefined do (
			contextMenu = BuildMenuItems #(res) AddFabricInPortsAsString
			ShowContextMenu contextMenu
		)
		form.Hide()
	)
	
	-- Add Out Ports to passed ContextMenu
	fn AddFabricOutportsAsString obj cm = (
		cm = cm.menuitems.Add obj.name
		-- Add all out ports to 
		for i = 1 to obj.GetPortCount() do (
			portName = obj.GetPortName i
			if (obj.getPortType portName != #DFGPortIn) do (
				portMenuItem = cm.menuitems.Add portName
				portMenuItem.tag = dotNetMXSValue obj
				portMenuItem.Name = portName
				dotnet.addEventHandler portMenuItem "Click" onFabOutPortClicked
			)
		)
	)
	
	---------------------------------------------------------------------
	-- Macroscript management

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