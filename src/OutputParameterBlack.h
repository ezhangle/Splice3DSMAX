#pragma once


class OutPortAccessors : PBAccessor
{
  virtual void Get( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t, Interval &valid )
  {
    IParamBlock2* outputBlock = owner->GetParamBlock( 1 );
    MCHAR* portName = outputBlock->FindParamAlias( id, tabIndex );
    FabricTranslationFPInterface* fabric = GetFabricInterface( owner );
    FPValue& vfp = fabric->GetArgValue( portName, _M( "" ) );
    switch (vfp.type)
    {
      case TYPE_FLOAT:
        v.f = vfp.f;
        break;
    }
  }

};

ParamBlockDesc2* GetOutputBlock(ClassDesc2* cd2)
{
  static OutPortAccessors outputBlockAccesor;
  static ParamBlockDesc2 _output_desc(
    //Required arguments ----------------------------
    1, _T( "output_params" ), 0, cd2,
    //flags
    P_AUTO_CONSTRUCT,

    //Dependent arguments ---------------------------
    //required because P_AUTO_CONSTRUCT was flagged
    //This declares the number of rollouts
    1,

    //Parameter Specifications ----------------------
      0, _T( "" ), TYPE_FLOAT, P_TRANSIENT | P_READ_ONLY | P_USE_ACCESSOR_ONLY, 0,
      p_accessor, &outputBlockAccesor,
      p_end,
    p_end
    );
}