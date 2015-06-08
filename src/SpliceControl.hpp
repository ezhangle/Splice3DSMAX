//////////////////////////////////////////////////////////////////////////
// Small class specializes a few functions especially for Control implementations
//
#pragma once

template<typename TResultType>
class SpliceControl : public SpliceTranslationLayer < Control, TResultType > {
protected:

	SpliceControl(BOOL loading);
	~SpliceControl();

	IOResult SaveImpData(ISave* isave) override;
	IOResult LoadImpData(ILoad* isave) override;

	bool CloneSpliceData(ParentClass* pMyClone) override { return true; }; // No cloning for me...

	// Handle parent value port
	DFGWrapper::ExecPortPtr m_parentValuePort;
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
IOResult SpliceControl<TResultType>::SaveImpData(ISave* isave)
{
	if (m_parentValuePort.isNull() && !m_parentValuePort->isValid())
		return IO_OK;

	const char* parentName = m_parentValuePort->getName();
	isave->WriteCString(parentName);
	return IO_OK;
}


template<typename TResultType>
IOResult SpliceControl<TResultType>::LoadImpData(ILoad* iload)
{
	char* parentName = nullptr;
	iload->ReadCStringChunk(&parentName);
	if (parentName != nullptr)
		m_parentValuePort = GetPort(parentName);
	return IO_OK;
}

