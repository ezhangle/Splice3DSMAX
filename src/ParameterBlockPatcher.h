//////////////////////////////////////////////////////////////////////////
//
// This file defines functions that are required to
// patch Max's incredible omission of access to paramblock Point2 values
//
//////////////////////////////////////////////////////////////////////////

#pragma once


// Point2
BOOL GetPB2Value( IParamBlock2* pblock, ParamID id, TimeValue t, Point2& v, Interval &ivalid, int tabIndex )
{
	int i = pblock->IDtoIndex( id );
	ParamBlockDesc2* pbd = pblock->GetDesc();
	v = Point2::Origin;
	DbgAssert( i >= 0 && i < pbd->count );
	if (!(i >= 0 && i < pbd->count))
		return FALSE;

	const ParamDef& pd = pbd->paramdefs[i];
	
	DbgAssert( !is_tab( pd.type ) || (tabIndex >= 0 && tabIndex < pblock->Count( id )) );
	if (!(!is_tab( pd.type ) || (tabIndex >= 0 && tabIndex < pblock->Count( id ))))
		return FALSE;

	ParamType2 type = base_type( pd.type );
	bool type_ok = (
		type == TYPE_POINT2/* ||
		type == TYPE_BOOL2 ||
		type == TYPE_INT2*/
		);
	DbgAssert( type_ok );
	if (!type_ok)
		return FALSE;

	PB2Value& vs = pblock->GetPB2Value( id, tabIndex );
	ReferenceMaker* owner = pblock->GetOwner();
	if (pd.flags&((ULONGLONG)P_USE_ACCESSOR_ONLY << (ULONGLONG) sizeof( DWORD ) * 8))
	{
		PB2Value vs;
		Point2 tmp_v = v;
		vs.p2 = &tmp_v;

		if (pd.accessor != NULL)
			pd.accessor->Get( vs, owner, id, tabIndex, t, ivalid );

		v = *vs.p2;
	}
	else if (vs.is_constant())
	{
		Point2* p = vs.p2;
		if (pd.accessor != NULL)
		{
			pd.accessor->Get( vs, owner, id, tabIndex, t, ivalid );
			if (vs.p2 != p)
			{
				if (p == NULL) p = new Point2;
				*p = *vs.p2;
				vs.p2 = p;
			}
		}
		v = *vs.p2;
	}
	else
	{
		if (vs.control == NULL)
			v = Point2( 0, 0 );
		else
		{
			vs.control->GetValue( t, &v, ivalid );
			if (pd.accessor != NULL)
			{
				Point2 *p = new Point2( v );
				PB2Value va; va.p2 = p;
				pd.accessor->Get( va, owner, id, tabIndex, t, ivalid );
				v = *va.p2;
				delete p;
				p = NULL;
			}
		}
	}
	return TRUE;
}

template<typename T>
BOOL GetPB2Value( IParamBlock2* pblock, ParamID id, TimeValue t, T& v, Interval &ivalid, int tabIndex )
{
	return pblock->GetValue( id, t, v, ivalid, tabIndex );
}

//////////////////////////////////////////////////////////////////////////

BOOL SetPB2Value( IParamBlock2* pblock, ParamID id, TimeValue t, Point2& v, int tabIndex )
{
	int i = pblock->IDtoIndex( id );
	ParamBlockDesc2* pbd = pblock->GetDesc();
	DbgAssert( i >= 0 && i < pbd->count );
	if (!(i >= 0 && i < pbd->count))
		return FALSE;

	const ParamDef& pd = pbd->paramdefs[i];

	DbgAssert( !is_tab( pd.type ) || (tabIndex >= 0 && tabIndex < pblock->Count( id )) );
	if (!(!is_tab( pd.type ) || (tabIndex >= 0 && tabIndex < pblock->Count( id ))))
		return FALSE;

	PB2Value& vd = pblock->GetPB2Value( id, tabIndex );
	ReferenceMaker* owner = pblock->GetOwner();

	ParamType2 type = base_type( pd.type );
	bool type_ok = (
		type == TYPE_POINT2 /*||
		type == TYPE_BOOL2 ||
		type == TYPE_INT2*/
		);
	DbgAssert( type_ok );
	if (!type_ok)
		return FALSE;

	if (vd.is_constant())
	{
		*vd.p2 = v;
		if (pd.accessor != NULL)
			pd.accessor->Set( vd, owner, id, tabIndex, t );
		
		pblock->NotifyDependents( FOREVER, PART_ALL, REFMSG_CHANGE );
	}
	else
	{
		vd.control->SetValue( t, v );
	}
	return TRUE;
}


template<typename T>
BOOL SetPB2Value( IParamBlock2* pblock, ParamID id, TimeValue t, T& v, int tabIndex )
{
	return pblock->SetValue( id, t, v, tabIndex );
}