dotNet.loadAssembly @"ScintillaNET.dll"
-- Scintilla throws the first time we try and load it
-- TODO: Fix this horrible hack.
try (
	local tmp = dotNetObject "ScintillaNET.Scintilla"
) catch ( 
	-- but not the 2nd time.
	sc= dotNet.loadAssembly @"ScintillaNET.dll"
)

-- Event Helpers
fn getStruct evSender = ( res = evSender.parent.parent.tag.value )
fn getIndex evSender = ( res = evSender.Parent.Parent.Controls.GetChildIndex evSender.Parent + 1)
fn FindCtrl layout name = ( res = layout.Controls.Find name true )

struct spliceEditorStruct
(
	mSpliceClass,
	mBtnCompile,
	mKlEditor,
	mDispResults,
	mEntryPt,
	mOutputName,
	mForm,

	
	-- Event handlers
	fn evDoCompile sender evt =
	(
		-- Find ptr to original splice class
		thisStruct = sender.parent.tag.value
		spliceClass = thisStruct.mSpliceClass
		klCode = thisStruct.mKlEditor
		entryPt = thisStruct.mEntryPt
		resultsBox = thisStruct.mDispResults
		outputName = thisStruct.mOutputName
		if (spliceClass != undefined) do 
		(
			spliceClass.OutPort = outputName.text;
			res = spliceClass.SetKLCode entryPt.text klCode.text 
			resultsBox.text = res;
		)
	),
	fn evRenameParam sender evt = (
		local paramIdx = getIndex sender
		local thisStruct = getStruct sender
		local spliceClass = thisStruct.mSpliceClass
		spliceClass.SetPortName paramIdx sender.Text
	),
	
	fn evMaxTypeChange sender evt = (
		local paramIdx = getIndex sender
		local thisStruct = getStruct sender
		local typeIdx = sender.SelectedIndex
		local spliceClass = thisStruct.mSpliceClass
		local legalTypes  = (spliceClass.GetLegalMaxTypes paramIdx) as array
		local newType = legalTypes[typeIdx] - 1
		local asArray = (FindCtrl sender.parent "ParamArray")[1].Checked
		if asArray then newType += 0x0800
		spliceClass.SetMaxType paramIdx newType
	),
	
	fn evDelParam sender evt = (
		local paramIdx = getIndex sender
		local thisStruct = getStruct sender
		success = thisStruct.mSpliceClass.DeletePort paramIdx
		-- Remove newly deleted param from table
		if success do (
			sender.Parent.Parent.Controls.Remove sender.Parent
		)		
	),
	
	fn evCreateParam sender evt = (
		local thisStruct = getStruct sender
		global rowLayout = sender.parent
		local paramName = (FindCtrl rowLayout "ParamName")[1].Text
		local paramType = (FindCtrl rowLayout "ParamType")[1].Text
		local maxType = (FindCtrl rowLayout "MaxType")[1].SelectedIndex + 1
		local asArray = (FindCtrl rowLayout "ParamArray")[1].Checked
		maxType = -maxType -- Negating gives -1 (none) and -2 (default type)
		thisStruct.mSpliceClass.CreatePort paramName paramType maxType isArray:asArray
		
		-- Recreate parameter rows
		local paramTable = rowLayout.parent
		paramTable.Controls.Clear()
		thisStruct.CreateParameterRows paramTable
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
		layout.RowCount = 4;
		layout.ColumnCount = 2;
		
		colStyle = dotNetObject "system.windows.forms.ColumnStyle"
		colStyle.sizetype = colStyle.sizetype.absolute
		colStyle.Width = 80
		layout.ColumnStyles.Add colStyle
		colStyle = dotNetObject "system.windows.forms.ColumnStyle"
		colStyle.sizetype = colStyle.sizetype.percent
		colStyle.Width = 100
		layout.ColumnStyles.Add colStyle
		
		-- First item is the entry pt:
		------------------------------------- Row 1 ----------------------------------------
		aLabel = dotNetObject "System.Windows.Forms.Label"
		aLabel.text = "Entry:"
		mEntryPt = dotNetObject "System.Windows.Forms.TextBox"
		mEntryPt.anchor = dotnet.combineEnums mEntryPt.anchor.Top mEntryPt.anchor.Left mEntryPt.anchor.Right
		
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 30
		layout.RowStyles.Add(rowStyle);
		layout.controls.add aLabel 0 0
		layout.controls.add mEntryPt 1 0
			
		-- 2nd item is an output port:
		------------------------------------- Row 2 ----------------------------------------
		aLabel = dotNetObject "System.Windows.Forms.Label"
		aLabel.text = "Output:\t"
		mOutputName = dotNetObject "System.Windows.Forms.TextBox"
		mOutputName.anchor = dotnet.combineEnums mEntryPt.anchor.Top mEntryPt.anchor.Left mEntryPt.anchor.Right
		
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 30
		layout.RowStyles.Add(rowStyle);
		layout.controls.add aLabel 0 1
		layout.controls.add mOutputName 1 1
		
		------------------------------------- Row 2 ----------------------------------------
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
		
		mKlEditor.ConfigurationManager.CustomLocation = "D:\Program Files\Autodesk\3ds Max 2014\SyntaxHighlighting"
		--mKlEditor.Lexing.LexerLanguageMap.add "kl" "kl";
		mKlEditor.ConfigurationManager.Language = "cpp";
		mKlEditor.ConfigurationManager.Configure();

		mKlEditor.AcceptsTab=true
		mKlEditor.AcceptsReturn = true 
		mKlEditor.MatchBraces = true
		mKlEditor.Margins.Margin0.Width = 20
		mKlEditor.Dock = (dotNetClass "System.Windows.Forms.DockStyle").Fill
		
		mDispResults = dotNetObject "System.Windows.Forms.TextBox"
		mDispResults.multiline = true
		mDispResults.ReadOnly = true
		mDispResults.BackColor = mDispResults.BackColor.LightGray
		mDispResults.Dock = (dotNetClass "System.Windows.Forms.DockStyle").Fill
		
		splitterResults.Panel1.Controls.Add mKlEditor
		splitterResults.Panel2.Controls.Add mDispResults
		
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.percent
		rowStyle.height = 100
		layout.RowStyles.Add(rowStyle);
		layout.controls.Add splitterResults 0 2
		layout.SetColumnSpan splitterResults 2
		
		------------------------------------- Row 3 ----------------------------------------
		mBtnCompile = dotNetObject "System.windows.forms.button"
		mBtnCompile.text = "Compile"
		mBtnCompile.Anchor = dotnet.combineEnums mBtnCompile.Anchor.Top mEntryPt.anchor.Right

		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 30
		layout.RowStyles.Add(rowStyle);
		layout.controls.Add mBtnCompile 1 3

		-- Add event handlerse
		dotNet.addEventHandler mBtnCompile "click" evDoCompile
		
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
		
		-- Add editable name box
		local paramName = mSpliceClass.GetPortName index
		local nameEdit = dotNetObject "System.Windows.Forms.TextBox"
		nameEdit.Name = "ParamName"
		nameEdit.Text = paramName
		ctrlLayout.Controls.Add nameEdit
		dotNet.addEventHandler nameEdit "TextChanged" evRenameParam
		
		-- Add non-editable type
		local spliceType = mSpliceClass.GetPortType index
		local typeLabel = dotNetObject "System.Windows.Forms.TextBox"
		typeLabel.Name = "ParamType"
		typeLabel.Enabled = false
		typeLabel.Text = spliceType
		typeLabel.Width = 110;
		global __label = typeLabel
		typeLabel.BackColor = typeLabel.BackColor.LightGray;
		ctrlLayout.Controls.Add typeLabel
		
		-- Add isArray checkbox
		local isArray = mSpliceClass.IsPortArray index
		local isArrayCB = dotNetObject "System.Windows.Forms.Checkbox"
		isArrayCB.Checked = isArray
		isArrayCB.Name = "ParamArray"
		isArrayCB.Text = "Is Array"
		isArrayCB.Enabled = false
		isArrayCB.Width = 75
		ctrlLayout.Controls.Add isArrayCB

		-- Allow user to switch between legal max types for input
		local maxType = mSpliceClass.GetMaxType index
		local legalMaxTypes = mSpliceClass.GetLegalMaxTypes index
		local ddlist  =  dotNetObject "System.Windows.Forms.ComboBox"
		ddlist.Name = "MaxType"
		ddlist.DropDownStyle = ddlist.DropDownStyle.DropDownList
		ddlist.items.addrange #("-none-")
		ddlist.items.addrange (GenerateNamesForTypes legalMaxTypes)
		ddlist.SelectedIndex = (GetSelBitIndex legalMaxTypes maxType)
		dotNet.addEventHandler ddlist "SelectedIndexChanged" evMaxTypeChange
		ctrlLayout.Controls.Add ddlist
	
		-- See http://tech-artists.org/forum/showthread.php?588-Maxscript-dotNet-and-the-vicious-ComboBox
		dotNet.setLifetimeControl ddlist #dotnet
		
		-- Allow user to delete parameters
		local delBtn = dotNetObject  "System.Windows.Forms.Button"
		delBtn.Text = "Delete"
		dotNet.addEventHandler delBtn "Click" evDelParam
		ctrlLayout.Controls.Add delBtn
		
		ctrlLayout;
	),
	
	fn CreateNewParameterCtrl = (
		-- Create a horizontal layout
		ctrlLayout = dotNetObject "System.Windows.Forms.FlowLayoutPanel"
		ctrlLayout.FlowDirection = ctrlLayout.FlowDirection.LeftToRight
		ctrlLayout.WrapContents = false
		ctrlLayout.height = 26
		ctrlLayout.Dock = ctrlLayout.Dock.Fill;
		
		-- Add editable name box
		local nameEdit = dotNetObject "System.Windows.Forms.TextBox"
		nameEdit.Name = "ParamName"
		nameEdit.Text = "-Param Name-"
		ctrlLayout.Controls.Add nameEdit
		
		-- Add editable type
		local spliceType =  dotNetObject "System.Windows.Forms.ComboBox"
		spliceType.Name = "SpliceType"
		spliceType.items.addrange #("Size", "Scalar", "Vec2", "Vec3", "Vec4", "Color", "Boolean", "Mat44", "Quat", "String", "PolygonMesh")
		spliceType.Width = 110;
		spliceType.Name = "ParamType"
		ctrlLayout.Controls.Add spliceType
		
		-- Add isArray checkbox
		local isArrayCB = dotNetObject "System.Windows.Forms.Checkbox"
		isArrayCB.Name = "ParamArray"
		isArrayCB.Text = "Is Array"
		isArrayCB.Width = 75
		ctrlLayout.Controls.Add isArrayCB

		-- Create empty dd list as place holder
		local maxType  =  dotNetObject "System.Windows.Forms.ComboBox"
		maxType.Name = "MaxType"
		maxType.DropDownStyle = maxType.DropDownStyle.DropDownList
		maxType.items.addrange #("-none-", "-default type-")
		maxType.SelectedIndex = 1
		ctrlLayout.Controls.Add maxType
	
		-- See http://tech-artists.org/forum/showthread.php?588-Maxscript-dotNet-and-the-vicious-ComboBox
		dotNet.setLifetimeControl spliceType #dotnet
		dotNet.setLifetimeControl maxType #dotnet
		
		-- Allow user to delete parameters
		local createBtn = dotNetObject  "System.Windows.Forms.Button"
		createBtn.Text = "Create"
		dotNet.addEventHandler createBtn "Click" evCreateParam
		ctrlLayout.Controls.Add createBtn
		
		ctrlLayout;
	),
	
	-------------------------------------------------------------------------------------------------------------------
	fn CreateParameterRows tabLayout = (
		-- Generate row contents, add to table
		local nParams = mSpliceClass.NumPorts
		tabLayout.RowCount = nParams + 1
		for i = 1 to nParams do
		(
			local paramCtrl = CreateParameterCtrl i
			rowStyle = dotNetObject "system.windows.forms.rowstyle"
			rowStyle.sizetype = rowStyle.sizetype.absolute
			rowStyle.height = 30
			tabLayout.RowStyles.Add(rowStyle);
			tabLayout.Controls.Add paramCtrl 0 (i-1)
		)
		
		-- Add empty row to allow new creations...
		local emptyRow = CreateNewParameterCtrl()
		rowStyle = dotNetObject "system.windows.forms.rowstyle"
		rowStyle.sizetype = rowStyle.sizetype.absolute
		rowStyle.height = 30
		tabLayout.RowStyles.Add(rowStyle);
		tabLayout.Controls.Add emptyRow 0 nParams
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
	
	on create do 
	(
		mForm = dotNetObject "MaxCustomControls.MaxForm"
		mForm.Height = 460;
		mForm.Width = 600;
		
		tabCtrl = dotNetObject "System.Windows.Forms.TabControl"
		tabCtrl.AutoSize = true;
        tabCtrl.dock = tabCtrl.dock.Fill
		tabEditor = CreateEditorTab();
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

		-- Initialize the code
		if (mSpliceClass != undefined) do 
		(
			mKlEditor.text = mSpliceClass.GetKLCode()
			mEntryPt.text = mSpliceClass.GetKLOperatorName();
			mOutputName.text = mSpliceClass.OutPort;
		)
		
		-- Use MaxForm's special display fn to ensure form is child of Max
		mForm.showModeless()
		-- Transfer scope to .Net from MxS
		dotNet.setLifeTimeControl mForm #dotNet
	)
)

-- Entry point, this fn will be called from C++ with instance of class to be edited.
fn fn_From_Cpp_ShowKLEditor spliceClass = (
	global newStruct = spliceEditorStruct mSpliceClass:spliceClass
)