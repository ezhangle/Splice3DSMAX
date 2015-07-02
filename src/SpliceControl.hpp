//////////////////////////////////////////////////////////////////////////
// Small class specializes a few functions especially for Control implementations
//
#pragma once

template<typename TResultType>
class SpliceControl : public SpliceTranslationLayer < Control, TResultType > {
protected:

	SpliceControl(BOOL loading);
	~SpliceControl();

	void ResetPorts() override;

	IOResult SaveImpData(ISave* isave) override;
	IOResult LoadImpData(ILoad* isave) override;

	bool CloneSpliceData(ParentClass* pMyClone) override { return true; }; // No cloning for me...

	// Handle parent value port
	std::string m_parentArgName;
	typedef SpliceControl<TResultType> ParentClass;
};

template<typename TResultType>
SpliceControl<TResultType>::SpliceControl(BOOL loading)
	: SpliceTranslationLayer< Control, TResultType >(loading)
{

}

template<typename TResultType>
SpliceControl<TResultType>::~SpliceControl()
{

}


template<typename TResultType>
void SpliceControl<TResultType>::ResetPorts()
{
	m_parentArgName = "parentValue";
	bool success = AddSpliceParameter(GetBinding(), GetValueType(), m_parentArgName.c_str(), FabricCore::DFGPortType_In);
	if (success)
	{
		m_binding.getExec().setExecPortMetadata(m_parentArgName.c_str(), "uiHidden", "true", false);
	}
	__super::ResetPorts();
}


template<typename TResultType>
IOResult SpliceControl<TResultType>::SaveImpData(ISave* isave)
{
	isave->WriteCString(m_parentArgName.c_str());
	return IO_OK;
}


template<typename TResultType>
IOResult SpliceControl<TResultType>::LoadImpData(ILoad* iload)
{
	char* parentName = nullptr;
	iload->ReadCStringChunk(&parentName);
	m_parentArgName = parentName;
	return IO_OK;
}

