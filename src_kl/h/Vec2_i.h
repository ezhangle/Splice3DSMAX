#ifndef __KL2EDK_AUTOGEN_Vec2_i__
#define __KL2EDK_AUTOGEN_Vec2_i__

#ifdef KL2EDK_INCLUDE_MESSAGES
  #pragma message ( "Including 'Vec2_i.h'" )
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

// KL struct 'Vec2_i'
// Defined at Vec2_i.kl:18:1

struct Vec2_i
{
  typedef Vec2_i &Result;
  typedef Vec2_i const &INParam;
  typedef Vec2_i &IOParam;
  typedef Vec2_i &OUTParam;
  
  SInt32 x;
  SInt32 y;
};

inline void Traits<Vec2_i>::ConstructEmpty( Vec2_i &val )
{
  Traits< SInt32 >::ConstructEmpty( val.x );
  Traits< SInt32 >::ConstructEmpty( val.y );
}
inline void Traits<Vec2_i>::ConstructCopy( Vec2_i &lhs, Vec2_i const &rhs )
{
  Traits< SInt32 >::ConstructCopy( lhs.x, rhs.x );
  Traits< SInt32 >::ConstructCopy( lhs.y, rhs.y );
}
inline void Traits<Vec2_i>::AssignCopy( Vec2_i &lhs, Vec2_i const &rhs )
{
  Traits< SInt32 >::AssignCopy( lhs.x, rhs.x );
  Traits< SInt32 >::AssignCopy( lhs.y, rhs.y );
}
inline void Traits<Vec2_i>::Destruct( Vec2_i &val )
{
  Traits< SInt32 >::Destruct( val.y );
  Traits< SInt32 >::Destruct( val.x );
}
}}}

#endif // __KL2EDK_AUTOGEN_Vec2_i__
