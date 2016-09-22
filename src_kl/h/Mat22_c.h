#ifndef __KL2EDK_AUTOGEN_Mat22_c__
#define __KL2EDK_AUTOGEN_Mat22_c__

#ifdef KL2EDK_INCLUDE_MESSAGES
  #pragma message ( "Including 'Mat22_c.h'" )
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
#include "Vec2_c.h"
#include "Complex.h"

namespace Fabric { namespace EDK { namespace KL {

// KL struct 'Mat22_c'
// Defined at Mat22_c.kl:21:1

struct Mat22_c
{
  typedef Mat22_c &Result;
  typedef Mat22_c const &INParam;
  typedef Mat22_c &IOParam;
  typedef Mat22_c &OUTParam;
  
  Vec2_c row0;
  Vec2_c row1;
};

inline void Traits<Mat22_c>::ConstructEmpty( Mat22_c &val )
{
  Traits< Vec2_c >::ConstructEmpty( val.row0 );
  Traits< Vec2_c >::ConstructEmpty( val.row1 );
}
inline void Traits<Mat22_c>::ConstructCopy( Mat22_c &lhs, Mat22_c const &rhs )
{
  Traits< Vec2_c >::ConstructCopy( lhs.row0, rhs.row0 );
  Traits< Vec2_c >::ConstructCopy( lhs.row1, rhs.row1 );
}
inline void Traits<Mat22_c>::AssignCopy( Mat22_c &lhs, Mat22_c const &rhs )
{
  Traits< Vec2_c >::AssignCopy( lhs.row0, rhs.row0 );
  Traits< Vec2_c >::AssignCopy( lhs.row1, rhs.row1 );
}
inline void Traits<Mat22_c>::Destruct( Mat22_c &val )
{
  Traits< Vec2_c >::Destruct( val.row1 );
  Traits< Vec2_c >::Destruct( val.row0 );
}
}}}

#endif // __KL2EDK_AUTOGEN_Mat22_c__
