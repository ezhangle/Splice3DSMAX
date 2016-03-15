#include "StdAfx.h"
#include "MaxConversionFns.h"
#include "MeshNormalSpec.h"

#pragma region Convert functions allow modifying a value just before it is sent to Fabric

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
// of converting Max bytes to Fabric bytes
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

FabricCore::Variant GetVariant(const Point4& param, bool asColor/*=false*/)
{
	FabricCore::Variant vec4 = FabricCore::Variant::CreateDict();
	if (asColor) {
		vec4.setDictValue("r", FabricCore::Variant::CreateFloat32(param.x));
		vec4.setDictValue("g", FabricCore::Variant::CreateFloat32(param.y));
		vec4.setDictValue("b", FabricCore::Variant::CreateFloat32(param.z));
		vec4.setDictValue("a", FabricCore::Variant::CreateFloat32(param.w));
	} else {
		vec4.setDictValue("x", FabricCore::Variant::CreateFloat32(param.x));
		vec4.setDictValue("y", FabricCore::Variant::CreateFloat32(param.y));
		vec4.setDictValue("z", FabricCore::Variant::CreateFloat32(param.z));
		vec4.setDictValue("t", FabricCore::Variant::CreateFloat32(param.w));
	}
	return vec4;
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
		case TYPE_FRGBA:
			return GetVariant(*value.p4, true);
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
void ConvertToRTVal(int param, FabricCore::RTVal& rtVal)
{
	DbgAssert(rtVal.isValid());
	if (!rtVal.isValid())
		return;

	FabricCore::RTVal type = rtVal.getTypeName();
	const char* spliceType = type.getStringCString();
	if (strcmp(spliceType, "SInt32") == 0)
		rtVal = FabricCore::RTVal::ConstructSInt32(GetClient(), param);
	else if (strcmp(spliceType, "UInt32") == 0)
		rtVal = FabricCore::RTVal::ConstructUInt32(GetClient(), param);

	else if (strcmp(spliceType, "SInt8") == 0)
		rtVal = FabricCore::RTVal::ConstructSInt8(GetClient(), (int8_t)param);
	else if (strcmp(spliceType, "SInt16") == 0)
		rtVal = FabricCore::RTVal::ConstructSInt16(GetClient(), (int16_t)param);
	else if (strcmp(spliceType, "SInt64") == 0)
		rtVal = FabricCore::RTVal::ConstructSInt64(GetClient(), param);

	else if (strcmp(spliceType, "UInt8") == 0)
		rtVal = FabricCore::RTVal::ConstructUInt8(GetClient(), (uint8_t)param);
	else if (strcmp(spliceType, "UInt16") == 0)
		rtVal = FabricCore::RTVal::ConstructUInt16(GetClient(), (uint16_t)param);
	else if (strcmp(spliceType, "UInt64") == 0)
		rtVal = FabricCore::RTVal::ConstructUInt64(GetClient(), param);

	// These last two param types may be deprecated
	else if (strcmp(spliceType, "Size") == 0)
		rtVal = FabricCore::RTVal::ConstructUInt64(GetClient(), param);
	else if (strcmp(spliceType, "Index") == 0)
		rtVal = FabricCore::RTVal::ConstructUInt64(GetClient(), param);
	else if (strcmp(spliceType, "Integer") == 0)
		rtVal = FabricCore::RTVal::ConstructSInt32(GetClient(), param);

	// These can come through sometimes
	else if (strcmp(spliceType, "Float32") == 0 || strcmp(spliceType, "Scalar") == 0)
		rtVal = FabricCore::RTVal::ConstructFloat32(GetClient(), (float)param);
	else if (strcmp(spliceType, "Float64") == 0)
		rtVal = FabricCore::RTVal::ConstructFloat64(GetClient(), param);
	else
	{
		// Set a default val
		rtVal = FabricCore::RTVal::ConstructSInt32(GetClient(), param);
		DbgAssert(false && "Missing translation when setting int to Fabric");
	}
}

void ConvertToRTVal(float param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	if (!val.isValid())
		return;

	FabricCore::RTVal type = val.getTypeName();
	const char* spliceType = type.getStringCString();

	if (strcmp(spliceType, "Float64") == 0)
		val = FabricCore::RTVal::ConstructFloat64(GetClient(), param);
	else
		val.setFloat32(param);
}

void ConvertToRTVal(bool param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	val = FabricCore::RTVal::ConstructBoolean(GetClient(), param);
}

void ConvertToRTVal( const Point2& param, FabricCore::RTVal& val )
{
	DbgAssert( val.isValid() );
	if (!val.isValid())
		return;

	val.maybeGetMemberRef( "x" ).setFloat32( param.x );
	val.maybeGetMemberRef( "y" ).setFloat32( param.y );
}

void ConvertToRTVal(const Point3& param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	if (!val.isValid())
		return;

	val.maybeGetMemberRef("x").setFloat32(param.x);
	val.maybeGetMemberRef("y").setFloat32(param.y);
	val.maybeGetMemberRef("z").setFloat32(param.z);
}

void ConvertToRTVal(const Color& param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	if (!val.isValid())
		return;

	FabricCore::RTVal type = val.getTypeName();
	const char* spliceType = type.getStringCString();

	if (strcmp( spliceType, "RGB" ) == 0) // 24-bit color
	{
		val.maybeGetMemberRef( "r" ).setUInt8( uint8_t( param.r * 128 ) );
		val.maybeGetMemberRef( "g" ).setUInt8( uint8_t( param.g * 128 ) );
		val.maybeGetMemberRef( "b" ).setUInt8( uint8_t( param.b * 128 ) );
	}
	else if (strcmp( spliceType, "RGBA" ) == 0) // 32-bit color
	{
		val.maybeGetMemberRef( "a" ).setUInt8( 128 );
		val.maybeGetMemberRef( "r" ).setUInt8( uint8_t( param.r * 128 ) );
		val.maybeGetMemberRef( "g" ).setUInt8( uint8_t( param.g * 128 ) );
		val.maybeGetMemberRef( "b" ).setUInt8( uint8_t( param.b * 128 ) );
	}
	else if (strcmp( spliceType, "Color" ) == 0) // Full 32-bit x 4 channels color
	{
		val.maybeGetMemberRef( "a" ).setFloat32( 1.0f );
		val.maybeGetMemberRef( "r" ).setFloat32( param.r );
		val.maybeGetMemberRef( "g" ).setFloat32( param.g );
		val.maybeGetMemberRef( "b" ).setFloat32( param.b );
	}
}

void ConvertToRTVal(const Point4& param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	if (!val.isValid())
		return;

	FabricCore::RTVal type = val.getTypeName();
	const char* spliceType = type.getStringCString();

	if (strcmp(spliceType, "Color") == 0)
	{
		val.maybeGetMemberRef("r").setFloat32(param.x);
		val.maybeGetMemberRef("g").setFloat32(param.y);
		val.maybeGetMemberRef("b").setFloat32(param.z);
		val.maybeGetMemberRef("a").setFloat32(param.w);
	}
	else if (strcmp( spliceType, "RGBA" ) == 0)
	{
		val.maybeGetMemberRef( "r" ).setUInt8( (uint8_t)FLto255( param.x ) );
		val.maybeGetMemberRef( "g" ).setUInt8( (uint8_t)FLto255( param.y ) );
		val.maybeGetMemberRef( "b" ).setUInt8( (uint8_t)FLto255( param.z ) );
		val.maybeGetMemberRef( "a" ).setUInt8( (uint8_t)FLto255( param.w ) );
	}
	else {
		val.maybeGetMemberRef("x").setFloat32(param.x);
		val.maybeGetMemberRef("y").setFloat32(param.y);
		val.maybeGetMemberRef("z").setFloat32(param.z);
		val.maybeGetMemberRef("t").setFloat32(param.w);
	}
}

void ConvertToRTVal(const Quat& param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	if (!val.isValid())
		return;

	FabricCore::RTVal v = val.maybeGetMemberRef("v");
	ConvertToRTVal(Point3(param.x, param.y, param.z), v);
	val.maybeGetMemberRef("w").setFloat32(param.w);
}

void ConvertToRTVal(const Matrix3& param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	if (!val.isValid())
		return;

	const MRow* pInMtx = param.GetAddr();
	FabricCore::RTVal row0 = val.maybeGetMemberRef("row0");
	FabricCore::RTVal row1 = val.maybeGetMemberRef("row1");
	FabricCore::RTVal row2 = val.maybeGetMemberRef("row2");
	FabricCore::RTVal row3 = val.maybeGetMemberRef("row3");

	ConvertToRTVal(Point4(pInMtx[0][0], pInMtx[1][0], pInMtx[2][0], pInMtx[3][0]), row0);
	ConvertToRTVal(Point4(pInMtx[0][1], pInMtx[1][1], pInMtx[2][1], pInMtx[3][1]), row1);
	ConvertToRTVal(Point4(pInMtx[0][2], pInMtx[1][2], pInMtx[2][2], pInMtx[3][2]), row2);
	ConvertToRTVal(Point4(0, 0, 0, 1), row3);
}


void ConvertToRTVal(const MSTR& param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	CStr cStr = param.ToCStr();
	val = FabricCore::RTVal::ConstructString(GetClient(), cStr.data());
}

void ConvertToRTVal(const MCHAR* param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	CStr cStr = CStr::FromMCHAR(param);
	val = FabricCore::RTVal::ConstructString(GetClient(), cStr.data());
}

void ConvertToRTVal(const Mesh& param, FabricCore::RTVal& rtMesh)
{
	DbgAssert(rtMesh.isValid());
	if (!rtMesh.isValid() || rtMesh.isNullObject())
		rtMesh = FabricCore::RTVal::Create(GetClient(), "PolygonMesh", 0, nullptr);
	
	UINT nbPolygons = rtMesh.callMethod("UInt32", "polygonCount", 0, 0).getUInt32();
	bool rebuildMesh = nbPolygons != (UINT)param.numFaces;
	if(rebuildMesh){
		rtMesh.callMethod("", "clear", 0, 0);
	}

	FabricCore::Client client = GetClient();

	// Send all vertices to Fabric
	{
		FabricCore::RTVal args[2];
		args[0] = FabricCore::RTVal::ConstructExternalArray(client, "Float32", param.numVerts * 3, param.verts);
		args[1] = FabricCore::RTVal::ConstructUInt32(client, 3); // components
		rtMesh.callMethod("", "setPointsFromExternalArray", 2, args);
	}

	if(rebuildMesh){
		// Send all indices to Fabric
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
	
		FabricCore::RTVal args[2];
		args[0] = FabricCore::RTVal::ConstructExternalArray(client, "UInt32", dPolyCounts.size(), &dPolyCounts[0]);
		args[1] = FabricCore::RTVal::ConstructExternalArray(client, "UInt32", dVertIndex.size(), &dVertIndex[0]);
		rtMesh.callMethod("", "setTopologyFromCountsIndicesExternalArrays", 2, args);
	}

	// Set normals
	// Do we have specified normals?
	Mesh* pNonConstMesh = const_cast<Mesh*>(&param);
	MeshNormalSpec* pNormalSpec = pNonConstMesh->GetSpecifiedNormals();
	if (pNormalSpec != NULL && pNormalSpec->GetNumNormals() > 0)
	{
		// 1 normal per index
	  MeshNormalFace* pFaces = pNormalSpec->GetFaceArray();
    Point3* pNormalData = pNormalSpec->GetNormalArray();
    int nFaces = pNormalSpec->GetNumFaces();

    // We set normals 1-per-poly-point
    std::vector<float> dNormals( nFaces * 3 * 3);
    size_t toidx = 0;
		for (int i = 0; i < nFaces; i++)
		{
      for (int j = 0; j < 3; j++)
      {
        Point3& normal = pNormalData[pFaces[i].GetNormalID( j )];
        dNormals[toidx++] = normal.x;
        dNormals[toidx++] = normal.y;
        dNormals[toidx++] = normal.z;
      }
		}
    FabricCore::RTVal arg = FabricCore::RTVal::ConstructExternalArray(client, "Float32", nFaces * 3 * 3, &dNormals[0]);
		rtMesh.callMethod("", "setNormalsFromExternalArray", 1, &arg );
	}
	else
	{
		rtMesh.callMethod("", "recomputePointNormals", 0, NULL);
	}

  // Send UV's
  if (param.mapSupport( 1 ))
  {
    // Unpack UV's (explode by idx)
    FabricCore::RTVal args[2];
    std::vector<float> dUVs;
    int uvIdx = 0;
    dUVs.resize( param.numFaces * 3 * 2 );
    for (size_t i = 0; i < param.numFaces; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        int tVertIdx = param.tvFace[i].getTVert( j );
        dUVs[uvIdx++] = param.tVerts[tVertIdx].x;
        dUVs[uvIdx++] = param.tVerts[tVertIdx].y;
      }
    }

    args[0] = FabricCore::RTVal::ConstructString( client, "uvs0" );
    args[1] = FabricCore::RTVal::ConstructExternalArray( client, "Vec2", param.numFaces * 3, &dUVs[0] );

    // Force create UV's
    rtMesh.callMethod( "", "setAttributeFromPolygonPackedData", 2, args );
  }
	return;
}

void ConvertToRTVal(const FPValue& param, FabricCore::RTVal& val)
{
	DbgAssert(val.isValid());
	if (is_tab(param.type) || val.isArray())
	{
		if (is_tab(param.type) && val.isArray())
		{
			int cnt = param.aa_tab->Count();
			val.setArraySize(cnt);

			/*for (int i = 0; i < cnt; i++) {
				ConvertToRTVal()
				}*/
		}
	}
	else
	{
		// Is not tab or array
		switch ((int)root_type(param.type))
		{
		case TYPE_BOOL:
			return ConvertToRTVal(param.b, val);
		case TYPE_INT:
			return ConvertToRTVal(param.i, val);
		case TYPE_FLOAT:
		case TYPE_ANGLE:
		case TYPE_WORLD:
		case TYPE_PCNT_FRAC:
			return ConvertToRTVal(param.f, val);
		case TYPE_RGBA:
			return ConvertToRTVal(*param.clr, val);
		case TYPE_FRGBA:
			return ConvertToRTVal(*param.p4, val);
		case TYPE_POINT3:
			return ConvertToRTVal(*param.p, val);
		case TYPE_POINT4:
			return ConvertToRTVal(*param.p4, val);
		case TYPE_QUAT:
			return ConvertToRTVal(*param.q, val);
		case TYPE_ANGAXIS:
		{
			// Fabric has no AngAxis, so try converting to Quat
			AngAxis& aa = *param.aa;
			Quat q(aa);
			return ConvertToRTVal(q, val);
		}
		case TYPE_TSTR:
		{
			if (val.isData())
			{
				// For simple data RTVals, try to set the value directly
				return ConvertToRTVal(*param.tstr, val);
			}
			else
			{
				// If we have a complex type, we assume we are being passed JSON
				CStr cStr = param.tstr->ToCStr();
				val.setJSON(cStr);
			}
		}
			
		case TYPE_MESH:
			return ConvertToRTVal(*param.msh, val);
		default:
			DbgAssert(_T("Implement Me"));
			//return FabricCore::RTVal();
		}
	}
}

#pragma endregion // GetVariants


//////////////////////////////////////////////////////////////////////////
// Fabric -> Max
#pragma region Fabric to Max

#pragma region Variant to Max
void FabricToMaxValue(const FabricCore::Variant& var, bool& param) {
	DbgAssert(var.isBoolean());
	if (var.isBoolean())
		param = var.getBoolean();
}

void FabricToMaxValue(const FabricCore::Variant& var, int& param)
{
	if (var.isBoolean())
		param = var.getBoolean();
	else if (var.isFloat32())
		param = (int)var.getFloat32();
	else if (var.isFloat64())
		param = (int)var.getFloat64();
	else if (var.isSInt8())
		param = var.getSInt8();
	else if (var.isSInt16())
		param = var.getSInt16();
	else if (var.isSInt32())
		param = var.getSInt32();
	else if (var.isSInt64())
		param = (int)var.getSInt64();
	else if (var.isUInt8())
		param = var.getUInt8();
	else if (var.isUInt16())
		param = var.getUInt16();
	else if (var.isUInt32())
		param = (int)var.getUInt32();
	else if (var.isUInt64())
		param = (int)var.getUInt64();
	else
	{
		DbgAssert(!"Unkonwn Type");
	}
}

void FabricToMaxValue(const FabricCore::Variant& var, float& param)
{
	if (var.isFloat32())
		param = var.getFloat32();
	else if (var.isFloat64())
		param = (float)var.getFloat64();
	else if (var.isSInt8())
		param = (float)var.getSInt8();
	else if (var.isSInt16())
		param = (float)var.getSInt16();
	else if (var.isSInt32())
		param = (float)var.getSInt32();
	else if (var.isSInt64())
		param = (float)var.getSInt64();
	else if (var.isUInt8())
		param = (float)var.getUInt8();
	else if (var.isUInt16())
		param = (float)var.getUInt16();
	else if (var.isUInt32())
		param = (float)var.getUInt32();
	else if (var.isUInt64())
		param = (float)var.getUInt64();
	else
	{
		DbgAssert(!"Unkonwn Type");
	}
}

void FabricToMaxValue(const FabricCore::Variant& var, Point3& param)
{
	DbgAssert(var.isDict());
	if (!var.isDict())
		return;
	FabricToMaxValue(*var.getDictValue("x"), param.x);
	FabricToMaxValue(*var.getDictValue("y"), param.y);
	FabricToMaxValue(*var.getDictValue("z"), param.z);
}

void FabricToMaxValue(const FabricCore::Variant& var, Color& param)
{
	DbgAssert(var.isDict());
	if (!var.isDict())
		return;
	FabricToMaxValue(*var.getDictValue("r"), param.r);
	FabricToMaxValue(*var.getDictValue("g"), param.g);
	FabricToMaxValue(*var.getDictValue("b"), param.b);
	param /= 128;
}

void FabricToMaxValue(const FabricCore::Variant& var, Point4& param)
{
	DbgAssert(var.isDict());
	if (!var.isDict())
		return;
	// A variant could be either a XYZT or a RGBA,
	if (var.getDictValue("x") != NULL) {
		FabricToMaxValue(*var.getDictValue("x"), param.x);
		FabricToMaxValue(*var.getDictValue("y"), param.y);
		FabricToMaxValue(*var.getDictValue("z"), param.z);
		FabricToMaxValue(*var.getDictValue("t"), param.w);
	}
	else {
		FabricToMaxValue(*var.getDictValue("r"), param.x);
		FabricToMaxValue(*var.getDictValue("g"), param.y);
		FabricToMaxValue(*var.getDictValue("b"), param.z);
		FabricToMaxValue(*var.getDictValue("a"), param.w);
	}
}

void FabricToMaxValue(const FabricCore::Variant& var, Quat& param)
{
	DbgAssert(var.isDict());
	if (!var.isDict())
		return;
	const FabricCore::Variant* axis = var.getDictValue("v");
	FabricToMaxValue(*axis->getDictValue("x"), param.x);
	FabricToMaxValue(*axis->getDictValue("y"), param.y);
	FabricToMaxValue(*axis->getDictValue("z"), param.z);
	FabricToMaxValue(*var.getDictValue("w"), param.w);
}

void FabricToMaxValue(const FabricCore::Variant& var, Matrix3& param)
{
	DbgAssert(var.isDict());
	if (!var.isDict())
		return;

	MRow* pInMtx = param.GetAddr();
	FabricCore::Variant spliceMat = FabricCore::Variant::CreateDict();
	FabricCore::Variant spliceMatRow = FabricCore::Variant::CreateDict();

	const FabricCore::Variant* row0 = var.getDictValue("row0");
	FabricToMaxValue(*row0->getDictValue("x"), pInMtx[0][0]);
	FabricToMaxValue(*row0->getDictValue("y"), pInMtx[1][0]);
	FabricToMaxValue(*row0->getDictValue("z"), pInMtx[2][0]);
	FabricToMaxValue(*row0->getDictValue("t"), pInMtx[3][0]);

	const FabricCore::Variant* row1 = var.getDictValue("row1");
	FabricToMaxValue(*row1->getDictValue("x"), pInMtx[0][1]);
	FabricToMaxValue(*row1->getDictValue("y"), pInMtx[1][1]);
	FabricToMaxValue(*row1->getDictValue("z"), pInMtx[2][1]);
	FabricToMaxValue(*row1->getDictValue("t"), pInMtx[3][1]);

	const FabricCore::Variant* row2 = var.getDictValue("row2");
	FabricToMaxValue(*row2->getDictValue("x"), pInMtx[0][2]);
	FabricToMaxValue(*row2->getDictValue("y"), pInMtx[1][2]);
	FabricToMaxValue(*row2->getDictValue("z"), pInMtx[2][2]);
	FabricToMaxValue(*row2->getDictValue("t"), pInMtx[3][2]);

	//const FabricCore::Variant* row3 = var.getDictValue("row3");
	//FabricToMaxValue(*row3->getDictValue("x"), pInMtx[0][3]);
	//FabricToMaxValue(*row3->getDictValue("y"), pInMtx[1][3]);
	//FabricToMaxValue(*row3->getDictValue("z"), pInMtx[2][3]);
	//FabricToMaxValue(*row3->getDictValue("t"), pInMtx[3][3]);
}

void FabricToMaxValue(const FabricCore::Variant& var, MSTR& param)
{
	DbgAssert(var.isString());
	if (!var.isString())
		return;

	const char* val = var.getString_cstr();
	param = MSTR::FromACP(val);
}
#pragma endregion // Variants

#pragma region Fabric RTVal -> Max
//////////////////////////////////////////////////////////////////////////
// Convert from RTVal to Max value
void FabricToMaxValue(const FabricCore::RTVal& rtVal, bool& param)
{
	FabricCore::RTVal& ncrtVal = const_cast<FabricCore::RTVal&>(rtVal);
	param = ncrtVal.getBoolean();
}

void FabricToMaxValue(const FabricCore::RTVal& rtVal, int& param)
{
	FabricCore::RTVal& ncrtVal = const_cast<FabricCore::RTVal&>(rtVal);
	FabricCore::RTVal type = rtVal.getTypeName();
	const char* spliceType = type.getStringCString();
	if (strcmp(spliceType, "SInt32") == 0)
		param = ncrtVal.getSInt32();
	else if (strcmp(spliceType, "UInt32") == 0)
		param = ncrtVal.getUInt32();

	else if (strcmp(spliceType, "SInt8") == 0)
		param = ncrtVal.getSInt8();
	else if (strcmp(spliceType, "SInt16") == 0)
		param = ncrtVal.getSInt16();
	else if (strcmp(spliceType, "SInt64") == 0)
		param = (int)ncrtVal.getSInt64();

	else if (strcmp(spliceType, "UInt8") == 0)
		param = (int)ncrtVal.getUInt8();
	else if (strcmp(spliceType, "UInt16") == 0)
		param = (int)ncrtVal.getUInt16();
	else if (strcmp(spliceType, "UInt64") == 0)
		param = (int)ncrtVal.getUInt64();

	// These last two param types may be deprecated
	else if (strcmp(spliceType, "Size") == 0)
		param = (int)ncrtVal.getUInt64();
	else if (strcmp(spliceType, "Index") == 0)
		param = (int)ncrtVal.getUInt64();

	// boolean option
	else if (strcmp(spliceType, "Boolean") == 0)
		param = (int)ncrtVal.getBoolean(); 

	else // default
		param = ncrtVal.getSInt32();
}

void FabricToMaxValue(const FabricCore::RTVal& rtVal, float& param)
{
	FabricCore::RTVal& ncrtVal = const_cast<FabricCore::RTVal&>(rtVal);
	FabricCore::RTVal type = rtVal.getTypeName();
	const char* spliceType = type.getStringCString();

	if (strcmp(spliceType, "Float64") == 0)
		param = (float)ncrtVal.getFloat64();
	else // default
		param = ncrtVal.getFloat32();
}

void FabricToMaxValue(const FabricCore::RTVal& rtVal, Point2& param)
{
	param[0] = const_cast<FabricCore::RTVal&>(rtVal).maybeGetMemberRef("x").getFloat32();
	param[1] = const_cast<FabricCore::RTVal&>(rtVal).maybeGetMemberRef("y").getFloat32();
}

void FabricToMaxValue(const FabricCore::RTVal& rtVal, Point3& param)
{
	param[0] = const_cast<FabricCore::RTVal&>(rtVal).maybeGetMemberRef("x").getFloat32();
	param[1] = const_cast<FabricCore::RTVal&>(rtVal).maybeGetMemberRef("y").getFloat32();
	param[2] = const_cast<FabricCore::RTVal&>(rtVal).maybeGetMemberRef("z").getFloat32();
}

void FabricToMaxValue(const FabricCore::RTVal& rtVal, Point4& param)
{
	FabricCore::RTVal& ncVal = const_cast<FabricCore::RTVal&>(rtVal);
	FabricCore::RTVal type = rtVal.getTypeName();
	const char* spliceType = type.getStringCString();
	if (strcmp( spliceType, "RGBA" ) == 0) {
		param[0] = ncVal.maybeGetMemberRef( "r" ).getUInt8() / 255.0f;
		param[1] = ncVal.maybeGetMemberRef( "g" ).getUInt8() / 255.0f;
		param[2] = ncVal.maybeGetMemberRef( "b" ).getUInt8() / 255.0f;
		param[3] = ncVal.maybeGetMemberRef( "a" ).getUInt8() / 255.0f;
	}
	else if (strcmp(spliceType, "Color") == 0) {
		param[0] = ncVal.maybeGetMemberRef("r").getFloat32();
		param[1] = ncVal.maybeGetMemberRef("g").getFloat32();
		param[2] = ncVal.maybeGetMemberRef("b").getFloat32();
		param[3] = ncVal.maybeGetMemberRef("a").getFloat32();
	}
	else { // Assumed type == "Vec4"
		param[0] = ncVal.maybeGetMemberRef("x").getFloat32();
		param[1] = ncVal.maybeGetMemberRef("y").getFloat32();
		param[2] = ncVal.maybeGetMemberRef("z").getFloat32();
		param[3] = ncVal.maybeGetMemberRef("t").getFloat32();
	}
}

void FabricToMaxValue(const FabricCore::RTVal& rtVal, Color& param)
{
	// Assumed KL RGB type (RGBA & Color map to Point4 type)
	FabricCore::RTVal& ncVal = const_cast<FabricCore::RTVal&>(rtVal);
	param.r = ncVal.maybeGetMemberRef( "r" ).getUInt8() / 255.0f;
	param.g = ncVal.maybeGetMemberRef( "g" ).getUInt8() / 255.0f;
	param.b = ncVal.maybeGetMemberRef( "b" ).getUInt8() / 255.0f;
}

void FabricToMaxValue(const FabricCore::RTVal& rtVal, Quat& param)
{
	// Fabric Quats are structured ( Vec3 v; Scalar w; )
	FabricCore::RTVal rtV = const_cast<FabricCore::RTVal&>(rtVal).maybeGetMemberRef("v");
	Point3 maxV;
	FabricToMaxValue(rtV, maxV);
	float w = const_cast<FabricCore::RTVal&>(rtVal).maybeGetMemberRef("w").getFloat32();
	param.Set(maxV, w);
	// When converting from Fabrics Y up to Max's Z up, our
	// rotations come out -ve.  Invert the Quat, and we 
	// get equivalent rotations (tested via euler rot)
	param.Invert();
}

void FabricToMaxValue(const FabricCore::RTVal& dgPort, Matrix3& param)
{
	FabricCore::RTVal pRow0 = dgPort.maybeGetMemberRef("row0");
	FabricCore::RTVal pRow1 = dgPort.maybeGetMemberRef("row1");
	FabricCore::RTVal pRow2 = dgPort.maybeGetMemberRef("row2");
	
	if (!pRow0.isValid() ||
		!pRow1.isValid() ||
		!pRow2.isValid())
	{
		DbgAssert(!"Fabric Matrix Valid");
		return;
	}

	Point4 columns[3];
	FabricToMaxValue(pRow0, columns[0]);
	FabricToMaxValue(pRow1, columns[1]);
	FabricToMaxValue(pRow2, columns[2]);

	param.SetColumn(0, columns[0]);
	param.SetColumn(1, columns[1]);
	param.SetColumn(2, columns[2]);

	param.ValidateFlags();
}

void FabricToMaxValue(const FabricCore::RTVal& rtv, Mesh& param)
{
	if (rtv.isNullObject())
	{
		DbgAssert(!"Fabric Mesh Valid");
		param.FreeAll();
		return;
	}

	FabricCore::Client client = GetClient();

	// Cache version number, compare for purposes of caching.
	// this line was causing asserts. 
	//UINT32 version = rtMesh.maybeGetMemberRef("version").getUInt32();
	FabricCore::RTVal& rtMesh = const_cast<FabricCore::RTVal&>(rtv);
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
		args[0] = FabricCore::RTVal::ConstructExternalArray(client, "Float32", nbPoints * 3, param.verts);
		args[1] = FabricCore::RTVal::ConstructUInt32(client, 3); // components
		rtMesh.callMethod("", "getPointsAsExternalArray", 2, &args[0]);
	}

	// Init UV's
	if(bHasUVs)
	{
		// Tell our mesh that yes, it does support mapping
		param.setMapSupport(1);

		Tab<Point2> uvs;
		uvs.SetCount(nbIndices);
		std::vector<FabricCore::RTVal> args(2);
		args[0] = FabricCore::RTVal::ConstructExternalArray(client, "Float32", uvs.Count() * 2, uvs.Addr(0));
		args[1] = FabricCore::RTVal::ConstructUInt32(client, 2); // components
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
		args[0] = FabricCore::RTVal::ConstructExternalArray(client, "UInt32", nbPolygons, faceVtxCount.Addr(0));
		args[1] = FabricCore::RTVal::ConstructExternalArray(client, "UInt32", nbIndices, indices.Addr(0));
		rtMesh.callMethod("", "getTopologyAsCountsIndicesExternalArrays", 2, &args[0]);
	}

  //////////////////////////////////////////////////////////////////////////
  // Get normals from rtMesh
  Tab<Point3> rtNormals;
  param.SpecifyNormals();
  MeshNormalSpec* pNormalSpec = param.GetSpecifiedNormals();
  rtNormals.SetCount( nbIndices );
  {
    std::vector<FabricCore::RTVal> args( 1 );
    args[0] = FabricCore::RTVal::ConstructExternalArray( client, "Float32", nbIndices * 3, rtNormals.Addr( 0 ) );
    rtMesh.callMethod( "", "getNormalsAsExternalArray", 1, &args[0] );
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
			DbgAssert(triIdx < nTriFaces);

			Face& f = param.faces[iFaceIdx];
			MeshNormalFace& nf = pNFaces[iFaceIdx];

			// Default to all edges visible
			f.setEdgeVisFlags(EDGE_VIS, EDGE_VIS, EDGE_VIS);
			if (triIdx > 0)
			{
				// For quads (and higher) hide edge between adjoining tri's
				param.faces[iFaceIdx - 1].setEdgeVis(2, EDGE_INVIS);
				f.setEdgeVis(2, EDGE_INVIS);
			}

			DbgAssert(zeroIdxForFace + (triIdx + 2) < nbIndices);
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
	//pNormalSpec->CheckAllData(nTriFaces);

	//if(rtMesh.callMethod("Boolean", "hasVertexColors", 0, 0).getBoolean())
	//{
	//	Tab<Point4> colors;
	//	colors.SetCount(nbIndices);
	//	std::vector<FabricCore::RTVal> args(2);
	//	args[0] = FabricCore::RTVal::ConstructExternalArrayRTVal("Float32", nbIndices * 4, colors.Addr(0));
	//	args[1] = FabricCore::RTVal::ConstructUInt32RTVal(4); // components
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

void FabricToMaxValue(const FabricCore::RTVal& rtv, MSTR& param) {
	FabricCore::RTVal& ncval = const_cast<FabricCore::RTVal&>(rtv);
	param = MSTR::FromACP(ncval.getStringCString(), ncval.getStringLength());
}

#pragma endregion // RTVals

#pragma endregion
