: compare ( c-addr1 u1 c-addr2 u2 -- n ) rot 2dup = if
    drop 0 do
        ( c-addr1 c-addr2 )
        dup c@ swap 1+ swap rot dup c@ swap 1+ swap rot
        ( c-addr2 c-addr1 c2 c2)
        = 0= if 2drop -1 unloop exit then loop
        ( c-addrX c-addrY )
        2drop 0
    else 2drop 2drop 1 then ;
: cmove ( c-addr1 c-addr2 u -- ) dup 0> 0= if drop else
    0 do swap dup c@ rot dup rot swap c! 1+ swap 1+ swap loop then 2drop ;

