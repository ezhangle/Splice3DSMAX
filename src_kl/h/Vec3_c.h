#ifndef __KL2EDK_AUTOGEN_Vec3_c__
#define __KL2EDK_AUTOGEN_Vec3_c__

#ifdef KL2EDK_INCLUDE_MESSAGES
  #pragma message ( "Including 'Vec3_c.h'" )
#endif

////////////////////////////////////////////////////////////////
// THIS FILE IS AUTOMATICALLY GENERATED -- DO NOT MODIFY!!
////////////////////////////////////////////////////////////////
// Generated by kl2edk version 2.3.0
////////////////////////////////////////////////////////////////

#include <FabricEDK.h>
#if FABRIC_EDK_VERSION_MAJ != 2 || FABRIC_EDK_VERSION_MIN != 3
# error "This file needs to be rebuilt for the current EDK version!"
#endif

#include "global.h"
#include "Complex.h"

namespace Fabric { namespace EDK { namespace KL {

// KL struct 'Vec3_c'
// Defined at Vec3_c.kl:21:1

struct Vec3_c
{
  typedef Vec3_c &Result;
  typedef Vec3_c const &INParam;
  typedef Vec3_c &IOParam;
  typedef Vec3_c &OUTParam;
  
  Complex x;
  Complex y;
  Complex z;
};

inline void Traits<Vec3_c>::ConstructEmpty( Vec3_c &val )
{
  Traits< Complex >::ConstructEmpty( val.x );
  Traits< Complex >::ConstructEmpty( val.y );
  Traits< Complex >::ConstructEmpty( val.z );
}
inline void Traits<Vec3_c>::ConstructCopy( Vec3_c &lhs, Vec3_c const &rhs )
{
  Traits< Complex >::ConstructCopy( lhs.x, rhs.x );
  Traits< Complex >::ConstructCopy( lhs.y, rhs.y );
  Traits< Complex >::ConstructCopy( lhs.z, rhs.z );
}
inline void Traits<Vec3_c>::AssignCopy( Vec3_c &lhs, Vec3_c const &rhs )
{
  Traits< Complex >::AssignCopy( lhs.x, rhs.x );
  Traits< Complex >::AssignCopy( lhs.y, rhs.y );
  Traits< Complex >::AssignCopy( lhs.z, rhs.z );
}
inline void Traits<Vec3_c>::Destruct( Vec3_c &val )
{
  Traits< Complex >::Destruct( val.z );
  Traits< Complex >::Destruct( val.y );
  Traits< Complex >::Destruct( val.x );
}
}}}

#endif // __KL2EDK_AUTOGEN_Vec3_c__
