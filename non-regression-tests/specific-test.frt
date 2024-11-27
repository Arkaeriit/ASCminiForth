( This file is meant to try words that are not available in ciforth )

: is_true IF ." OK. " ELSE ." Failed! " THEN ;
: is_0 0= IF ." OK. " ELSE ." Failed! " THEN ;

( Arguments )
: test.arg argc @ 1- 0> if ." Printing arguments: " argc @ 1- 0 do i 1+ arg type space loop cr then ;

( Strings)
: test.compare ." Testing compare: " S" various" S" size" compare S" SAME SIZE" S" same size" compare S" the same" S" the same" compare 0= and and is_true cr ;
: test.string-size-with-new-lines ." Testing string size " S" 123 " 4 = swap drop S" 1
" swap drop 2 = S" \ " swap drop 2 = is_true is_true is_true cr ;
: test.holds ." Testing " <# 10 hold s" OK." holds s" holds: " holds 0 #> type ;
hex
: test.escaped-s s\" Testing s\\\" assuming 32 bit or 64 bit little endian: " type s\" \a\e\\\xfc\z\z\z\z" drop @ fc5c1b07 = is_true s\" \x66\z\na" nip 4 = is_true s\" \n" type ;
decimal

( C strings )
: test-strlen ." Testing strlen: " s" 123456789ABC" drop strlen 12 = is_true cr ;
: test-print ." Testing print: " s" OK" drop print cr ;

( Programming tools )
: test.bye ." Testing bye: " 1 is_true CR bye ." Not Okay at all, abbort everything.!!!" CR ;

( Files )
S" content in the file" drop constant _cif
: cif ( -- caddr n ) _cif _cif strlen ;
: test-file-name s" test.txt" ;
: test.write ." Testing writting to a file: " test-file-name w/o create-file is_0 dup >r cif r> write-file is_0 close-file is_0 cr ;
1024 constant buf-size
: test.read ." Testing reading from file: " buf-size allocate is_0 dup dup buf-size test-file-name r/o open-file is_0 dup >r read-file is_0 r> close-file is_0 cif compare is_0 cr free drop ;

( macro )
s" 1 is_true cr ;" macro-string end-word
:macro (test.macro) ." Testing macros: " end-word ;
: test.macro (test.macro)

( stack manipulation )
: test.tuck ." Testing tuck " 1 2 3 tuck 3 = >r 2 = >r 3 = >r 1 = r> r> r> and and and is_true cr ;

( evaluation )
: test.(evaluate) ." Testing (evaluate) " s" 7" (evaluate) s" 5" (evaluate) s" 6" (evaluate) s"  " (evaluate) 756 = is_true cr ;


: benchmark 11 22 33
test.arg
test-strlen test-print
test.compare test.holds test.escaped-s test.string-size-with-new-lines
test.write test.read
test.macro
test.tuck
test.(evaluate)
." Testing stack state: " 33 = is_true cr
;

benchmark
test.bye

