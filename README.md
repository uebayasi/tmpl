tmpl - simple template scanner

SYNTAX

`~{foo~}` is replaced with the value of `foo`.

`~{foo~=a~}` sets `foo` to `a`.

`~{foo~=a~=b~=c~}` sets `foo` to `a`, `a` to `b`, and `b` to `c`.

`~{i~@foo~@xxx~{i~}xxx~}` expands `xxx~{i~}xxx` with `i` set to values of `foo`.
