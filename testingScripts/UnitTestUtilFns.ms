fn FAssert b msg = (
	assert (b) message:msg options:#(#dialog)
	if not b do (
		throw "Error running script"
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