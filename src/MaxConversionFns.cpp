#include "StdAfx.h"
#include "MaxConversionFns.h"
#include "MeshNormalSpec.h"

#pragma region Convert functions allow modifying a value just before it is sent to Splice

void Convert( TimeValue in, TimeValue /*t*/, Interval& /*ivValid*/, float& out )
{
	out = TicksToSec(in);
}

void Convert( BOOL b, TimeValue /*t*/, Interval& /*ivValid*/, bool& out )
{
	out = b != FALSE;
}

void Convert( INode* node, TimeValue t, Interval& ivValid, Point3& out )
{
	if (node != nullptr) 
		out = node->GetNodeTM(t, &ivValid).GetTrans();
	else
		out.Set(0, 0, 0);
}

void Convert( INode* node, TimeValue t, Interval& ivValid, Quat out )
{
	if (node != nullptr) 
		out = Quat(node->GetNodeTM(t, &ivValid));
	out.Identity();
}

void Convert( INode* node, TimeValue t, Interval& ivValid, Matrix3& out )
{
	if (node != nullptr) 
		out = node->GetNodeTM(t, &ivValid);
	else
		out.IdentityMatrix();
}

void Convert( Object* pObject, TimeValue t, Interval& ivValid, Mesh& out )
{
	if (pObject != nullptr)
	{
		TriObject* pTriObject = static_cast<TriObject*>(pObject->ConvertToType(t, triObjectClassID));
		if (pTriObject != NULL)
		{
			out = pTriObject->GetMesh();
			if (pTriObject != pObject)
				pTriObject->MaybeAutoDelete();
		}
		ivValid = pObject->ObjectValidity(t);
	}
}

void Convert( INode* pNode, TimeValue t, Interval& ivValid, Mesh& out )
{
	if (pNode != nullptr)
	{
		// Convert to mesh if possible
		ObjectState os = pNode->EvalWorldState(t);
		Convert(os.obj, t, ivValid, out);
	}
}

#pragma endregion

// GetVariant implements the base functionality
// of converting Max bytes to Splice bytes
#pragma region GetVariants
FabricCore::Variant GetVariant(int param)
{
	return FabricCore::Variant::CreateSInt32(param);
}

FabricCore::Variant GetVariant(float param)
{
	return FabricCore::Variant::CreateFloat32(param);
}

FabricCore::Variant GetVariant(bool param)
{
	return FabricCore::Variant::CreateBoolean(param);
}

FabricCore::Variant GetVariant(const Point3& param)
{
	FabricCore::Variant vec3 = FabricCore::Variant::CreateDict();
	vec3.setDictValue("x", FabricCore::Variant::CreateFloat32(param.x));
	vec3.setDictValue("y", FabricCore::Variant::CreateFloat32(param.y));
	vec3.setDictValue("z", FabricCore::Variant::CreateFloat32(param.z));
	return vec3;
}

FabricCore::Variant GetVariant(const Color& param)
{
	FabricCore::Variant color = FabricCore::Variant::CreateDict();
	color.setDictValue("a", FabricCore::Variant::CreateUInt16(128));
	color.setDictValue("r", FabricCore::Variant::CreateUInt16(uint16_t(param.r * 128)));
	color.setDictValue("g", FabricCore::Variant::CreateUInt16(uint16_t(param.g * 128)));
	color.setDictValue("b", FabricCore::Variant::CreateUInt16(uint16_t(param.b * 128)));
	return color;
}

FabricCore::Variant GetVariant(const Point4& param)
{
	FabricCore::Variant vec3 = FabricCore::Variant::CreateDict();
	vec3.setDictValue("x", FabricCore::Variant::CreateFloat32(param.x));
	vec3.setDictValue("y", FabricCore::Variant::CreateFloat32(param.y));
	vec3.setDictValue("z", FabricCore::Variant::CreateFloat32(param.z));
	vec3.setDictValue("w", FabricCore::Variant::CreateFloat32(param.w));
	return vec3;
}

FabricCore::Variant GetVariant(const Quat& param)
{
	FabricCore::Variant quat = FabricCore::Variant::CreateDict();
	FabricCore::Variant quatAxis = FabricCore::Variant::CreateDict();
	quatAxis.setDictValue("x", FabricCore::Variant::CreateFloat64(param.x));
	quatAxis.setDictValue("y", FabricCore::Variant::CreateFloat64(param.y));
	quatAxis.setDictValue("z", FabricCore::Variant::CreateFloat64(param.z));
	quat.setDictValue("v", quatAxis);
	quat.setDictValue("w", FabricCore::Variant::CreateFloat64(param.w));
	return quat;
}

FabricCore::Variant GetVariant(const Matrix3& param)
{
	const MRow* pInMtx = param.GetAddr();
	FabricCore::Variant spliceMat = FabricCore::Variant::CreateDict();
	FabricCore::Variant spliceMatRow = FabricCore::Variant::CreateDict();

	spliceMatRow.setDictValue("x", FabricCore::Variant::CreateFloat64(pInMtx[0][0]));
	spliceMatRow.setDictValue("y", FabricCore::Variant::CreateFloat64(pInMtx[1][0]));
	spliceMatRow.setDictValue("z", FabricCore::Variant::CreateFloat64(pInMtx[2][0]));
	spliceMatRow.setDictValue("t", FabricCore::Variant::CreateFloat64(pInMtx[3][0]));
	spliceMat.setDictValue("row0", spliceMatRow);

	spliceMatRow.setDictValue("x", FabricCore::Variant::CreateFloat64(pInMtx[0][1]));
	spliceMatRow.setDictValue("y", FabricCore::Variant::CreateFloat64(pInMtx[1][1]));
	spliceMatRow.setDictValue("z", FabricCore::Variant::CreateFloat64(pInMtx[2][1]));
	spliceMatRow.setDictValue("t", FabricCore::Variant::CreateFloat64(pInMtx[3][1]));
	spliceMat.setDictValue("row1", spliceMatRow);

	spliceMatRow.setDictValue("x", FabricCore::Variant::CreateFloat64(pInMtx[0][2]));
	spliceMatRow.setDictValue("y", FabricCore::Variant::CreateFloat64(pInMtx[1][2]));
	spliceMatRow.setDictValue("z", FabricCore::Variant::CreateFloat64(pInMtx[2][2]));
	spliceMatRow.setDictValue("t", FabricCore::Variant::CreateFloat64(pInMtx[3][2]));
	spliceMat.setDictValue("row2", spliceMatRow);

	spliceMatRow.setDictValue("x", FabricCore::Variant::CreateFloat64(0));
	spliceMatRow.setDictValue("y", FabricCore::Variant::CreateFloat64(0));
	spliceMatRow.setDictValue("z", FabricCore::Variant::CreateFloat64(0));
	spliceMatRow.setDictValue("t", FabricCore::Variant::CreateFloat64(1));
	spliceMat.setDictValue("row3", spliceMatRow);

	return spliceMat;
}


FabricCore::Variant GetVariant(const MSTR& param)
{
	CStr cStr = param.ToCStr();
	return FabricCore::Variant::CreateString(cStr.data());
}

FabricCore::Variant GetVariant(const MCHAR* param)
{
	CStr cStr = CStr::FromMCHAR(param);
	return FabricCore::Variant::CreateString(cStr.data());
}

FabricCore::Variant GetVariant(const char* param)
{
	return FabricCore::Variant::CreateString(param);
}

extern FabricCore::Variant GetVariant(const FPValue& value)
{
	if (is_tab(value.type)) 
	{
		// For the sake of getting the count, the memory structure of every tab is the same
		int size = value.i_tab->Count();
		FabricCore::Variant res = FabricCore::Variant::CreateArray(size);
		for (int i = 0; i < size; i++) 
		{
			//res.setElementTake(i, GetVariant())
		}
		return res;
	}
	else 
	{
		switch ((int)value.type)
		{
		case TYPE_BOOL:
			return GetVariant(value.b); 
		case TYPE_INT:
			return GetVariant(value.i);
		case TYPE_FLOAT:
		case TYPE_ANGLE:
		case TYPE_WORLD:
		case TYPE_PCNT_FRAC:
			return GetVariant(value.f);
		case TYPE_RGBA:
			return GetVariant(*value.clr);
		case TYPE_POINT3:
			return GetVariant(*value.p);
		case TYPE_POINT4:
			return GetVariant(*value.p4);
		case TYPE_QUAT:
			return GetVariant(*value.q);
		case TYPE_TSTR:
			return GetVariant(*value.tstr);
		default:
			return FabricCore::Variant();
		}
	}
}

#pragma endregion // GetVariants

#pragma region Get RTVals
FabricCore::RTVal ConvertToRTVal(int param, FabricCore::RTVal val)
{
	val = FabricSplice::constructSInt32RTVal(param);
	return val;
}

FabricCore::RTVal ConvertToRTVal(float param, FabricCore::RTVal val)
{
	val = FabricSplice::constructFloat32RTVal(param);
	return val;
}

FabricCore::RTVal ConvertToRTVal(bool param, FabricCore::RTVal val)
{
	val = FabricSplice::constructBooleanRTVal(param);
	return val;
}

FabricCore::RTVal ConvertToRTVal(const Point3& param, FabricCore::RTVal val)
{
	if(!val.isValid())
		val = FabricSplice::constructRTVal("Vec3");
	val.setMember("x", FabricSplice::constructFloat32RTVal(param.x));
	val.setMember("y", FabricSplice::constructFloat32RTVal(param.y));
	val.setMember("z", FabricSplice::constructFloat32RTVal(param.z));
	return val;
}

FabricCore::RTVal ConvertToRTVal(const Color& param, FabricCore::RTVal val)
{
	if(!val.isValid())
		val = FabricSplice::constructRTVal("Color");
	val.setMember("a", FabricSplice::constructUInt16RTVal(128));
	val.setMember("r", FabricSplice::constructUInt16RTVal(uint16_t(param.r * 128)));
	val.setMember("g", FabricSplice::constructUInt16RTVal(uint16_t(param.g * 128)));
	val.setMember("b", FabricSplice::constructUInt16RTVal(uint16_t(param.b * 128)));
	return val;
}

FabricCore::RTVal ConvertToRTVal(const Point4& param, FabricCore::RTVal val)
{
	if(!val.isValid())
		val = FabricSplice::constructRTVal("Vec4");
	val.setMember("x", FabricSplice::constructFloat32RTVal(param.x));
	val.setMember("y", FabricSplice::constructFloat32RTVal(param.y));
	val.setMember("z", FabricSplice::constructFloat32RTVal(param.z));
	val.setMember("t", FabricSplice::constructFloat32RTVal(param.w));
	return val;
}

FabricCore::RTVal ConvertToRTVal(const Quat& param, FabricCore::RTVal val)
{
    if(!val.isValid())
		val = FabricSplice::constructRTVal("Quat");

	val.setMember("v", ConvertToRTVal(Point3(param.x, param.y, param.z), val.maybeGetMember("v")));
	val.setMember("w", FabricSplice::constructFloat32RTVal(param.w));
	return val;
}

FabricCore::RTVal ConvertToRTVal(const Matrix3& param, FabricCore::RTVal val)
{
    if(!val.isValid())
		val = FabricSplice::constructRTVal("Mat44");

	const MRow* pInMtx = param.GetAddr();

	val.setMember("row0", ConvertToRTVal(Point4(pInMtx[0][0], pInMtx[1][0], pInMtx[2][0], pInMtx[3][0]), val.maybeGetMember("row0")));
	val.setMember("row1", ConvertToRTVal(Point4(pInMtx[0][1], pInMtx[1][1], pInMtx[2][1], pInMtx[3][1]), val.maybeGetMember("row1")));
	val.setMember("row2", ConvertToRTVal(Point4(pInMtx[0][2], pInMtx[1][2], pInMtx[2][2], pInMtx[3][2]), val.maybeGetMember("row2")));
	val.setMember("row3", ConvertToRTVal(Point4(0, 0, 0, 1), val.maybeGetMember("row3")));
	return val;
}


FabricCore::RTVal ConvertToRTVal(const MSTR& param, FabricCore::RTVal val)
{
	CStr cStr = param.ToCStr();
	val = FabricSplice::constructStringRTVal(cStr.data());
	return val;
}

FabricCore::RTVal ConvertToRTVal(const MCHAR* param, FabricCore::RTVal val)
{
	CStr cStr = CStr::FromMCHAR(param);
	val = FabricSplice::constructStringRTVal(cStr.data());
	return val;
}

FabricCore::RTVal ConvertToRTVal(const Mesh& param, FabricCore::RTVal rtMesh)
{
    if(!rtMesh.isValid())
		rtMesh = FabricSplice::constructRTVal("PolygonMesh");
	
	if(!rtMesh.isValid() || rtMesh.isNullObject())
		return rtMesh;
	
	UINT nbPolygons = rtMesh.callMethod("UInt32", "polygonCount", 0, 0).getUInt32();
	bool rebuildMesh = nbPolygons != (UINT)param.numFaces;
	if(rebuildMesh){
		rtMesh.callMethod("", "clear", 0, 0);
	}

	// Send all vertices to Splice
	std::vector<FabricCore::RTVal> args(2);
	args[0] = FabricSplice::constructExternalArrayRTVal("Float32", param.numVerts * 3, param.verts);
	args[1] = FabricSplice::constructUInt32RTVal(3); // components
	rtMesh.callMethod("", "setPointsFromExternalArray", 2, &args[0]);

	if(rebuildMesh){
		// Send all indices to Splice
		// First, construct an array of polygon sizes
		// We only support triangles, so all sizes are 3
		std::vector<unsigned int> dPolyCounts;
		dPolyCounts.assign(param.numFaces, 3);

		// Construct array of indices
		std::vector<unsigned int> dVertIndex;
		dPolyCounts.reserve(param.numFaces * 3);
		Face* pFaces = param.faces;
		for (int f = 0; f < param.numFaces; f++)
		{
			for (int i = 0; i < 3; i++)
				dVertIndex.push_back(pFaces[f].getVert(i));
		}
	

		args[0] = FabricSplice::constructExternalArrayRTVal("UInt32", dPolyCounts.size(), &dPolyCounts[0]);
		args[1] = FabricSplice::constructExternalArrayRTVal("UInt32", dVertIndex.size(), &dVertIndex[0]);
		rtMesh.callMethod("", "setTopologyFromCountsIndicesExternalArrays", 2, &args[0]);
	}

	// Set normals
	// Do we have specified normals?
	Mesh* pNonConstMesh = const_cast<Mesh*>(&param);
	MeshNormalSpec* pNormalSpec = pNonConstMesh->GetSpecifiedNormals();
#pragma message("TODO: Fix this!")
	if (false && pNormalSpec != NULL && pNormalSpec->GetNumNormals() > 0)
	{
		std::vector<FabricCore::RTVal> args(3);
		args[0] = FabricSplice::constructStringRTVal("normals");

		// 1 normal per index
		int nNormals = pNormalSpec->GetNumNormals();
		Point3* pNormalData = pNormalSpec->GetNormalArray();
		std::vector<double> dNormalsDbls(nNormals * 3);
		for (int i = 0; i < nNormals; i++)
		{
			for (int j = 0; j < 3; j++)
				dNormalsDbls[(i * 3) + j] = pNormalData[i][j];
		}
		args[1] = FabricSplice::constructExternalArrayRTVal("Vec3", nNormals, &dNormalsDbls[0]);

		std::vector<UINT64> dNormalIndices;
		int nFaces = pNormalSpec->GetNumFaces();
		dNormalIndices.resize(nFaces * 3);
		MeshNormalFace* pFaces = pNormalSpec->GetFaceArray();
		for (int f = 0; f < nFaces; f++)
		{
			for (int j = 0; j < 3; j++)
				dNormalIndices[f* 3 + j] = pFaces[f].GetNormalID(j);
			//			memcpy(&dNormalIndices[f*3], pFaces[f].GetNormalIDArray(), sizeof(int) * 3);
		}
		args[2] = FabricSplice::constructExternalArrayRTVal("UInt64", nFaces * 3, &dNormalIndices[0]);
		rtMesh.callMethod("", "setAttributeFromPolygonPackedData", 3, &args[0]);
	}
	else
	{
		rtMesh.callMethod("", "recomputePointNormals", 0, NULL);

		std::vector<Point3> dNormals;
		dNormals.resize(param.numFaces);
		for (int i = 0; i < param.numFaces; i++)
			dNormals[i] = pNonConstMesh->getFaceNormal(i);
	}

	return rtMesh;
}

#pragma endregion // GetVariants


#pragma region Splice to Max

template<>
void SpliceToMaxValue(FabricCore::RTVal& rtVal, int& param)
{
	param = rtVal.getSInt32();
}

template<>
void SpliceToMaxValue(FabricCore::RTVal& rtVal, float& param)
{
	param = static_cast<float>(rtVal.getFloat32());
}

template<>
void SpliceToMaxValue(FabricCore::RTVal& rtVal, Point3& param)
{
	param[0] = rtVal.maybeGetMemberRef("x").getFloat32();
	param[1] = rtVal.maybeGetMemberRef("y").getFloat32();
	param[2] = rtVal.maybeGetMemberRef("z").getFloat32();
}

template<>
void SpliceToMaxValue(FabricCore::RTVal& rtVal, Point4& param)
{
	param[0] = rtVal.maybeGetMemberRef("x").getFloat32();
	param[1] = rtVal.maybeGetMemberRef("y").getFloat32();
	param[2] = rtVal.maybeGetMemberRef("z").getFloat32();
	param[3] = rtVal.maybeGetMemberRef("t").getFloat32();
}

template<>
void SpliceToMaxValue(FabricCore::RTVal& rtVal, Quat& param)
{
	// Fabric Quats are structured ( Vec3 v; Scalar w; )
	FabricCore::RTVal rtV = rtVal.maybeGetMemberRef("v");
	Point3 maxV;
	SpliceToMaxValue(rtV, maxV);
	float w = rtVal.maybeGetMemberRef("w").getFloat32();
	param.Set(maxV, w);
	// When converting from Splices Y up to Max's Z up, our
	// rotations come out -ve.  Invert the Quat, and we 
	// get equivalent rotations (tested via euler rot)
	param.Invert();
}

template<>
void SpliceToMaxValue(FabricCore::RTVal& dgPort, Matrix3& param)
{
	FabricCore::RTVal pRow0 = dgPort.maybeGetMemberRef("row0");
	FabricCore::RTVal pRow1 = dgPort.maybeGetMemberRef("row1");
	FabricCore::RTVal pRow2 = dgPort.maybeGetMemberRef("row2");
	
	if (!pRow0.isValid() ||
		!pRow1.isValid() ||
		!pRow2.isValid())
	{
		return;
	}

	Point4 columns[3];
	SpliceToMaxValue(pRow0, columns[0]);
	SpliceToMaxValue(pRow1, columns[1]);
	SpliceToMaxValue(pRow2, columns[2]);

	param.SetColumn(0, columns[0]);
	param.SetColumn(1, columns[1]);
	param.SetColumn(2, columns[2]);
/*
	pMaxRow[0][0] = (float)pRow0.maybeGetMemberRef("x").getFloat32();
	pMaxRow[1][0] = (float)pRow0.maybeGetMemberRef("y").getFloat32();
	pMaxRow[2][0] = (float)pRow0.maybeGetMemberRef("z").getFloat32();
	pMaxRow[3][0] = (float)pRow0.maybeGetMemberRef("t").getFloat32();

	pMaxRow[0][1] = (float)pRow1.maybeGetMemberRef("x").getFloat32();
	pMaxRow[1][1] = (float)pRow1.maybeGetMemberRef("y").getFloat32();
	pMaxRow[2][1] = (float)pRow1.maybeGetMemberRef("z").getFloat32();
	pMaxRow[3][1] = (float)pRow1.maybeGetMemberRef("t").getFloat32();

	pMaxRow[0][2] = (float)pRow2.maybeGetMemberRef("x").getFloat32();
	pMaxRow[1][2] = (float)pRow2.maybeGetMemberRef("y").getFloat32();
	pMaxRow[2][2] = (float)pRow2.maybeGetMemberRef("z").getFloat32();
	pMaxRow[3][2] = (float)pRow2.maybeGetMemberRef("t").getFloat32();*/

	param.ValidateFlags();
}

template<>
void SpliceToMaxValue(FabricCore::RTVal& rtMesh, Mesh& param)
{
	// Cache version number, compare for purposes of caching.
	// this line was causing asserts. 
	//UINT32 version = rtMesh.maybeGetMemberRef("version").getUInt32();

	UINT nbPoints = rtMesh.callMethod("UInt32", "pointCount", 0, 0).getUInt32();
	UINT nbPolygons = rtMesh.callMethod("UInt32", "polygonCount", 0, 0).getUInt32();
	UINT nbIndices = rtMesh.callMethod("UInt32", "polygonPointsCount", 0, 0).getUInt32();
	bool bHasUVs = rtMesh.callMethod("Boolean", "hasUVs", 0, 0).getBoolean();

	if (nbPoints != int(nbPoints)) // try to catch overflow
		return;

	param.setNumVerts(nbPoints);
	// If we have an invalid mesh, simply bail (TODO: log error)
	if (nbPoints == 0 || nbPolygons == 0 || nbIndices == 0)
	{
		// Ensure we have no left-over faces to corrupt this
		param.setNumFaces(0);
		return;
	}

	// Get vertices from rtMesh
	{
		std::vector<FabricCore::RTVal> args(2);
		args[0] = FabricSplice::constructExternalArrayRTVal("Float32", nbPoints * 3, param.verts);
		args[1] = FabricSplice::constructUInt32RTVal(3); // components
		rtMesh.callMethod("", "getPointsAsExternalArray", 2, &args[0]);
	}

	// Get normals from rtMesh
	Tab<Point3> rtNormals;
	param.SpecifyNormals();
	MeshNormalSpec* pNormalSpec = param.GetSpecifiedNormals();
	rtNormals.SetCount(nbIndices);
	{
		std::vector<FabricCore::RTVal> args(1);
		args[0] = FabricSplice::constructExternalArrayRTVal("Float32", nbIndices * 3, rtNormals.Addr(0));
		rtMesh.callMethod("", "getNormalsAsExternalArray", 1, &args[0]);
	}

	// Init UV's
	if(bHasUVs)
	{
		// Tell our mesh that yes, it does support mapping
		param.setMapSupport(1);

		Tab<Point2> uvs;
		uvs.SetCount(nbIndices);
		std::vector<FabricCore::RTVal> args(2);
		args[0] = FabricSplice::constructExternalArrayRTVal("Float32", uvs.Count() * 2, uvs.Addr(0));
		args[1] = FabricSplice::constructUInt32RTVal(2); // components
		rtMesh.callMethod("", "getUVsAsExternalArray", 2, &args[0]);

		param.setNumTVerts(nbIndices);
		for (size_t i = 0; i < nbIndices; i++)
			param.setTVert(i, uvs[i].x, uvs[i].y, 0.0f);
	}

	// Get topology from rtMesh
	Tab<UINT32> faceVtxCount;
	Tab<UINT32> indices;
	faceVtxCount.SetCount(nbPolygons);
	indices.SetCount(nbIndices);
	{
		std::vector<FabricCore::RTVal> args(2);
		args[0] = FabricSplice::constructExternalArrayRTVal("UInt32", nbPolygons, faceVtxCount.Addr(0));
		args[1] = FabricSplice::constructExternalArrayRTVal("UInt32", nbIndices, indices.Addr(0));
		rtMesh.callMethod("", "getTopologyAsCountsIndicesExternalArrays", 2, &args[0]);
	}
	//////////////////////////////////////////////////////////////////////////
	// Begin conversion, pushing values from rtMesh to our max Mesh
	// We will convert from polygons to tri's, so count
	// the number of tri's we will need
	UINT nTriFaces = 0;
	for (UINT i = 0; i < nbPolygons; i++)
		nTriFaces += max(faceVtxCount[i] - 2, 0);

	// Sanity
	if (nTriFaces == 0)
		return;

	// Set triangle indices.  We need to triangulate 
	// N-order poly's from splice in this step
	param.setNumFaces(nTriFaces);
	// 1 normal per index
	pNormalSpec->SetNumNormals(nbIndices);
	pNormalSpec->SetNumFaces(nTriFaces);
	MeshNormalFace* pNFaces = pNormalSpec->GetFaceArray();

	// Copy in normals
	memcpy(pNormalSpec->GetNormalArray(), &rtNormals[0], sizeof(Point3) * nbIndices);

	// Iterate over poly mesh, split to triangles
	UINT iFaceIdx = 0;
	UINT zeroIdxForFace = 0;
	for (UINT polyIdx = 0; polyIdx < nbPolygons; polyIdx++)
	{
		UINT nVerts = faceVtxCount[polyIdx];
		// We create a new triangle for
		// every 3 + i verts in the poly
		for (UINT triIdx = 0; triIdx < nVerts - 2; triIdx++)
		{
			Face& f = param.faces[iFaceIdx];
			MeshNormalFace& nf = pNFaces[iFaceIdx];

			if (triIdx > 0)
			{
				// For quads (and higher) hide edge between adjoining tri's
				param.faces[iFaceIdx - 1].setEdgeVis(2, EDGE_INVIS);
				f.setEdgeVis(2, EDGE_INVIS);
			}
			if (triIdx % 2 == 0)
			{
				// For even tri's, triangulate A - B - C
				f.setVerts(
					indices[zeroIdxForFace], 
					indices[zeroIdxForFace + (triIdx + 1)], 
					indices[zeroIdxForFace + (triIdx + 2)]
				);
				nf.SetNormalID(0, zeroIdxForFace);
				nf.SetNormalID(1, zeroIdxForFace + (triIdx + 1));
				nf.SetNormalID(2, zeroIdxForFace + (triIdx + 2));

				if (param.tvFace != NULL)
				{
					TVFace& tface = param.tvFace[iFaceIdx];
					tface.setTVerts(
						zeroIdxForFace,
						zeroIdxForFace + (triIdx + 1),
						zeroIdxForFace + (triIdx + 2)
						);
				}
			}
			else
			{
				// for odd tri's, triangulate B - C - A
				f.setVerts(
					indices[zeroIdxForFace + (triIdx + 1)],
					indices[zeroIdxForFace + (triIdx + 2)],
					indices[zeroIdxForFace]
				);

				nf.SetNormalID(0, zeroIdxForFace + (triIdx + 1));
				nf.SetNormalID(1, zeroIdxForFace + (triIdx + 2));
				nf.SetNormalID(2, zeroIdxForFace);

				if (param.tvFace != NULL)
				{
					TVFace& tface = param.tvFace[iFaceIdx];
					tface.setTVerts(
						zeroIdxForFace + (triIdx + 1),
						zeroIdxForFace + (triIdx + 2),
						zeroIdxForFace
						);
				}
			}
			iFaceIdx++;
		}
		zeroIdxForFace += nVerts;
	}

	// Validate normals.
	pNormalSpec->SetAllExplicit();
	pNormalSpec->CheckAllData(nTriFaces);

	//if(rtMesh.callMethod("Boolean", "hasVertexColors", 0, 0).getBoolean())
	//{
	//	Tab<Point4> colors;
	//	colors.SetCount(nbIndices);
	//	std::vector<FabricCore::RTVal> args(2);
	//	args[0] = FabricSplice::constructExternalArrayRTVal("Float32", nbIndices * 4, colors.Addr(0));
	//	args[1] = FabricSplice::constructUInt32RTVal(4); // components
	//	rtMesh.callMethod("", "getVertexColorsAsExternalArray", 2, &args[0]);

	//	// how do color verts work on MNMesh?
	//	MIntArray face(nbSamples);
	//	MIntArray indices(nbSamples);
	//	unsigned int offset = 0;
	//	for(unsigned int i=0;i<mayaCounts.length();i++)
	//	{
	//		for(unsigned int j=0;j<mayaCounts[i];j++,offset++)
	//		{
	//			face[offset] = i;
	//			indices[offset] = j;
	//		}
	//	}
	//	mesh.setFaceVertexColors(values, face, indices);
	//}

	param.InvalidateGeomCache();
	param.InvalidateTopologyCache();

}
#pragma endregion