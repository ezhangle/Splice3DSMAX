#pragma once

#include "Fabric3dsmax.h"

//////////////////////////////////////////////////////////////////////////////////////
// Max->Fabric


// This file defines conversion structs that can be sent to the Max->Fabric functions
// to modify the values gotten from max before being sent to Fabric
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

extern void ConvertToRTVal(int param, FabricCore::RTVal& val);
extern void ConvertToRTVal(float param, FabricCore::RTVal& val);
extern void ConvertToRTVal(bool param, FabricCore::RTVal& val);
extern void ConvertToRTVal(const Point3& param, FabricCore::RTVal& val);
extern void ConvertToRTVal(const Point4& param, FabricCore::RTVal& val);
extern void ConvertToRTVal(const Color& param, FabricCore::RTVal& val);
extern void ConvertToRTVal(const Quat& param, FabricCore::RTVal& val);
extern void ConvertToRTVal(const Matrix3& param, FabricCore::RTVal& val);
extern void ConvertToRTVal(const MCHAR* param, FabricCore::RTVal& val);
extern void ConvertToRTVal(const MSTR& param, FabricCore::RTVal& val);
extern void ConvertToRTVal(const Mesh& param, FabricCore::RTVal& val);
extern void ConvertToRTVal(const FPValue& param, FabricCore::RTVal& val);
// Annoyingly, if we don't have a conversion function for a type,
// some types get silently promoted to bool, and we call the wrong fn
template<typename T>
void ConvertToRTVal(const T& param, FabricCore::RTVal& val) { ThisShouldNotCompile }

// Entry point to the Max->Fabric value translation
template<typename TResultType, typename TConvertType>
void MaxValuesToFabric(FabricCore::DFGBinding & binding, const char* argName, TimeValue t, Interval& ivValid, const TResultType* params, int nParams)
{
	if (!binding.getExec().haveExecPort(argName))
		return;

	MAXSPLICE_CATCH_BEGIN

	// Initialize handle to splice values
	//FabricCore::Variant spliceVal;
	TConvertType convert;

	// Is resetting the value necessary?
	bool canUndo = theHold.Holding();

	FabricCore::RTVal rtVal = binding.getArgValue(argName);
	if (rtVal.isArray())
	{
		rtVal.setArraySize(nParams);
		for (int i = 0; i < nParams; i++)
		{
			Convert(params[i], t, ivValid, convert);
			FabricCore::RTVal aVal = rtVal.getArrayElement(i);
			ConvertToRTVal(convert, aVal);
			rtVal.setArrayElement(i, aVal);
		}
	}
	else
	{
		DbgAssert(nParams == 1);
		{
			Convert(*params, t, ivValid, convert);
			ConvertToRTVal(convert, rtVal);
		}
	}
	binding.setArgValue(argName, rtVal, canUndo);

	// Is resetting the value necessary?
	if (canUndo)
		theHold.Put(new FabricCoreRestoreObj());

	MAXSPLICE_CATCH_END
}

template<typename TResultType>
void MaxValueToFabric(FabricCore::DFGBinding& binding, const char* argName, TimeValue t, Interval& ivValid, const TResultType& param) { MaxValuesToFabric<TResultType, TResultType>(binding, argName, t, ivValid, &param, 1); }

//////////////////////////////////////////////////////////////////////////////////////
// Fabric->Max

/** Convert from Fabric Variant to max type */
extern void FabricToMaxValue(const FabricCore::Variant& spliceVal, int& param);
extern void FabricToMaxValue(const FabricCore::Variant& spliceVal, float& param);
extern void FabricToMaxValue(const FabricCore::Variant& spliceVal, bool& param);
extern void FabricToMaxValue(const FabricCore::Variant& spliceVal, Point3& param);
extern void FabricToMaxValue(const FabricCore::Variant& spliceVal, Point4& param);
extern void FabricToMaxValue(const FabricCore::Variant& spliceVal, Color& param);
extern void FabricToMaxValue(const FabricCore::Variant& spliceVal, Quat& param);
extern void FabricToMaxValue(const FabricCore::Variant& spliceVal, Matrix3& param);
extern void FabricToMaxValue(const FabricCore::Variant& spliceVal, MSTR& param);

// Annoyingly, if we don't have a conversion function for a type,
// some types get silently promoted to bool, and we call the wrong fn
template<typename TResultType>
void FabricToMaxValue(const FabricCore::Variant& spliceVal, TResultType& maxVal) { ThisShouldNotCompile }

/** Get the value of the splice dgPort on the reference of param */
void FabricToMaxValue(const FabricCore::RTVal& rtVal, int& param);
void FabricToMaxValue(const FabricCore::RTVal& rtVal, bool& param);
void FabricToMaxValue(const FabricCore::RTVal& rtVal, float& param);
void FabricToMaxValue(const FabricCore::RTVal& rtVal, Point2& param);
void FabricToMaxValue(const FabricCore::RTVal& rtVal, Point3& param);
void FabricToMaxValue(const FabricCore::RTVal& rtVal, Point4& param);
void FabricToMaxValue(const FabricCore::RTVal& rtVal, Color& param);
void FabricToMaxValue(const FabricCore::RTVal& rtVal, Quat& param);
void FabricToMaxValue(const FabricCore::RTVal& dgPort, Matrix3& param);
void FabricToMaxValue(const FabricCore::RTVal& rtv, Mesh& param);
void FabricToMaxValue(const FabricCore::RTVal& rtv, MSTR& param);
	// Annoyingly, if we don't have a conversion function for a type,
	// some types get silently promoted to bool, and we call the wrong fn
template<typename TResultType>
void FabricToMaxValue(const FabricCore::RTVal& spliceVal, TResultType& maxVal)  { ThisShouldNotCompile }