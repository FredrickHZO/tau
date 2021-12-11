package ast

import (
	"fmt"
	"github.com/NicoNex/tau/compiler"
	"github.com/NicoNex/tau/obj"
)

type Dot struct {
	l Node
	r Node
}

func NewDot(l, r Node) Node {
	return Dot{l, r}
}

func (d Dot) Eval(env *obj.Env) obj.Object {
	var left = d.l.Eval(env)

	if takesPrecedence(left) {
		return left
	}

	if assertTypes(left, obj.ClassType) {
		l := left.(obj.Class)
		o, ok := l.Get(d.r.String())
		if !ok {
			return obj.NewUndefined(l, d.r.String())
		}
		return o
	}

	return obj.NewError("%v object has no attribute %s", left.Type(), d.r)
}

func (d Dot) String() string {
	return fmt.Sprintf("%v.%v", d.l, d.r)
}

func (d Dot) Compile(c *compiler.Compiler) (position int) {
	return 0
}
