package ast

import (
	"fmt"

	"github.com/NicoNex/tau/obj"
)

type Or struct {
	l Node
	r Node
}

func NewOr(l, r Node) Node {
	return Or{l, r}
}

func (o Or) Eval(env *obj.Env) obj.Object {
	var (
		left  = o.l.Eval(env)
		right = o.r.Eval(env)
	)

	if takesPrecedence(left) {
		return left
	}
	if takesPrecedence(right) {
		return right
	}

	return obj.ParseBool(isTruthy(left) || isTruthy(right))
}

func (o Or) String() string {
	return fmt.Sprintf("(%v || %v)", o.l, o.r)
}
