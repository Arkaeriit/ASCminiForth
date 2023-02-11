: compare ( addr1 u1 addr2 u2 -- n ) rot 2dup = if 
    drop 0 swap 0 do
        drop 2dup c@ swap c@ = invert if 1 leave then 1+ swap 1+ 0 loop
    swap drop swap drop
    else 1 then ;
: cmove ( c-addr1 c-addr2 u -- ) dup 0> 0= if exit then
    0 do swap dup @ rot dup rot swap ! 1+ swap 1+ swap loop ;

