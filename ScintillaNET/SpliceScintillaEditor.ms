sysEnv = dotNetClass "System.Environment"
ScintillaNETFolder = sysEnv.GetEnvironmentVariable "SCINTILLANETDIR"
ScintillaNETAssembly = ScintillaNETFolder + "ScintillaNET.dll"
dotNet.loadAssembly ScintillaNETAssembly

-- Scintilla throws the first time we try and load it
-- TODO: Fix this horrible hack.
try (
	local tmp = dotNetObject "ScintillaNET.Scintilla"
) catch ( 
	-- but not the 2nd time.
	sc= dotNet.loadAssembly ScintillaNETAssembly
)

-- Event Helpers
fn getName evSender = ( res = evSender.Parent.tag.value)
fn setName evSender name = ( evSender.Parent.tag = dotnetmxsvalue name)
fn getStruct evSender = ( res = evSender.parent.parent.tag.value )
fn FindCtrl layout name = ( res = layout.Controls.Find name true )

struct spliceEditorStruct
(
	mSpliceClass,
	mBtnCompile,
	mKlEditor,
	mDispResults,
	mEntryPt,
	mOutputName,
	mAvailablePorts,
	mForm,

	
	-- Event handlers
	fn evDoCompile sender evt =
	(
		-- Find ptr to original splice class
		local thisStruct = getStruct(sender)
		local spliceClass = thisStruct.mSpliceClass
		local klCode = thisStruct.mKlEditor
		local entryPt = thisStruct.mEntryPt
		local resultsBox = thisStruct.mDispResults
		local outputName = thisStruct.mOutputName
		if (spliceClass != undefined) do 
		(
			spliceClass.OutPort = outputName.text;
			res = spliceClass.SetKLCode entryPt.text klCode.text 
			resultsBox.text = res;
		)
	),
	
	fn evLoadPreset sender evt =
	(
		ldFile = getOpenFileName caption:"Splice Preset"  types:"Splice Preset(*.splice)|*.splice|All Files|*.*"
		if (ldFile != undefined) do (
			local thisStruct = getStruct(sender)
			local spliceClass = thisStruct.mSpliceClass	
			local resultsBox = thisStruct.mDispResults
			local klCode = thisStruct.mKlEditor
			res = spliceClass.LoadFromFile ldFile true
			
			thisStruct.UpdateParameterTab()
			thisStruct.UpdateKLCodeTab()
			
			if (res) then (
				resultsBox.text = ("Loaded: " + ldFile)
			) else (
				resultsBox.text = ("Load Failed: " + ldFile)
			)
		)
	),
	
	fn evSavePreset sender evt =
	(
		svFile = getSaveFileName caption:"Splice Preset"  types:"Splice Preset(*.splice)|*.splice"
		if (svFile != undefined) do (
			local thisStruct = getStruct(sender)
			local spliceClass = thisStruct.mSpliceClass	
			local resultsBox = thisStruct.mDispResults
			res = spliceClass.SaveToFile svFile
			
			if (res) then (
				resultsBox.text = ("Saved to: " + svFile)
			) else (
				resultsBox.text = ("Save Failed: " + svFile)
			)
		)
	),
	
	fn evRenameParam sender evt = (
		local paramName = getName sender
		local thisStruct = getStruct sender
		local spliceClass = thisStruct.mSpliceClass
		if spliceClass.SetPortName paramName sender.Text do ( setName sender sender.Text )
		thisStruct.UpdateKLCodeTab()
	),
	
	fn evRenameOutPort sender evt = (
		local paramName = sender.Text
		local thisStruct = sender.parent.tag.value
		local spliceClass = thisStruct.mSpliceClass
		spliceClass.outPort = paramName
		thisStruct.UpdateKLCodeTab()
	),
	
	fn evMaxTypeChange sender evt = (
		local paramName = getName sender
		local thisStruct = getStruct sender
		local typeIdx = sender.SelectedIndex
		local spliceClass = thisStruct.mSpliceClass
		local legalTypes  = (spliceClass.GetLegalMaxTypes paramName) as array
		local newType = -1
		local asArray = false
		if(typeIdx > 0) do (
			newType = legalTypes[typeIdx] - 1
			asArray = (FindCtrl sender.parent "ParamArray")[1].Checked
		)
		if asArray then newType += 0x0800
		spliceClass.SetMaxType paramName newType
	),
	
	fn evDelParam sender evt = (
		local paramName = getName sender
		local thisStruct = getStruct sender
		success = thisStruct.mSpliceClass.RemovePort paramName
		-- Remove newly deleted param from table
		if success do (
			thisStruct.UpdateParameterTab()
			thisStruct.UpdateKLCodeTab()
		)		
	),
	
	fn evCreateParam sender evt = (
		local thisStruct = getStruct sender
		local rowLayout = sender.parent
		local paramName = (FindCtrl rowLayout "ParamName")[1].Text
		local paramType = (FindCtrl rowLayout "ParamType")[1].Text
		local paramExtension = (FindCtrl rowLayout "Extension")[1].Text
		local maxType = (FindCtrl rowLayout "MaxType")[1].SelectedIndex + 1
		local asArray = (FindCtrl rowLayout "ParamArray")[1].Checked
		maxType = -maxType -- Negating gives -1 (none) and -2 (default type)
		thisStruct.mSpliceClass.AddInputPort paramName paramType maxType isArray:asArray Extension:paramExtension
		
		-- Recreate parameter rows
		thisStruct.UpdateParameterTab()
		thisStruct.UpdateKLCodeTab()
	),
	
	-------------------------------------------------------------------------------------
	fn CreateEditorTab = (
		-- Generate the layout container to hold our controls
		layout = dotNetObject "System.Windows.Forms.TableLayoutPanel"
		layout.AutoSize = true;
        layout.Dock = (dotNetClass "System.Windows.Forms.DockStyle").Fill
		layout.Name = "Layout";
		
		-- Provide a link from the form back to this struct
		-- This allows dotnet event handlers to get this structs instance
		layout.tag = dotnetmxsvalue this
		layout.RowCount = 5;
		layout.ColumnCount = 2;
		
		colStyle = dotNetObject "system.windows.forms.ColumnStyle"
		colStyle.sizetype = colStyle.sizetype.absolute
		colStyle.Width = 70
		layout.ColumnStyles.Add colStyle
		colStyle = dotNetObject "system.windows.forms.ColumnStyle"
		colStyle.sizetype = colStyle.sizetype.percent
		colStyle.Width = 100
		layout.ColumnStyles.Add colStyle
		
		row = 0
		-- First item is the entry pt:
		------------------------------------- Row 1 ----------------------------------------
		aLabel = dotNetObject "System.Windows.Forms.Label"
		aLabel.text = "Entry:"
		aLabel.TextAlign = aLabel.TextAlign.MiddleLeft
		mEntryPt = dotNetObject "System.Windows.Forms.TextBox"
		mEntryPt.anchor = dotnet.combineEnums mEntryPt.anchor.Top mEntryPt.anchor.Left mEntryPt.anchor.Right
		
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 25
		layout.RowStyles.Add(rowStyle);
		layout.controls.add aLabel 0 row
		layout.controls.add mEntryPt 1 row
		
		-- 2nd item is an output port:
		------------------------------------- Row 2 ----------------------------------------
		row = row + 1
		aLabel = dotNetObject "System.Windows.Forms.Label"
		aLabel.text = "Output:\t"
		aLabel.TextAlign = aLabel.TextAlign.MiddleLeft
		mOutputName = dotNetObject "System.Windows.Forms.TextBox"
		mOutputName.anchor = dotnet.combineEnums mEntryPt.anchor.Top mEntryPt.anchor.Left mEntryPt.anchor.Right
		dotNet.addEventHandler mOutputName "TextChanged" evRenameOutPort
		
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 25
		layout.RowStyles.Add(rowStyle);
		layout.controls.add aLabel 0 row
		layout.controls.add mOutputName 1 row
		
		-- 3rd item is all available ports:
		------------------------------------- Row 3 ----------------------------------------
		row = row + 1
		aLabel = dotNetObject "System.Windows.Forms.Label"
		aLabel.text = "Available:\t"
		aLabel.TextAlign = aLabel.TextAlign.MiddleLeft
		mAvailablePorts = dotNetObject "System.Windows.Forms.TextBox"
		mAvailablePorts.anchor = dotnet.combineEnums mEntryPt.anchor.Top mEntryPt.anchor.Left mEntryPt.anchor.Right
		mAvailablePorts.ReadOnly = true
		
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 25
		layout.RowStyles.Add(rowStyle);
		layout.controls.add aLabel 0 row
		layout.controls.add mAvailablePorts 1 row
		
		------------------------------------- Row 4----------------------------------------
		row = row + 1
		-- Create a splitter to allow user to resize Code/Results window
		splitterResults = dotNetObject "System.Windows.Forms.SplitContainer"
		splitterResults.Dock = splitterResults.Dock.Fill;
        -- You can drag the splitter no nearer than 20 pixels from the right edge of the container.
        splitterResults.Panel1MinSize = 30;
        splitterResults.Panel2MinSize = 20;
		splitterResults.SplitterWidth = 6;
		splitterResults.Orientation = splitterResults.Orientation.Horizontal;
		splitterResults.FixedPanel = splitterResults.FixedPanel.Panel2
		
		mKlEditor = dotNetObject "ScintillaNET.Scintilla"
		
		mKlEditor.ConfigurationManager.CustomLocation = ScintillaNETFolder+"\SyntaxHighlighting"
		--mKlEditor.Lexing.LexerLanguageMap.add "kl" "kl";
		mKlEditor.ConfigurationManager.Language = "cpp";
		mKlEditor.ConfigurationManager.Configure();

		mKlEditor.AcceptsTab=true
		mKlEditor.AcceptsReturn = true 
		mKlEditor.MatchBraces = true
		mKlEditor.Margins.Margin0.Width = 20
		mKlEditor.Dock = (dotNetClass "System.Windows.Forms.DockStyle").Fill
		
		mDispResults = dotNetObject "System.Windows.Forms.RichTextBox"
		mDispResults.multiline = true
		mDispResults.ReadOnly = true
		mDispResults.WordWrap = false
		mDispResults.ScrollBars = mDispResults.ScrollBars.Both
		mDispResults.BackColor = mDispResults.BackColor.LightGray
		mDispResults.Dock = (dotNetClass "System.Windows.Forms.DockStyle").Fill
		
		splitterResults.Panel1.Controls.Add mKlEditor
		splitterResults.Panel2.Controls.Add mDispResults
		
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.percent
		rowStyle.height = 100
		layout.RowStyles.Add(rowStyle);
		layout.controls.Add splitterResults 0 row
		layout.SetColumnSpan splitterResults row
		
		------------------------------------- Row 5 ----------------------------------------
		row = row + 1
		
		global ctrBtnlLayout = dotNetObject "System.Windows.Forms.Panel"
		--ctrBtnlLayout.FlowDirection = ctrBtnlLayout.FlowDirection.LeftToRight
		--ctrBtnlLayout.WrapContents = false
		ctrBtnlLayout.Dock = ctrBtnlLayout.Dock.Fill;
		--ctrlBtnLayout.BackgroundColor = ctrlBtnLayout.BackgroundColor.red;
		
		mBtnCompile = dotNetObject "System.windows.forms.button"
		mBtnCompile.text = "Compile"
		mBtnCompile.Anchor = mBtnCompile.Anchor.None --mBtnCompile.Anchor.Right --dotnet.combineEnums mBtnCompile.Anchor.Top mEntryPt.anchor.Right
		mBtnCompile.Dock = (dotNetClass "System.Windows.Forms.DockStyle").Right
		
		mBtnLoad = dotNetObject "System.windows.forms.button"
		mBtnLoad.text = "Load"
		mBtnLoad.Anchor = mBtnLoad.anchor.Left
		mBtnLoad.Dock = (dotNetClass "System.Windows.Forms.DockStyle").Left

		mBtnSave = dotNetObject "System.windows.forms.button"
		mBtnSave.text = "Save"
		mBtnSave.Anchor = mBtnSave.anchor.Left
		mBtnSave.Dock = (dotNetClass "System.Windows.Forms.DockStyle").Left

		ctrBtnlLayout.controls.Add mBtnCompile
		ctrBtnlLayout.controls.Add mBtnLoad
		ctrBtnlLayout.controls.Add mBtnSave

		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 30
		layout.RowStyles.Add(rowStyle);
		--layout.controls.Add mBtnCompile 1 row
		layout.controls.Add ctrBtnlLayout 0 row
		layout.SetColumnSpan ctrBtnlLayout row
		
		--layout.controls.Add mBtnLoad 1 row
		--layout.controls.Add mBtnSave 1 row

		-- Add event handlerse
		dotNet.addEventHandler mBtnCompile "click" evDoCompile
		dotNet.addEventHandler mBtnSave "click" evSavePreset
		dotNet.addEventHandler mBtnLoad "click" evLoadPreset
		
		-- Return result
		tabCodeEditor = dotNetObject "System.Windows.Forms.TabPage" "KL Editor"
		tabCodeEditor.Controls.Add layout;
		tabCodeEditor;
	),
	
	-------------------------------------------------------------------------------------------------------------------
	m_IntToType = #("Float", "Int", "RGBA", "Point3", "BOOL", "Angle", "Percent", "Float World", "String", "Filename", "hsv_OBS", \
								"ColorChannel", "TimeValue", "Radio Button", "Material", "Texture", "Bitmap", "Scene Node", "Reference Target", \
								"Index", "Matrix", "ParameterBlock", "Point4", "32Bit RGBA", \ -- The following types are not directly representable in PB2, but perhaps
								"Enum", "Void", "Interval", "AngleAxis", "Quat", "Ray", \		  -- we could figure out a way to support them... for example, Quat would be nice
								"Point2", "BitArray", "Class", "Mesh", "Object", "Control", "Point", "String", "IObject", "Interface", "HWND", "Name", \
								"Color", "FPValue", "MXSValue", "DWORD", "bool", "INTPTR", "INT64", "Double"),
										
	fn GenerateNamesForTypes bitArrayTypes = (
		ar = #();
		for i in bitArrayTypes do (
			append ar m_IntToType[i]
		)
		ar;
	),
	
	-------------------------------------------------------------------------------------------------------------------
	fn GetSelBitIndex bitArrayTypes type = (
		-- Offset the type by 1 to match because we
		-- are matching vs indices here
		type = type + 1;
		ar = bitArrayTypes as array
		res = 0
		for i = 1 to ar.count do (
			if  ar[i] == type do res = i
		)
		res
	),
	
	-------------------------------------------------------------------------------------------------------------------
	fn CreateParameterCtrl index = (
		-- Create a horizontal layout
		ctrlLayout = dotNetObject "System.Windows.Forms.FlowLayoutPanel"
		ctrlLayout.FlowDirection = ctrlLayout.FlowDirection.LeftToRight
		ctrlLayout.WrapContents = false
		ctrlLayout.height = 26
		ctrlLayout.Dock = ctrlLayout.Dock.Fill;
		tabsUsed = index * 3
		
		-- Add editable name box
		local paramName = mSpliceClass.GetPortName index
		local nameEdit = dotNetObject "System.Windows.Forms.TextBox"
		nameEdit.Name = "ParamName"
		nameEdit.Text = paramName
		nameEdit.TabStop = true
		nameEdit.TabIndex = tabsUsed + 1
		ctrlLayout.Controls.Add nameEdit
		dotNet.addEventHandler nameEdit "TextChanged" evRenameParam

		-- The following line does not actually set the name of the 
		-- param/edit, what it does is remembers the name of the parameter
		-- being edited by this ctrlLayout, so evHandlers can
		-- call functions on the appropriate port in splice
		-- NOTE: This used to be the param index, but I discovered that renaming
		-- parameters could reorder their index, so using full names
		-- seemed much safer.
		setName nameEdit paramName
		
		-- Add non-editable type
		local spliceType = mSpliceClass.GetPortType paramName
		local typeLabel = dotNetObject "System.Windows.Forms.TextBox"
		typeLabel.Name = "ParamType"
		typeLabel.Enabled = false
		typeLabel.Text = spliceType
		typeLabel.Width = 110;
		typeLabel.TabStop = false
		global __label = typeLabel
		typeLabel.BackColor = typeLabel.BackColor.LightGray;
		ctrlLayout.Controls.Add typeLabel
		
		-- Add extension configuration
		-- Splice doesn't alow us to query the extension used. 
		-- local ext = mSpliceClass.GetPortExtension index
		-- local extLabel = dotNetObject "System.Windows.Forms.TextBox"
		-- extLabel.Name = "Extension"
		-- extLabel.Enabled = false
		-- extLabel.Text = spliceType
		-- extLabel.Width = 110;
		-- extLabel.TabStop = false
		-- global __label = extLabel
		-- extLabel.BackColor = extLabel.BackColor.LightGray;
		-- ctrlLayout.Controls.Add extLabel
		
		-- Add isArray checkbox
		local isArray = mSpliceClass.IsPortArray paramName
		local isArrayCB = dotNetObject "System.Windows.Forms.Checkbox"
		isArrayCB.Checked = isArray
		isArrayCB.Name = "ParamArray"
		isArrayCB.Text = "Is Array"
		isArrayCB.Enabled = false
		isArrayCB.Width = 75
		isArrayCB.TabStop = false
		ctrlLayout.Controls.Add isArrayCB

		-- Allow user to switch between legal max types for input
		local maxType = mSpliceClass.GetMaxType paramName
		local legalMaxTypes = mSpliceClass.GetLegalMaxTypes paramName
		local ddlist  =  dotNetObject "System.Windows.Forms.ComboBox"
		ddlist.Name = "MaxType"
		ddlist.DropDownStyle = ddlist.DropDownStyle.DropDownList
		ddlist.items.addrange #("-none-")
		ddlist.items.addrange (GenerateNamesForTypes legalMaxTypes)
		ddlist.SelectedIndex = (GetSelBitIndex legalMaxTypes maxType)
		ddlist.TabStop = true
		ddlist.TabIndex = tabsUsed + 2
		dotNet.addEventHandler ddlist "SelectedIndexChanged" evMaxTypeChange
		ctrlLayout.Controls.Add ddlist
	
		-- See http://tech-artists.org/forum/showthread.php?588-Maxscript-dotNet-and-the-vicious-ComboBox
		dotNet.setLifetimeControl ddlist #dotnet
		
		-- Allow user to delete parameters
		local delBtn = dotNetObject  "System.Windows.Forms.Button"
		delBtn.Text = "Delete"
		delBtn.TabStop = true
		delBtn.TabIndex = tabsUsed + 3
		dotNet.addEventHandler delBtn "Click" evDelParam
		ctrlLayout.Controls.Add delBtn
		
		ctrlLayout;
	),
	
	fn CreateNewParameterLabels = (
		local nParams = mSpliceClass.PortCount
		local tabIndex  = nParams * 3 + 100
		-- Create a horizontal layout
		ctrlLayout = dotNetObject "System.Windows.Forms.FlowLayoutPanel"
		ctrlLayout.FlowDirection = ctrlLayout.FlowDirection.LeftToRight
		ctrlLayout.WrapContents = false
		ctrlLayout.height = 26
		ctrlLayout.Dock = ctrlLayout.Dock.Fill;
		
		-- Add editable name box

		nameLabel = dotNetObject "System.Windows.Forms.Label"
		nameLabel.text = "Name:"
		nameLabel.TextAlign = nameLabel.TextAlign.MiddleLeft
		ctrlLayout.Controls.Add nameLabel

		-- Add editable type label
		typeLabel = dotNetObject "System.Windows.Forms.Label"
		typeLabel.text = "KLType:"
		typeLabel.TextAlign = typeLabel.TextAlign.MiddleLeft
		ctrlLayout.Controls.Add typeLabel

		-- Add isarray lable
		isArrayLabel = dotNetObject "System.Windows.Forms.Label"
		isArrayLabel.text = "IsArray:"
		isArrayLabel.TextAlign = isArrayLabel.TextAlign.MiddleLeft
		ctrlLayout.Controls.Add isArrayLabel

		-- Add editable extension box

		extLabel = dotNetObject "System.Windows.Forms.Label"
		extLabel.text = "Extension:"
		extLabel.TextAlign = extLabel.TextAlign.MiddleLeft
		ctrlLayout.Controls.Add extLabel

		-- Create empty dd list as place holder

		maxTypeLabel = dotNetObject "System.Windows.Forms.Label"
		maxTypeLabel.text = "MaxType:"
		maxTypeLabel.TextAlign = maxTypeLabel.TextAlign.MiddleRight
		ctrlLayout.Controls.Add maxTypeLabel

		ctrlLayout;
	),
	
	fn CreateNewParameterCtrl = (
		local nParams = mSpliceClass.PortCount
		local tabIndex  = nParams * 3 + 100
		-- Create a horizontal layout
		ctrlLayout = dotNetObject "System.Windows.Forms.FlowLayoutPanel"
		ctrlLayout.FlowDirection = ctrlLayout.FlowDirection.LeftToRight
		ctrlLayout.WrapContents = false
		ctrlLayout.height = 26
		ctrlLayout.Dock = ctrlLayout.Dock.Fill;
		
		-- Add editable name box
		local nameEdit = dotNetObject "System.Windows.Forms.TextBox"
		nameEdit.Name = "ParamName"
		nameEdit.Text = "-New Port Name-"
		nameEdit.TabIndex = tabIndex + 1
		ctrlLayout.Controls.Add nameEdit
		
		-- Add editable type
		local spliceType =  dotNetObject "System.Windows.Forms.ComboBox"
		spliceType.Name = "SpliceType"
		spliceType.items.addrange #("Integer", "Scalar", "Vec2", "Vec3", "Vec4", "Color", "Boolean", "Mat44", "Quat", "String", "PolygonMesh")
		spliceType.Width = 110;
		spliceType.TabIndex = tabIndex + 2
		spliceType.Name = "ParamType"
		ctrlLayout.Controls.Add spliceType

		-- Add isArray checkbox
		local isArrayCB = dotNetObject "System.Windows.Forms.Checkbox"
		isArrayCB.Name = "ParamArray"
		isArrayCB.Text = "Is Array"
		isArrayCB.Width = 75
		isArrayCB.TabIndex = tabIndex + 4
		ctrlLayout.Controls.Add isArrayCB

		-- Add editable extension box
		local extEdit = dotNetObject "System.Windows.Forms.TextBox"
		extEdit.Name = "Extension"
		extEdit.Text = ""
		extEdit.TabIndex = tabIndex + 5
		ctrlLayout.Controls.Add extEdit
		
		-- Create empty dd list as place holder
		local maxType  =  dotNetObject "System.Windows.Forms.ComboBox"
		maxType.Name = "MaxType"
		maxType.DropDownStyle = maxType.DropDownStyle.DropDownList
		maxType.items.addrange #("-none-", "-default type-")
		maxType.SelectedIndex = 1
		maxType.TabIndex = tabIndex + 6
		ctrlLayout.Controls.Add maxType
	
		-- See http://tech-artists.org/forum/showthread.php?588-Maxscript-dotNet-and-the-vicious-ComboBox
		dotNet.setLifetimeControl spliceType #dotnet
		dotNet.setLifetimeControl maxType #dotnet
		
		-- Allow user to delete parameters
		local createBtn = dotNetObject  "System.Windows.Forms.Button"
		createBtn.Text = "Create"
		createBtn.TabIndex = tabIndex + 7
		dotNet.addEventHandler createBtn "Click" evCreateParam
		ctrlLayout.Controls.Add createBtn
		
		ctrlLayout;
	),
	
	-------------------------------------------------------------------------------------------------------------------
	fn CreateParameterRows tabLayout = (

		-- Add a heading row for the existing ports.
		portsHeadingLayout = dotNetObject "System.Windows.Forms.FlowLayoutPanel"
		portsHeadingLayout.FlowDirection = portsHeadingLayout.FlowDirection.LeftToRight
		portsHeadingLayout.WrapContents = false
		portsHeadingLayout.height = 26
		portsHeadingLayout.Dock = portsHeadingLayout.Dock.Fill;

		nameLabel = dotNetObject "System.Windows.Forms.Label"
		nameLabel.text = "Existing Ports:"
		nameLabel.TextAlign = nameLabel.TextAlign.MiddleLeft
		portsHeadingLayout.Controls.Add nameLabel

		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 30
		tabLayout.RowStyles.Add(rowStyle);
		tabLayout.Controls.Add portsHeadingLayout 0 0


		-- Generate row contents, add to table
		local nParams = mSpliceClass.PortCount
		tabLayout.RowCount = nParams + 1
		for i = 1 to nParams do
		(
			local paramCtrl = CreateParameterCtrl i
			rowStyle = dotNetObject "system.windows.forms.rowstyle"
			rowStyle.sizetype = rowStyle.sizetype.absolute
			rowStyle.height = 30
			tabLayout.RowStyles.Add(rowStyle);
			tabLayout.Controls.Add paramCtrl 0 (i)
		)
		

		-- Add empty row to allow new creations...
		local labelsRow = CreateNewParameterLabels()
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 30
		tabLayout.RowStyles.Add(rowStyle);
		tabLayout.Controls.Add labelsRow 0 (nParams+1) 

		-- Add empty row to allow new creations...
		local emptyRow = CreateNewParameterCtrl()
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 30
		tabLayout.RowStyles.Add(rowStyle);
		tabLayout.Controls.Add emptyRow 0 (nParams+2)
		tabLayout
	),
	
	-------------------------------------------------------------------------------------------------------------------
	fn CreateParameterTab = (
		tabParams = dotNetObject "System.Windows.Forms.TabPage" "Parameters"
		
		-- Create layout.  This layout arranges the list of input Splice parameters
		local tabLayout = dotNetObject "System.Windows.Forms.TableLayoutPanel"
		tabLayout.AutoSize = true;
		tabLayout.Dock = tabLayout.Dock.Fill
		tabLayout.Name = "ParameterTable";
		-- Provide a link from the form back to this struct
		-- This allows dotnet event handlers to get this structs instance
		tabLayout.tag = dotnetmxsvalue this
		
		-- 1 column.  I tried using a FlowPanel but could not get rows to fill column completely
		tabLayout.ColumnCount = 1;
		colStyle = dotNetObject "system.windows.forms.ColumnStyle"
		colStyle.sizetype = colStyle.sizetype.percent
		colStyle.Width = 100
		tabLayout.ColumnStyles.Add colStyle

		-- Add all rows to table
		CreateParameterRows tabLayout;
		tabParams.Controls.Add tabLayout
		tabParams;
	),
	
	fn UpdateParameterTab = (
		-- This is a nasty nasty way to access the paramtable
		paramTable = mForm.Controls.Item[0].Controls.Item[1].Controls.Item[0]
		paramTable.Controls.Clear()
		CreateParameterRows paramTable
	),
	
	fn UpdateKLCodeTab = (
		if (mSpliceClass != undefined) do 
		(
			mKlEditor.text = mSpliceClass.GetKLCode()
			mEntryPt.text = mSpliceClass.GetKLOperatorName();
			mOutputName.text = mSpliceClass.OutPort;
			mAvailablePorts.text = mSpliceClass.AllPortsSignature
		)
	),
	
	on create do 
	(
		mForm = dotNetObject "MaxCustomControls.MaxForm"
		mForm.Height = 460;
		mForm.Width = 600;
		mForm.TabStop = false;
		
		tabCtrl = dotNetObject "System.Windows.Forms.TabControl"
		tabCtrl.AutoSize = true;
        tabCtrl.dock = tabCtrl.dock.Fill
		tabEditor = CreateEditorTab();
		
		-- Initialize the code.  This _must_ be done before creating the
		-- parameter tab, as it is possible for calling GetKLCode to cause a
		-- default parameter to be added to the DGNode
		updateKLCodeTab()
		
		-- Somewhere (post-create) our form fore color is set to grey?
		-- Can't find out how, so we set our tabs explicitly here
		tabEditor.ForeColor = tabEditor.ForeColor.Black
		tabCtrl.TabPages.Add tabEditor
		
		tabParams = CreateParameterTab()
		tabParams.ForeColor = tabParams.ForeColor.Black
		tabCtrl.TabPages.Add tabParams

		------------------------------------- Finalize ----------------------------------------
		-- Add to form
		mForm.controls.add tabCtrl


		
		-- Use MaxForm's special display fn to ensure form is child of Max
		mForm.ShowModeless()
		-- Transfer scope to .Net from MxS
		dotNet.setLifeTimeControl mForm #dotNet
	)
)

-- Entry point, this fn will be called from C++ with instance of class to be edited.
fn fn_From_Cpp_ShowKLEditor spliceClass = (
	newStruct = spliceEditorStruct mSpliceClass:spliceClass
	newStruct
)

-- Entry point, this fn will be called from C++ with instance of class to be updated.
fn fn_From_Cpp_UpdateKLEditor klEditorStruct = (
	if (klEditorStruct != undefined) do (
		-- Update port UI.  
		klEditorStruct.UpdateParameterTab()
		-- Update KL Code?
		klEditorStruct.UpdateKLCodeTab()
	)
)

fn fn_From_Cpp_CloseKLEditor_Dialog klEditorStruct = (
	-- Create the struct to hold our values
	if (klEditorStruct != undefined) do (
		klEditorStruct.mForm.Close()
	)
)
