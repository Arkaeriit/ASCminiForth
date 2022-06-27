: 1+ 1 + ;
: 1- 1 - ;
: 2dup swap dup rot dup rot swap ;
: 2drop drop drop ;
: <> = 0= ;
: 0<> 0 <> ;
: > 2dup < 0= rot rot <> and ;
: 0> 0 > ;
