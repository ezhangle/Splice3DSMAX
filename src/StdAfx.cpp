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

const MSTR EMPTY(_M(""));