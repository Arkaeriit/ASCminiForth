: 1+ 1 + ;
: 1- 1 - ;
: 2dup swap dup rot dup rot swap ;
: 2drop drop drop ;
: <> = 0= ;
: 0<> 0 <> ;
: > 2dup < 0= rot rot <> and ;
: 0> 0 > ;
: bl s"  " drop c@ ;
: c 1 allot ;
: cell+ 1 cells + ;
: char+ 1+ ;
: chars ;
: max 2dup < if swap then drop ;
: mod /mod drop ;
: / /mod swap drop ;
: negate -1 * ;
: min 2dup max negate + + ;
: nip swap drop ;
: over 2dup drop ;
: tuck dup rot swap ;
: 2over 3 pick 3 pick ;
: 2swap rot >r rot r> ;
: 2>r swap >r >r ;
: 2r> r> r> swap ;
: decimal 10 base ! ;
:macro loop 1 +loop ;
: space bl emit ;
: spaces dup 0> if 0 do space loop else drop then ;
: invert -1 xor ;
: type dup 0> if 0 do dup c@ emit 1+ loop drop else 2drop then ;
:macro while if ;
:macro repeat 0 until then ;
: ?dup dup if dup then ;
:macro ['] ' ;
: +! dup @ rot + swap ! ;
: cell 1 cells ;
: , here cell allot ! ;
: c, here 1 chars allot c! ;
: 2! swap over ! cell+ ! ;
: 2@ dup cell+ @ swap @ ;
: 2* 2 * ;
: 2/ 2 / ;
: aligned begin dup cell mod 0<> while 1+ repeat ;
: align begin here aligned here <> while 1 allot repeat ;
: count dup char+ swap c@ ;
: evaluate (evaluate) s"  " (evaluate) ;
:macro [char] char ;
: hex 16 base ! ;
: 2r@ r> r> 2dup >r >r swap ;
:macro again 0 until ;
: false 0 ;
:macro buffer: align here swap allot constant ;
:macro variable cell buffer: ;
: accept dup 0 > 0= if 2drop 0 exit then
0 begin
key dup 10 = if drop nip nip exit then
( addr n1 n2 c )
swap >r swap >r over c! 1+ r> r> 1+
( addr n1 n2 )
2dup = if nip nip exit then repeat ;

