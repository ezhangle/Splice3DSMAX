#ifndef __KL2EDK_AUTOGEN_Vec4_d__
#define __KL2EDK_AUTOGEN_Vec4_d__

#ifdef KL2EDK_INCLUDE_MESSAGES
  #pragma message ( "Including 'Vec4_d.h'" )
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

namespace Fabric { namespace EDK { namespace KL {

// KL struct 'Vec4_d'
// Defined at Vec4_d.kl:19:1

struct Vec4_d
{
  typedef Vec4_d &Result;
  typedef Vec4_d const &INParam;
  typedef Vec4_d &IOParam;
  typedef Vec4_d &OUTParam;
  
  Float64 x;
  Float64 y;
  Float64 z;
  Float64 t;
};

inline void Traits<Vec4_d>::ConstructEmpty( Vec4_d &val )
{
  Traits< Float64 >::ConstructEmpty( val.x );
  Traits< Float64 >::ConstructEmpty( val.y );
  Traits< Float64 >::ConstructEmpty( val.z );
  Traits< Float64 >::ConstructEmpty( val.t );
}
inline void Traits<Vec4_d>::ConstructCopy( Vec4_d &lhs, Vec4_d const &rhs )
{
  Traits< Float64 >::ConstructCopy( lhs.x, rhs.x );
  Traits< Float64 >::ConstructCopy( lhs.y, rhs.y );
  Traits< Float64 >::ConstructCopy( lhs.z, rhs.z );
  Traits< Float64 >::ConstructCopy( lhs.t, rhs.t );
}
inline void Traits<Vec4_d>::AssignCopy( Vec4_d &lhs, Vec4_d const &rhs )
{
  Traits< Float64 >::AssignCopy( lhs.x, rhs.x );
  Traits< Float64 >::AssignCopy( lhs.y, rhs.y );
  Traits< Float64 >::AssignCopy( lhs.z, rhs.z );
  Traits< Float64 >::AssignCopy( lhs.t, rhs.t );
}
inline void Traits<Vec4_d>::Destruct( Vec4_d &val )
{
  Traits< Float64 >::Destruct( val.t );
  Traits< Float64 >::Destruct( val.z );
  Traits< Float64 >::Destruct( val.y );
  Traits< Float64 >::Destruct( val.x );
}
}}}

#endif // __KL2EDK_AUTOGEN_Vec4_d__
