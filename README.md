tmpl - simple template scanner

SYNTAX

`~{foo~}` is replaced with the value of `foo` (expand)

`~{foo~}` expands `~{foo~}` when `foo` is not defined (unexpand)

`~{foo~=a~}` sets `foo` to `a` (global)

`~{foo~=a~=b~=c~}` sets `foo` to `a`, `a` to `b`, and `b` to `c` (global list)

`~{i~@foo~@xxx~{i~}xxx~}` expands `xxx~{i~}xxx` with `i` set to values of `foo` (template)

`xxx~{i~|hoge~|~{i~}~}xxx~{i~}xxx` expands `xxxhogexxx~{i~}xxx` (local)

`~{i~/x~/axbxc~/xxx~{i~}xxx ~}` expands `xxxaxxxx xxxbxxx xxxcxxx ` (split)
