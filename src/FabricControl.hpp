//////////////////////////////////////////////////////////////////////////
// Small class specializes a few functions especially for Control implementations
//
#pragma once

template<typename TResultType>
class FabricControl : public FabricTranslationLayer < Control, TResultType > {
protected:

	FabricControl(BOOL loading);
	~FabricControl();

	void ResetPorts() override;

	IOResult SaveImpData(ISave* isave) override;
	IOResult LoadImpData(ILoad* isave) override;

	bool CloneFabricData( FabricTranslationLayer < Control, TResultType >* pMyClone) override; // No cloning for me...

	// Handle parent value port
	virtual int GetParentValueType() = 0;
	std::string m_parentArgName;
	typedef FabricControl<TResultType> ParentClass;
};

template<typename TResultType>
FabricControl<TResultType>::FabricControl(BOOL loading)
	: FabricTranslationLayer< Control, TResultType >(loading)
{
	// Unfortunately, we cannot call ResetPorts here because
	// it is a virtual call (can only happen after child is constructed)
}

template<typename TResultType>
FabricControl<TResultType>::~FabricControl()
{

}


template<typename TResultType>
void FabricControl<TResultType>::ResetPorts()
{
	MACROREC_GUARD;

	m_parentArgName = "parentValue";
	AddFabricParameter(this, GetParentValueType(), m_parentArgName.c_str(), FabricCore::DFGPortType_In);
	SetPortMetaData(m_parentArgName.c_str(), FABRIC_UI_HIDDEN, "true", "");
	__super::ResetPorts();
}


template<typename TResultType>
IOResult FabricControl<TResultType>::SaveImpData(ISave* isave)
{
	isave->WriteCString(m_parentArgName.c_str());
	return IO_OK;
}


template<typename TResultType>
IOResult FabricControl<TResultType>::LoadImpData(ILoad* iload)
{
	char* parentName = nullptr;
	iload->ReadCStringChunk(&parentName);
	m_parentArgName = parentName;
	return IO_OK;
}

template<typename TResultType>
bool FabricControl<TResultType>::CloneFabricData( FabricTranslationLayer < Control, TResultType >* pMyClone)
{
  static_cast<FabricControl<TResultType>*>(pMyClone)->m_parentArgName = m_parentArgName;
  return true;
}
