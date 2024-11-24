variable (current-arg)
1 (current-arg) !
: shift-args ( -- ) 1 (current-arg) +! ;
: next-arg   ( -- addr u ) (current-arg) @ dup argc @ 1- > if drop 0 0 else arg shift-args then ;
: (bye) ( n -- ) exit-code ! bye ;
