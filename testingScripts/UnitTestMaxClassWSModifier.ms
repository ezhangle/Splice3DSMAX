-----------------------------------------------------------
--
-- Tests creating a geometry that modifies its object based on the position in the world
--
-----------------------------------------------------------
fn TestWSModifier = 
(
	tp = Teapot radius:40 pos:[-14.2223,31.2546,0]
	sp = Sphere radius:40 smooth:on segs:32 pos:[58.9291,-39.6191,0]

	dfgMod = Fabric_WSModifier()
	addModifier sp dfgMod

	dfgMod.DFGAddFunc "shrinkWrap" "dfgEntry {
	  // TODO
	}
	" [390.000000,211.000000]  execPath:""

	dfgMod.DFGAddPort "source" 1 "PolygonMesh" portToConnect:"" extDep:"Geometry" metaData:"" execPath:"shrinkWrap"
	dfgMod.DFGAddPort "sourceTrans" 0 "Mat44" portToConnect:"" extDep:"" metaData:"" execPath:"shrinkWrap"
	dfgMod.DFGAddPort "target" 0 "PolygonMesh" portToConnect:"" extDep:"" metaData:"" execPath:"shrinkWrap"
	dfgMod.DFGAddPort "targetTrans" 0 "Mat44" portToConnect:"" extDep:"" metaData:"" execPath:"shrinkWrap"
	dfgMod.DFGAddPort "offset" 0 "Float32" portToConnect:"" extDep:"" metaData:"" execPath:"shrinkWrap"
	dfgMod.DFGSetCode "
	operator ParallelShrinkWrap<<<index>>>( io PolygonMesh source, SpatialQueryable target, Scalar offset, Mat44 srcToTarget, Mat44 targToSource ) {
	  Ray ray;
	  ray.start = srcToTarget * source.getPointPosition( index );
	  ray.direction = -ray.start.unit();//Unit vector toward targets center

	  GeometryLocation location = target.raycast(ray, true, 0.0, SCALAR_INFINITE);
	  if( location.isValid() ) {
		// How close to source was this hit?
		Vec3 hitPos = target.getPositionAtLocation( location );
		// If the hit is closer than 10 units, then push back to 10 units
		Vec3 vFromHit = ray.start - hitPos;
		Scalar hitDist = vFromHit.length();
		if (hitDist < offset) {
		  vFromHit *= offset / hitDist;
		  hitPos += vFromHit;
		  source.setPointPosition( index, targToSource * hitPos );
		}
	  }
	}

	dfgEntry {
	  //Initialize options to use a sparse grid
	  GenericValueContainer options = GenericValueContainer();
	  PrepareForSpatialQueries_setSparseGrid(options);

	  //Prepare for one query per source point

	  Mat44 srcToTarget = sourceTrans * targetTrans.inverse();
	  Mat44 targToSource = srcToTarget.inverse();
	  
	  target.prepareForSpatialQueries( source.pointCount(), options );
	  ParallelShrinkWrap<<<source.pointCount()>>>( source, target, offset, srcToTarget, targToSource );
	}
	"    execPath:"shrinkWrap"

	dfgMod.DFGConnect "baseMeshTransform" "shrinkWrap.sourceTrans"   execPath:""
	dfgMod.DFGConnect "baseMesh" "shrinkWrap.source"   execPath:""
	dfgMod.DFGAddPort "target" 0 "PolygonMesh" portToConnect:"shrinkWrap.target" extDep:"" metaData:"" execPath:""
	dfgMod.DFGAddPort "targetTrans" 0 "Mat44" portToConnect:"shrinkWrap.targetTrans" extDep:"" metaData:"" execPath:""
	dfgMod.DFGAddPort "offset" 0 "Float32" portToConnect:"shrinkWrap.offset" extDep:"" metaData:"" execPath:""

	dfgMod.SetPortMetadata "targetTrans" "MaxType" "17" -- Reset to type Node input
	dfgMod.DFGConnect "shrinkWrap.source" "outputValue"   execPath:""

	FAssert (CompBB sp 138.564) "Bad init size"

	dfgMod.target = tp
	dfgMod.targetTrans = tp
	dfgMod.offset = 70

	-- Check mesh updates when modified object noves
	sp.position = [5.04404,-47.2943,26.2671]
	FAssert (CompBB sp 153.829) "Bad modified size"

	-- Check mesh updates when projected mesh moves
	tp.position = [3.84099,-47.8585,-84.0985]
	FAssert (CompBB sp 131.938) "Bad Final size"

	v2 = Cylinder smooth:on heightsegs:50 capsegs:1 sides:18 height:127.604 radius:43.6304 mapcoords:on pos:[59.5356,-139.222,-100] isSelected:on
		-- For some reason, we are not redrawing after creating the cylinder, and that seems to cause crashes on some video cards
	redrawViews()
	bb1 = GetBBSize(v2)
	wsmClone = copy sp.modifiers[1]
	addModifier v2 wsmClone
	bb2 = GetBBSize(v2)
	FAssert (bb1 != bb2) "Clone WSM had no effect"
)