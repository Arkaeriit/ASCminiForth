( This file contain various test function used to validate )
( the words from ASCminiForth that are complient to the Forth standard )
( This file is meant to be tested with ASCminiForth and ciforth )

: TEST.. ." Printing 0: " 0 . CR ;
: is_true IF ." OK. " ELSE ." Failed! " THEN ;
: is_0 0= IF ." OK. " ELSE ." Failed! " THEN ;

( FLOW CONTROL TEST )
: TEST.IF ." Testing flow control. " 0 IF ." Failed!" ELSE ." OK." THEN 1 IF ."  OK." ELSE ."  Failed!" THEN CR ;
: TEST.BEGIN.LOOP ." Testing begin loop. " 2 5 BEGIN SWAP 2 * SWAP 1 - DUP 0= UNTIL DROP 64 = is_true CR ;
: TEST.NESTED.IF ." Testing nested ifs. " 0 IF 0 ELSE 1 IF 0 IF 0 ELSE 1 THEN ELSE 0 THEN THEN is_true CR ;
: (TEST.COMPLEX.IF) ( -- 77 ) 0 IF 1 IF 1 EXIT ELSE 2 EXIT THEN 3 EXIT ELSE 77 EXIT THEN 4 EXIT ;
: TEST.COMPLEX.IF ." Testing complex nested ifs " (TEST.COMPLEX.IF) 77 = is_true CR ;
: TEST.NESTED.LOOP ." Testing nested loops. " 0 5 BEGIN SWAP 10 BEGIN SWAP 2 + SWAP 1 - DUP 0= UNTIL DROP SWAP 1 - DUP 0= UNTIL DROP 100 = is_true CR ;
: TEST.+LOOP ." Testing +loop. " 0 10 0 DO I + LOOP 45 = 0 10 0 DO -10 0 DO 1+ -1 +LOOP LOOP 110 = AND is_true CR ;
: TEST.LEAVE ." Testing leave " 10 0 DO I LEAVE LOOP 0= 77 20 0 DO LEAVE 10 0 DO I LOOP LOOP 77 = AND is_true CR ;
: TEST.WHILE ." Testing while loops " 88 BEGIN 0 WHILE 99 EXIT REPEAT 88 = is_true 2 0 BEGIN DUP 10 = 0= WHILE 1+ SWAP 1+ SWAP REPEAT 10 = is_true 12 = is_true  CR ;
: TEST.PACKED.ELSE ." Testing packed else " 98 0 IF 99 ELSE THEN 98 = is_true 20 1 IF 97 ELSE THEN 97 = is_true CR DROP ;
: TEST.PACKED.IF ." Testing packed if " 1 IF ELSE THEN 0 IF ELSE THEN 1 IF THEN 0 IF THEN 1 is_true CR ;
: TEST.?DO ." Testing ?do " 0 10 0 ?DO 1+ LOOP 10 = is_true 5 5 ?DO 0 is_true LOOP CR ;

( BOOLEAN LOGIC )
: TEST.0= ." Testing 0= " -0 0= is_true CR ;
: TEST.0< ." Testing 0< " -1 0< is_true CR ;
: TEST.= ." Testing = " 987 987 = is_true CR ;
: TEST.AND ." Testing and " 2 1 AND 0= 1 1 AND 1 = 0 1 AND 0= AND AND is_true CR ;
: TEST.OR ." Testing or " 2 1 OR 3 = 1 1 OR 1 = 0 1 OR 1 = AND AND is_true CR ;
: TEST.XOR ." Testing xor " 2 1 XOR 3 = 1 1 XOR 0 = 0 1 XOR 1 = AND AND is_true CR ;
: TEST.INVERT ." Testing invert " 6 INVERT -7 = 0 INVERT -1 = AND is_true CR ;

( STACK MANIPULATION )
: TEST.DROP ." Testing drop " 0 1 DROP is_0 CR ;
: TEST.SWAP ." Testing swap " 0 1 SWAP is_0 DROP CR ;
: TEST.ROT ." Testing rot " 0 1 1 ROT is_0 DROP DROP CR ;
: TEST.DUP ." Testing dup " 1 0 DUP DROP is_0 DROP CR ;
: TEST.2D ." Testing 2dup and 2drop " 1 0 2DUP 2DROP is_0 DROP CR ;
: TEST.R ." Testing >r and r> " 12 16 >R 12 = 16 R> = AND is_true CR ;
: TEST.NIP ." Testing nip " 1 2 3 NIP + 4 = is_true CR ;
: TEST.OVER ." Testing over " 1 2 3 OVER + + + 8 = is_true CR ;
: TEST.?DUP ." Testing ?dup " 4 5 ?DUP + 10 = is_true DROP 8 0 ?DUP + 8 = is_true CR ;
: TEST.2OVER ." Testing 2over " 1 2 3 4 2OVER 2 = is_true 1 = is_true 4 = is_true 3 = is_true 2 = is_true 1 = is_true cr ;
: TEST.2SWAP ." Testing 2swap " 1 2 3 4 2SWAP 2 = is_true 1 = is_true 4 = is_true 3 = is_true cr ;
: TEST.DEPTH ." Testing depth " DEPTH 3 = is_true 1 2 3 7 DEPTH = is_true DROP DROP DROP CR ;

( BASIC MATH )
: TEST.+ ." Testing + " -5 5 + is_0 CR ;
: TEST.* ." Testing * " 3 4 * 12 = is_true CR ;
: TEST.*/ ." Testing */ " 6 3 9 */ 2 = is_true CR ;
: TEST.*/MOD ." Testing */mod " 6 3 10 */MOD 1 = is_true 8 = is_true CR ;
: TEST./ ." Testing / " 9 3 / 3 = is_true CR ;
: TEST./MOD ." Testing /mod " 9 3 /MOD 3 = is_true is_0 CR ;
: TEST.< ." Testing < " 4 1 < is_0 1 4 < is_true 4 4 < is_0 CR ;
: TEST.> ." Testing > " 4 1 > is_true 1 4 > is_0 4 4 < is_0 CR ;
: TEST.MAX ." Testing max " 4 5 MAX 5 = 88 66 MAX 88 = AND is_true CR ;
: TEST.MIN ." Testing min " 4 5 MIN 4 = 88 66 MIN 66 = AND is_true CR ;
: TEST.NEGATE ." Testing negate " 0 NEGATE 0 = 5 NEGATE -5 = -88 NEGATE 88 = AND AND is_true CR ;
: TEST.+! ." Testing +! " HERE 1 CELLS ALLOT DUP DUP 55 SWAP ! 45 SWAP +! @ 100 = is_true CR ;
: TEST.SM/REM ." Testing sm/rem "
     10.  7 SM/REM  1 = is_true  3 = is_true
    -10.  7 SM/REM -1 = is_true -3 = is_true
     10. -7 SM/REM -1 = is_true  3 = is_true
    -10. -7 SM/REM  1 = is_true -3 = is_true CR ;
: TEST.FM/MOD ." Testing fm/mod "
     0.  1 FM/MOD  0 = is_true  0 = is_true
     1.  1 FM/MOD  1 = is_true  0 = is_true
     2.  1 FM/MOD  2 = is_true  0 = is_true
    -1.  1 FM/MOD -1 = is_true  0 = is_true
    -2.  1 FM/MOD -2 = is_true  0 = is_true
     0. -1 FM/MOD  0 = is_true  0 = is_true
     1. -1 FM/MOD -1 = is_true  0 = is_true
     2. -1 FM/MOD -2 = is_true  0 = is_true
    -1. -1 FM/MOD  1 = is_true  0 = is_true
    -2. -1 FM/MOD  2 = is_true  0 = is_true
     2.  2 FM/MOD  1 = is_true  0 = is_true
    -1. -1 FM/MOD  1 = is_true  0 = is_true
    -2. -2 FM/MOD  1 = is_true  0 = is_true
     7.  3 FM/MOD  2 = is_true  1 = is_true
     7. -3 FM/MOD -3 = is_true -2 = is_true
    -7.  3 FM/MOD -3 = is_true  2 = is_true
    -7. -3 FM/MOD  2 = is_true -1 = is_true CR ;
: TEST.U>.U< ." Testing u< and u> " -1 5 U< is_0 -1 5 U> is_true 2 6 U< is_true 8 8 U> is_0 CR ;
: TEST.U/MOD ." Testing u/mod " -5 16 U/MOD 16 * INVERT 15 = is_true 11 = is_true CR ;
: TEST.WITHIN ." Testing within " 1 -5 5 WITHIN is_true 3 4 5 WITHIN is_0 CR ;
: TEST.SHIFT ." Testing lshift and rshift " 5 3 LSHIFT 40 = is_true 80 4 RSHIFT 5 = is_true CR ;

( MEMORY MANAGEMENT )
: TEST.MEM ." Testing memory management " HERE 1 CELLS ALLOT DUP 5456 SWAP ! @ 5456 = is_true CR ;
: TEST.CMEM ." Testing char memory management " HERE 1 ALLOT DUP 12 SWAP C! C@ 12 = is_true CR ;
: TEST., ." Testing , " HERE 1564 , @ 1564 = is_true CR ;
: TEST.C, ." Testing C, " HERE 64 C, C@ 64 = is_true CR ;
: TEST.ALIGN ." Testing aligned and align " ALIGN HERE ALIGNED HERE = is_true 1 ALLOT HERE ALIGN HERE <> is_true ALIGN HERE ALIGNED HERE = is_true CR ;
: TEST.UNUSED ." Testing unused " UNUSED 30 ALLOT UNUSED - 30 = is_true CR ;

( BASE )
HEX
: reg-10 10 ;
A BASE !
: TEST.BASE_RECORD ." Testing recording in non-10 base " reg-10 16 = is_true CR ;
: TEST.BASE_PRINT ." Printing 10: " 16 16 BASE ! . 10 BASE ! CR ; 

( IO )
: TEST.EMIT ." Testing emit and S " S" OK." DROP DUP C@ EMIT 1+ DUP C@ EMIT 1+ C@ EMIT CR ;
: TEST.BL ." Testing bl " S"  " DROP C@ BL = is_true CR ;

( VARIABLE AND CONSTANT )
777 CONSTANT sev
VARIABLE var
888 var !
: TEST.CONSTANT ." Testing constant " sev 777 = is_true CR ;
: TEST.VARIABLE ." Testing variable " var @ 888 = is_true CR ;

( EXECUTION TOKENS )
: give44 44 ;
VARIABLE g44xt
' give44 g44xt !
: TEST.EXECUTE ." Testing execute " g44xt @ EXECUTE 44 = is_true CR ;
: recursive-fib DUP 2 < IF EXIT THEN DUP 1- RECURSE SWAP 2 - RECURSE + ;
: TEST.RECURSE ." Testing recurse " 20 recursive-fib 6765 = is_true CR ;
:NONAME ." OK." CR ;
CONSTANT noname-xt
: TEST.NONAME ." Testing :noname " noname-xt execute ;

( EVALUATION )
: p33-def s" : p33 33 ;" ;
: p44 44 ;
: p44-33 s" p33-def evaluate" evaluate s" p44" evaluate s" p33" evaluate ;
: TEST.EVALUATE ." Testing evaluate " p44-33 33 = is_true 44 = s" is_true CR" evaluate ;

( STRINGS )
: TEST.TYPE ." Testing type " S" OK." TYPE CR ;
: TEST.CMOVE ." Testing cmove " S" OK." DUP >R HERE DUP >R SWAP DUP ALLOT CMOVE R> R> TYPE CR ;
: TEST.STRING-SIZE ." Testing string size " S" 123 " 4 = SWAP DROP S" 1 " SWAP DROP 2 = S" \ " SWAP DROP 2 = is_true is_true is_true CR ;
: TEST.STRING-BASE ." Testing strings in non decimal base " 8 BASE ! ." OK." CR DECIMAL ;
: TEST.COUNT ." Testing count " S" abc" DROP COUNT 97 = is_true COUNT 98 = is_true COUNT 99 = is_true DROP CR ;
: TEST.CHAR ." Testing [char] " [CHAR] a S" a" DROP C@ = is_true CR ;
: .. <# #s #> type space ;
: TEST.NUMERIC_CONVERSION ." Printing 7865: " 7865. .. CR ;
: TEST.COMPILE-TIME-PRINT .( Testing .(: OK.) ; CR


: BENCHMARK 11 22 33 TEST..
TEST.IF TEST.BEGIN.LOOP TEST.NESTED.IF TEST.COMPLEX.IF TEST.NESTED.LOOP TEST.+LOOP TEST.LEAVE TEST.WHILE TEST.PACKED.ELSE TEST.PACKED.IF TEST.?DO
TEST.0= TEST.0< TEST.= TEST.AND TEST.OR TEST.XOR TEST.INVERT
TEST.DROP TEST.SWAP TEST.ROT TEST.DUP TEST.2D TEST.R TEST.NIP TEST.OVER TEST.?DUP TEST.2OVER TEST.2SWAP TEST.DEPTH
TEST.+ TEST.* TEST.*/ TEST.*/MOD TEST./ TEST./MOD TEST.< TEST.> TEST.MAX TEST.MIN TEST.NEGATE TEST.+! TEST.SM/REM TEST.FM/MOD TEST.U>.U< TEST.U/MOD TEST.WITHIN TEST.SHIFT
TEST.MEM TEST.CMEM TEST., TEST.C, TEST.ALIGN TEST.UNUSED
TEST.BASE_RECORD TEST.BASE_PRINT
TEST.EMIT TEST.BL
TEST.CONSTANT TEST.VARIABLE
TEST.EXECUTE TEST.EVALUATE TEST.RECURSE TEST.NONAME
TEST.TYPE TEST.CMOVE TEST.STRING-SIZE TEST.STRING-BASE TEST.COUNT TEST.CHAR TEST.NUMERIC_CONVERSION
." Testing stack state: " 33 = is_true CR ;

BENCHMARK BYE

