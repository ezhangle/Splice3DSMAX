#ifndef __KL2EDK_AUTOGEN_Mat44_cd__
#define __KL2EDK_AUTOGEN_Mat44_cd__

#ifdef KL2EDK_INCLUDE_MESSAGES
  #pragma message ( "Including 'Mat44_cd.h'" )
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
#include "Vec4_cd.h"
#include "Complex_d.h"

namespace Fabric { namespace EDK { namespace KL {

// KL struct 'Mat44_cd'
// Defined at Mat44_cd.kl:50:1

struct Mat44_cd
{
  typedef Mat44_cd &Result;
  typedef Mat44_cd const &INParam;
  typedef Mat44_cd &IOParam;
  typedef Mat44_cd &OUTParam;
  
  Vec4_cd row0;
  Vec4_cd row1;
  Vec4_cd row2;
  Vec4_cd row3;
};

inline void Traits<Mat44_cd>::ConstructEmpty( Mat44_cd &val )
{
  Traits< Vec4_cd >::ConstructEmpty( val.row0 );
  Traits< Vec4_cd >::ConstructEmpty( val.row1 );
  Traits< Vec4_cd >::ConstructEmpty( val.row2 );
  Traits< Vec4_cd >::ConstructEmpty( val.row3 );
}
inline void Traits<Mat44_cd>::ConstructCopy( Mat44_cd &lhs, Mat44_cd const &rhs )
{
  Traits< Vec4_cd >::ConstructCopy( lhs.row0, rhs.row0 );
  Traits< Vec4_cd >::ConstructCopy( lhs.row1, rhs.row1 );
  Traits< Vec4_cd >::ConstructCopy( lhs.row2, rhs.row2 );
  Traits< Vec4_cd >::ConstructCopy( lhs.row3, rhs.row3 );
}
inline void Traits<Mat44_cd>::AssignCopy( Mat44_cd &lhs, Mat44_cd const &rhs )
{
  Traits< Vec4_cd >::AssignCopy( lhs.row0, rhs.row0 );
  Traits< Vec4_cd >::AssignCopy( lhs.row1, rhs.row1 );
  Traits< Vec4_cd >::AssignCopy( lhs.row2, rhs.row2 );
  Traits< Vec4_cd >::AssignCopy( lhs.row3, rhs.row3 );
}
inline void Traits<Mat44_cd>::Destruct( Mat44_cd &val )
{
  Traits< Vec4_cd >::Destruct( val.row3 );
  Traits< Vec4_cd >::Destruct( val.row2 );
  Traits< Vec4_cd >::Destruct( val.row1 );
  Traits< Vec4_cd >::Destruct( val.row0 );
}
}}}

#endif // __KL2EDK_AUTOGEN_Mat44_cd__