( This file is meant to try words that are not available in ciforth )

: is_true IF ." OK. " ELSE ." Failed! " THEN ;
: is_0 0= IF ." OK. " ELSE ." Failed! " THEN ;

( Arguments )
: test.arg argc 0> if ." Printing arguments: " argc 0 do i arg type space loop cr then ;

( Strings)
: test.compare ." Testing compare: " S" various" S" size" compare S" SAME SIZE" S" same size" compare S" the same" S" the same" compare 0= and and is_true cr ;

: benchmark 11 22 33
test.arg
test.compare
." Printing 33: " . cr ;

benchmark

