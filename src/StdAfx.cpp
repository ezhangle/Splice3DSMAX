#include "StdAfx.h"

HINSTANCE hInstance = 0;
TCHAR *GetString(int id)
{
    static TCHAR buf[256];

    if(hInstance)
        return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
    return NULL;
}

// Max Matrix class never bothered exporting its Identity member, so we define our own here.
const Matrix3 Matrix3::Identity(1);

// The following 
#ifdef _DEBUG
explicit_bool ifThisDoesntCompileUpdateFabricVersion;
#endif

const MSTR* EmptyStr()
{
  static MSTR empty( _M("") );
  return &empty;
}


MSTR ToMstr( const QString& qs )
{
	// NOTE - this copy should get RVO'ed away
	MSTR str = MSTR::FromACP( qs.toStdString().data() );
	//wchar_t* guts = str.dataForWrite( qs.size() );
	
	//qs.toWCharArray( guts );
	return str;
}

extern MSTR ToMstr( const std::string& str )
{
	return MSTR::FromACP( str.data() );
}

extern MSTR ToMstr( const char* str )
{
	return MSTR::FromACP( str );
}

CStr ToCStr( const MSTR& ms )
{
	// NOTE - this copy should get RVO'ed away
	CStr str = ms.ToCStr();
	return str;
}

QString ToQStr( const MSTR& ms )
{
	// NOTE - this copy should get RVO'ed away
	QString str = ms.ToCStr().data();
	return str;
}