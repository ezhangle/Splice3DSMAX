#pragma once

#include "Splice3dsmax.h"

//////////////////////////////////////////////////////////////////////////////////////
// Max->Splice


// This file defines conversion structs that can be sent to the Max->Splice functions
// to modify the values gotten from max before being sent to Splice
template<typename TInType, typename TOutType>
void Convert(TInType& in, TimeValue /*t*/, Interval& /*ivValid*/, TOutType& out) { out = in; }

// The following functions provide methods for converting between types
extern void Convert(TimeValue in, TimeValue /*t*/, Interval& /*ivValid*/, float& out); 
extern void Convert(BOOL b, TimeValue /*t*/, Interval& /*ivValid*/, bool& out); 
extern void Convert(INode* node, TimeValue t, Interval& ivValid, Point3& out);;
extern void Convert(INode* node, TimeValue t, Interval& ivValid, Quat out);;
extern void Convert(INode* node, TimeValue t, Interval& ivValid, Matrix3& out);;
extern void Convert(Object* pObject, TimeValue t, Interval& ivValid, Mesh& out);;
extern void Convert(INode* pNode, TimeValue t, Interval& ivValid, Mesh& out);

extern FabricCore::Variant GetVariant(int param);
extern FabricCore::Variant GetVariant(float param);
extern FabricCore::Variant GetVariant(bool param);
extern FabricCore::Variant GetVariant(const Point3& param);
extern FabricCore::Variant GetVariant(const Point4& param);
extern FabricCore::Variant GetVariant(const Color& param);
extern FabricCore::Variant GetVariant(const Quat& param);
extern FabricCore::Variant GetVariant(const Matrix3& param);
extern FabricCore::Variant GetVariant(const MCHAR* param);
extern FabricCore::Variant GetVariant(const MSTR& param);
extern FabricCore::Variant GetVariant(const char* param);
// Annoyingly, if we don't have a conversion function for a type,
// some types get silently promoted to bool, and we call the wrong fn
template<typename T>
FabricCore::Variant GetVariant(const T& param) { ThisShouldNotCompile }

extern FabricCore::RTVal GetRTVal(int param);
extern FabricCore::RTVal GetRTVal(float param);
extern FabricCore::RTVal GetRTVal(bool param);
extern FabricCore::RTVal GetRTVal(const Point3& param);
extern FabricCore::RTVal GetRTVal(const Point4& param);
extern FabricCore::RTVal GetRTVal(const Color& param);
extern FabricCore::RTVal GetRTVal(const Quat& param);
extern FabricCore::RTVal GetRTVal(const Matrix3& param);
extern FabricCore::RTVal GetRTVal(const MCHAR* param);
extern FabricCore::RTVal GetRTVal(const MSTR& param);
extern FabricCore::RTVal GetRTVal(const Mesh& param);

// Entry point to the Max->Splice value translation
template<typename TResultType, typename TConvertType>
void MaxValuesToSplice(FabricSplice::DGPort& dgPort, TimeValue t, Interval& ivValid, const TResultType* params, int nParams)
{

	if (!dgPort.isValid())
		return;

	MAXSPLICE_CATCH_BEGIN()

	// Initialize handle to splice values
	//FabricCore::Variant spliceVal;
	TConvertType convert;
	if (dgPort.isArray())
	{
		FabricCore::RTVal spliceVal = dgPort.getRTVal();
		spliceVal.setArraySize(nParams);
		for (int i = 0; i < nParams; i++)
		{
			Convert(params[i], t, ivValid, convert);
			FabricCore::RTVal aVal = GetRTVal(convert);
			spliceVal.setArrayElement(i, aVal);
		}
	}
	else
	{
		DbgAssert(nParams == 1);
		if (nParams > 0)
		{
			Convert(*params, t, ivValid, convert);
			dgPort.setRTVal(GetRTVal(convert));
		}
	}
	MAXSPLICE_CATCH_END()
}

template<typename TResultType>
void MaxValueToSplice(FabricSplice::DGPort& dgPort, TimeValue t, Interval& ivValid, const TResultType& param) { MaxValuesToSplice<TResultType, TResultType>(dgPort, t, ivValid, &param, 1); }

//////////////////////////////////////////////////////////////////////////////////////
// Splice->Max

/** Get the value of the splice dgPort on the reference of param */
template<typename TResultType>
void SpliceToMaxValue(FabricCore::RTVal& spliceVal, TResultType& maxVal);

template<typename TResultType>
void SpliceToMaxValue(FabricSplice::DGPort& dgPort, TResultType& param, int index=-1)
{
	FabricCore::RTVal rtVal = dgPort.getRTVal();
	if (dgPort.isArray())
	{
		DbgAssert(index >= 0);
		int nValues = dgPort.getArrayCount();
		if (index < nValues && index >= 0)
		{
			rtVal = rtVal.getArrayElement(index);
		}
	}
	if (rtVal.isValid())
		SpliceToMaxValue(rtVal, param);
}
