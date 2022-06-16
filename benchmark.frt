( This file contain various test function used to validate )
( the words from ASCminiForth that are complient to the Forth standard )

: TEST.. ." Printing 0: " 0 . CR ;
: is_true IF ." OK. " ELSE ." Failed! " THEN ;
: is_0 0= IF ." OK. " ELSE ." Failed! " THEN ;

( FLOW CONTROL TEST )
: TEST.IF ." Testing flow control. " 0 IF ." Failed!" ELSE ." OK." THEN 1 IF ."  OK." ELSE ."  Failed!" THEN CR ;
: TEST.BEGIN.LOOP ." Testing begin loop. " 2 5 BEGIN SWAP 2 * SWAP 1 - DUP 0= UNTIL DROP 64 = is_true CR ;
: TEST.NESTED.IF ." Testing nested ifs. " 0 IF 0 ELSE 1 IF 0 IF 0 ELSE 1 THEN ELSE 0 THEN THEN is_true CR ;
: TEST.NESTED.LOOP ." Testing nested loops. " 0 5 BEGIN SWAP 10 BEGIN SWAP 2 + SWAP 1 - DUP 0= UNTIL DROP SWAP 1 - DUP 0= UNTIL DROP 100 = is_true CR ;

( BOOLEAN LOGIC )
: TEST.0= ." Testing 0= " -0 0= is_true CR ;
: TEST.0< ." Testing 0< " -1 0< is_true CR ;
: TEST.= ." Testing = " 987 987 = is_true CR ;
: TEST.AND ." Testing AND " 2 1 AND 0= 1 1 AND 1 = 0 1 AND 0= AND AND is_true CR ;

( STACK MANIPULATION )
: TEST.DROP ." Testing drop " 0 1 DROP is_0 CR ;
: TEST.SWAP ." Testing swap " 0 1 SWAP is_0 DROP CR ;
: TEST.ROT ." Testing rot " 0 1 1 ROT is_0 DROP DROP CR ;
: TEST.DUP ." Testing dup " 1 0 DUP DROP is_0 DROP CR ;
: TEST.2D ." Testing 2dup and 2drop " 1 0 2DUP 2DROP is_0 DROP CR ;
: TEST.R ." Testing >r and r> " 12 16 >R 12 = 16 R> = AND is_true CR ;

( BASIC MATH )
: TEST.+ ." Testing + " -5 5 + is_0 CR ;
: TEST.* ." Testing * " 3 4 * 12 = is_true CR ;
: TEST.*/ ." Testing */ " 6 3 9 */ 2 = is_true CR ;
: TEST.*/MOD ." Testing */mod " 6 3 10 */MOD 1 = is_true 8 = is_true CR ;
: TEST./ ." Testing / " 9 3 / 3 = is_true CR ;
: TEST./MOD ." Testing /mod " 9 3 /MOD 3 = is_true is_0 CR ;

( MEMORY MANAGEMENT )
: TEST.MEM ." Testing memory management " HERE 1 CELLS ALLOT DUP 5456 SWAP ! @ 5456 = is_true CR ;
: TEST.CMEM ." Testing char memory management " HERE 1 ALLOT DUP 12 SWAP C! C@ 12 = is_true CR ;

( BASE )
16 BASE !
: reg-10 10 ;
A BASE !
: TEST.BASE_RECORD ." Testing recording in non-10 base " reg-10 16 = is_true CR ;
: TEST.BASE_PRINT ." Printing 10: " 16 16 BASE ! . 10 BASE ! CR ; 

( IO )
: TEST.EMIT ." Testing emit and S " S" OK." DROP DUP C@ EMIT 1+ DUP C@ EMIT 1+ C@ EMIT CR ;

: BENCHMARK 11 22 33 TEST.. TEST.IF TEST.BEGIN.LOOP TEST.NESTED.IF TEST.NESTED.LOOP TEST.0= TEST.0< TEST.= TEST.AND TEST.DROP TEST.SWAP TEST.ROT TEST.DUP TEST.2D TEST.R TEST.+ TEST.* TEST.*/ TEST.*/MOD TEST./ TEST./MOD TEST.MEM TEST.CMEM TEST.BASE_RECORD TEST.BASE_PRINT TEST.EMIT
." Printing 33: " . CR ;

BENCHMARK 

