fn FAssert b msg = (
	assert (b) message:msg options:#(#dialog)
)

fn CompArray a b = (a as string == b as string)
fn CompBB anode expSize = (
	bb = (nodeLocalBoundingBox anode)
	size = length (bb[1] - bb[2])
	res =abs(size - expSize) < 1
)