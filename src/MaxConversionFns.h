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
extern FabricCore::Variant GetVariant(const Point4& param, bool asColor=false);
extern FabricCore::Variant GetVariant(const Color& param);
extern FabricCore::Variant GetVariant(const Quat& param);
extern FabricCore::Variant GetVariant(const Matrix3& param);
extern FabricCore::Variant GetVariant(const MCHAR* param);
extern FabricCore::Variant GetVariant(const MSTR& param);
extern FabricCore::Variant GetVariant(const char* param);
extern FabricCore::Variant GetVariant(const FPValue& value);
// Annoyingly, if we don't have a conversion function for a type,
// some types get silently promoted to bool, and we call the wrong fn
template<typename T>
FabricCore::Variant GetVariant(const T& param) { ThisShouldNotCompile }

extern FabricCore::RTVal ConvertToRTVal(int param, FabricCore::RTVal val);
extern FabricCore::RTVal ConvertToRTVal(float param, FabricCore::RTVal val);
extern FabricCore::RTVal ConvertToRTVal(bool param, FabricCore::RTVal val);
extern FabricCore::RTVal ConvertToRTVal(const Point3& param, FabricCore::RTVal val);
extern FabricCore::RTVal ConvertToRTVal(const Point4& param, FabricCore::RTVal val, bool asColor=false);
extern FabricCore::RTVal ConvertToRTVal(const Color& param, FabricCore::RTVal val);
extern FabricCore::RTVal ConvertToRTVal(const Quat& param, FabricCore::RTVal val);
extern FabricCore::RTVal ConvertToRTVal(const Matrix3& param, FabricCore::RTVal val);
extern FabricCore::RTVal ConvertToRTVal(const MCHAR* param, FabricCore::RTVal val);
extern FabricCore::RTVal ConvertToRTVal(const MSTR& param, FabricCore::RTVal val);
extern FabricCore::RTVal ConvertToRTVal(const Mesh& param, FabricCore::RTVal val);

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
			FabricCore::RTVal aVal = spliceVal.getArrayElement(i);
			ConvertToRTVal(convert, aVal);
			spliceVal.setArrayElement(i, aVal);
		}
	}
	else
	{
		DbgAssert(nParams == 1);
		if (nParams > 0)
		{
			Convert(*params, t, ivValid, convert);
			FabricCore::RTVal aVal = dgPort.getRTVal();
			dgPort.setRTVal(ConvertToRTVal(convert, aVal));
		}
	}
	MAXSPLICE_CATCH_END()
}

template<typename TResultType>
void MaxValueToSplice(FabricSplice::DGPort& dgPort, TimeValue t, Interval& ivValid, const TResultType& param) { MaxValuesToSplice<TResultType, TResultType>(dgPort, t, ivValid, &param, 1); }

//////////////////////////////////////////////////////////////////////////////////////
// Splice->Max

/** Convert from Splice Variant to max type */
extern void SpliceToMaxValue(const FabricCore::Variant& spliceVal, int& param);
extern void SpliceToMaxValue(const FabricCore::Variant& spliceVal, float& param);
extern void SpliceToMaxValue(const FabricCore::Variant& spliceVal, bool& param);
extern void SpliceToMaxValue(const FabricCore::Variant& spliceVal, Point3& param);
extern void SpliceToMaxValue(const FabricCore::Variant& spliceVal, Point4& param);
extern void SpliceToMaxValue(const FabricCore::Variant& spliceVal, Color& param);
extern void SpliceToMaxValue(const FabricCore::Variant& spliceVal, Quat& param);
extern void SpliceToMaxValue(const FabricCore::Variant& spliceVal, Matrix3& param);
extern void SpliceToMaxValue(const FabricCore::Variant& spliceVal, MSTR& param);

// Annoyingly, if we don't have a conversion function for a type,
// some types get silently promoted to bool, and we call the wrong fn
template<typename TResultType>
void SpliceToMaxValue(const FabricCore::Variant& spliceVal, TResultType& maxVal) { ThisShouldNotCompile }

/** Get the value of the splice dgPort on the reference of param */
void SpliceToMaxValue(const FabricCore::RTVal& rtVal, int& param);
void SpliceToMaxValue(const FabricCore::RTVal& rtVal, float& param);
void SpliceToMaxValue(const FabricCore::RTVal& rtVal, Point3& param);
void SpliceToMaxValue(const FabricCore::RTVal& rtVal, Point4& param);
void SpliceToMaxValue(const FabricCore::RTVal& rtVal, Quat& param);
void SpliceToMaxValue(const FabricCore::RTVal& dgPort, Matrix3& param);
void SpliceToMaxValue(const FabricCore::RTVal& rtv, Mesh& param);
	// Annoyingly, if we don't have a conversion function for a type,
	// some types get silently promoted to bool, and we call the wrong fn
template<typename TResultType>
void SpliceToMaxValue(const FabricCore::RTVal& spliceVal, TResultType& maxVal)  { ThisShouldNotCompile }

//////////////////////////////////////////////////////////////////////////
template<typename TResultType>
void SpliceToMaxValue(const FabricSplice::DGPort& dgPort, TResultType& param, int index=-1)
{
	// De-const because some splice functions are not marked const
	FabricSplice::DGPort& ncDGPort = const_cast<FabricSplice::DGPort&>(dgPort);
	FabricCore::RTVal rtVal = ncDGPort.getRTVal();
	if (ncDGPort.isArray())
	{
		DbgAssert(index >= 0);
		int nValues = ncDGPort.getArrayCount();
		if (index < nValues && index >= 0)
		{
			rtVal = rtVal.getArrayElement(index);
		}
	}
	if (rtVal.isValid())
		SpliceToMaxValue(rtVal, param);
}
