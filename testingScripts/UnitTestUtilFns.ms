fn FAssert b msg = (
	assert (b) message:msg options:#(#dialog)
	if not b do (
		throw ("Error running script: " + msg)
	)
	true
)

fn CompArray a b = (a as string == b as string)
fn GetBBSize anode = (
	bb = (nodeLocalBoundingBox anode)
	size = length (bb[1] - bb[2])
)
fn CompBB anode expSize = (
	res =abs(GetBBSize(anode) - expSize) < 1
)
global ss
-- Returns true if stack length is greater than x
fn StackHasDepth depth =
(
	-- Add 1 to account for the stack frome of this function
	global searchstr = "[stack level: " + (depth + 1) as string + "]\n"
	global ss = stringstream ""
	stack showLocals:false firstFrameOnly:false to:ss
	seek ss 0 --go to start of stream
	skipToString ss searchstr --test if frame is present
	hasDepth = not (eof ss)
)

-- Return true if the caller is at root level
fn IsRootLevel=
(
	isRoot = not StackHasDepth 2

)
fn TestRootLevel=
(
	res = IsRootLevel()
	print res
)
-- Execute the passed unit test function, and return result
fn TestFunction pfn doReset:true= 
(
	try (
		print ("Running: " + (pfn as string))
		pfn()
		
		if doReset == true then 
		(
			print " - Resetting Scene"
			-- Always reset at the end of the test
			resetMaxFile #noPrompt
			gc()
		)
	)
	catch
	(
		format "*** % ***\n" (getCurrentException())
		return false;
	)
	
	-- If we were here, it was a successful test
	print "--- All Passed ---!"
)