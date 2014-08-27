klScript = "
require Scalar;
require PolygonMesh;
require Vec3;
require Vec4;
require SInt32;
require Xfo;
require Quat;

operator geomGen(Scalar twist, Scalar length, io PolygonMesh geo) {
  geo = PolygonMesh();
  Size nbQuads = 100;
  
  if(geo.pointCount() != nbQuads * 4) {
    geo.clear();
    geo.createPoints(nbQuads * 4);  
    geo.beginStructureChanges();
    Size vOff = 0;
    for(Size i=0;i<nbQuads;i++)
    {
      geo.addPolygon(vOff + 0, vOff + 1, vOff + 2, vOff + 3);
      vOff += 4;
    }
    geo.endStructureChanges();
  }
  

  Xfo inc;
  inc.tr.y = length;
  inc.ori.setFromEulerAngles(Vec3(0.2 * twist, 0.4 * twist, 0.1 * twist));
  Xfo xfo;

  Size vOff = 0;
  for(Size i=0;i<nbQuads;i++)
  {
    geo.setPointPosition(vOff++, xfo.transformVector(Vec3(-2, 0, -2)));  
    geo.setPointPosition(vOff++, xfo.transformVector(Vec3(-2, 0, 2)));  
    geo.setPointPosition(vOff++, xfo.transformVector(Vec3(2, 0, 2)));  
    geo.setPointPosition(vOff++, xfo.transformVector(Vec3(2, 0, -2)));  
    xfo = xfo * inc;
  }

  geo.addShell(0.5);
  geo.recomputePointNormals(HALF_PI * 0.5);
}
"

plugin geometry scriptedKLPlugin
name:"Twisted Boxes"
classID:#(0x133067, 0x54374)
category:"KL Primitives"
replaceUI:true
extends:SpliceMesh
(
	local clickAt
	
	tool create
	(
		on mousePoint click do
		(
			case click of 
			(
				1: (
					clickAt = gridPoint
					nodeTM.translation = gridPoint
				)
				2:  #stop
			)
		)
		
		on mouseMove click do 
		(
			if click == 2 then 
			(
				newPt = gridPoint
				dist = length(newPt - clickAt)
				delegate.length = dist / 100
			)
		)
	)
	rollout twisting "Parameters"
	(
		spinner twist_value "Twist" range:[-10,10,0]
		on twist_value changed val do delegate.Twist = val * PI / 180
	)
	
	on create do 
	(
		-- Initialize our scripted splice stuff
		delegate.outport = "geo"
		delegate.CreatePort "twist" "Scalar" -2
		delegate.CreatePort "length" "Scalar" -2
		delegate.SetKLCode "geomGen" klScript
	)
)