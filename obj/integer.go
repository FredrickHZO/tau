package obj

import "strconv"

type Integer int64

func NewInteger(i int64) Object {
	return &Integer(i)
}

func (i Integer) String() string {
	return strconv.FormatInt(int64(i), 10)
}

func (i Integer) Type() Type {
	return INT
}

func (i Integer) Val() int64 {
	return int64(i)
}
