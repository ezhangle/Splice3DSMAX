resetMaxFile #noPrompt

-- Create the solver object.
customIKSolver = SpliceMatrixController()
customIKSolver.addOutputPort "transforms" "Xfo" isArray:true extension:"RiggingToolbox"
customIKSolver.addInputPort "root" "Mat44" 17
customIKSolver.addInputPort "iktarget" "Mat44" 17
customIKSolver.OutPort = "SpliceMatrixControllerVal"

boneLengths = #(50.1, 40.1, 30.1)
customIKSolver.addInputPort "bone0Length" "Scalar" -2
customIKSolver.bone0Length = boneLengths[1]
customIKSolver.addInputPort "bone1Length" "Scalar" -2
customIKSolver.bone1Length = boneLengths[2]
customIKSolver.addInputPort "bone2Length" "Scalar" -2
customIKSolver.bone2Length = boneLengths[3]

-- Init base compilation
klCode =  "
require Math;
require RiggingToolbox;

operator solveIK(
   in Mat44 root,
   in Mat44 iktarget,
	io Xfo transforms[],
	Scalar bone0Length,
	Scalar bone1Length,
	Scalar bone2Length,
	io Mat44 SpliceMatrixControllerVal
) {
   report(bone0Length);
	UInt32 numJoints = 4;// including a 'nub' joint.
	Xfo basePose[];
	basePose.resize(numJoints);
	basePose[0] = root;
	basePose[1] = basePose[0] * Xfo(Vec3(0.0, 0.0, bone0Length), Quat(Euler(0.0, 0.4, 0.0)));
	basePose[2] = basePose[1] * Xfo(Vec3(0.0, 0.0, bone1Length), Quat(Euler(0.0, 0.4, 0.0)));
	basePose[3] = basePose[2] * Xfo(Vec3(0.0, 0.0, bone2Length), Quat(Euler(0.0, 0.4, 0.0)));

	transforms = solveNCFIK(basePose, iktarget.translation());
}"
customIKSolver.SetKLCode "solveIK" klCode
root = point()
root.name = "root"
root.pos = [0,0,60]
customIKSolver.root = root

target = point()
target.name = "target"
target.pos = [82,0,0]
customIKSolver.iktarget = target


fn createJoint name id length = (

	spliceCtrl = SpliceMatrixController()
	spliceCtrl.addInputPort "transforms" "Xfo" -1 isArray:true
	spliceCtrl.addInputPort "id" "Size" -2
	spliceCtrl.OutPort = "SpliceMatrixControllerVal"
	klCode = "require Math;

	operator SpliceMatrixController(
	  io Mat44 SpliceMatrixControllerVal, 
	  in Mat44 parentValue,
	  in Xfo transforms[],
	  in Size id
	) {
	  if(id < transforms.size)
			SpliceMatrixControllerVal = transforms[id].toMat44();
	}
	";
	spliceCtrl.SetKLCode "SpliceMatrixController" klCode
	spliceCtrl.id = id
	b = Box()
	b.length = 5
	b.width = 5
	b.height = length
	b.name = name
	b.transform.controller = spliceCtrl

	spliceCtrl.ConnectPorts "transforms"  customIKSolver "transforms"
)

createJoint "Joint0" 0 boneLengths[1]
createJoint "Joint1" 1 boneLengths[2]
createJoint "Joint2" 2 boneLengths[3]


-- create a helper just to hold the ik controller so we can inspect its UI.
customIKSolver = Point()
customIKSolver[3].controller = customIKSolver
