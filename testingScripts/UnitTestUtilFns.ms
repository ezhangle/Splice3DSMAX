fn FAssert b msg = (
	assert (b) message:msg options:#(#dialog)
)

fn CompArray a b = (a as string == b as string)
fn GetBBSize anode = (
	bb = (nodeLocalBoundingBox anode)
	size = length (bb[1] - bb[2])
)
fn CompBB anode expSize = (
	res =abs(BBSize(anode) - expSize) < 1
)