package obj

import "strconv"

type Boolean bool

func NewBoolean(b bool) Object {
	return &Boolean(b)
}

func (b Boolean) String() string {
	return strconv.FormatBool(bool(b))
}

func (b Boolean) Type() Type {
	return BOOL
}

func (b Boolean) Val() bool {
	return bool(b)
}
