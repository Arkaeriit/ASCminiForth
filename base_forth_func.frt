( ------------------------------- Flow control ------------------------------- )

:macro while ( n -- ) if ;
:macro repeat ( -- ) 0 until then ;
:macro loop ( -- ) 1 +loop ;
:macro again ( -- ) 0 until ;

( ---------------------------- Stack manipulation ---------------------------- )

: 2dup ( x1 x2 -- x1 x2 x1 x2 )swap dup rot dup rot swap ;
: 2drop ( x1 x2 -- ) drop drop ;
: nip ( x1 x2 -- x2 ) swap drop ;
: over ( x1 x2 -- x1 x2 x1 ) 2dup drop ;
: tuck ( x1 x2 -- x2 x1 x2 ) dup rot swap ;
: 2over ( x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2 ) 3 pick 3 pick ;
: 2swap ( x1 x2 x3 x4 -- x3 x4 x1 x2) rot >r rot r> ;
: 2>r ( x1 x2 -- ) ( -- x1 x2 ) swap >r >r ;
: 2r> ( -- x1 x2 ) ( x1 x2 -- ) r> r> swap ;
: ?dup ( n1 -- n1 | [n1 n1] )dup if dup then ;
: 2r@ ( -- x1 x2 ) ( x1 x2 -- x1 x2 ) r> r> 2dup >r >r swap ;

( ----------------------------------- Math ----------------------------------- )

: 1+ ( n -- n ) 1 + ;
: 1- ( n -- n ) 1 - ;
: <> ( x1 x2 -- b ) = 0= ;
: 0<> ( n -- b ) 0 <> ;
: > ( n1 n2 -- b ) 2dup < 0= rot rot <> and ;
: 0> ( n1 -- b ) 0 > ;
: max ( n1 n2 -- n1 | n2 ) 2dup < if swap then drop ;
: /mod ( n1 n2 -- n3 n4 ) sm/rem ;
: mod ( n n -- n ) /mod drop ;
: / ( n n -- n ) /mod swap drop ;
: */mod ( n n n -- n n ) >r * r> /mod ;
: */ ( n n n -- n ) >r * r> / ;
: negate ( n -- n ) -1 * ;
: min ( n1 n2 -- n1 | n2 ) 2dup max negate + + ;
: invert ( n -- n ) -1 xor ;
: +! ( n addr -- ) dup @ rot + swap ! ;
: 2* ( n -- n ) 2 * ;
: 2/ ( n -- n ) 2 / ;
:macro s>d ( n1 -- n1 ) ;
:macro d>s ( n1 -- n1 ) ;
: u> ( u u -- b ) 2dup u< 0= rot rot <> and ;
: within ( test low high -- flag ) over - >r - r> u< ;

( ----------------------------- Memory management ---------------------------- )

: c ( -- ) 1 allot ;
: cell+ ( n -- n ) 1 cells + ;
: char+ ( n -- n ) 1+ ;
: chars ( n -- n ) ;
: cell ( -- n ) 1 cells ;
: , ( x -- ) here cell allot ! ;
: c, ( c -- ) here 1 chars allot c! ;
: 2! ( x x addr -- ) swap over ! cell+ ! ;
: 2@ ( addr -- x x ) dup cell+ @ swap @ ;
: aligned ( addr -- addr ) begin dup cell mod 0<> while 1+ repeat ;
: align ( -- ) begin here aligned here <> while 1 allot repeat ;
:macro buffer: ( n -- ) ( consume a name ) align here swap allot constant ;
:macro variable ( -- ) ( consume a name ) cell buffer: ;

( ---------------------------------- Display --------------------------------- )

: decimal ( -- ) 10 base ! ;
: bl ( -- c ) s"  " drop c@ ;
: space ( -- ) bl emit ;
: spaces ( n -- ) dup 0> if 0 do space loop else drop then ;
: type ( addr n -- ) dup 0> if 0 do dup c@ emit 1+ loop drop else 2drop then ;
:macro [char] ( -- c ) ( consume a name ) char ;
: hex ( -- ) 16 base ! ;

( --------------------------------- Execution -------------------------------- )

:macro ['] ( -- xt ) ( consume a name ) ' ;
: evaluate ( addr n -- ? ) (evaluate) s"  " (evaluate) ;

( ---------------------------------- Strings --------------------------------- )

: count ( addr -- addr n ) dup char+ swap c@ ;
: accept ( addr n -- n ) dup 0 > 0= if 2drop 0 exit then
0 begin
key dup 10 = if drop nip nip exit then
( addr n1 n2 c )
swap >r swap >r over c! 1+ r> r> 1+
( addr n1 n2 )
2dup = if nip nip exit then repeat ;

( ----------------------------------- Misc. ---------------------------------- )

: false ( -- b ) 0 ;

( ---------------------------- Numeric conversion ---------------------------- )

64 constant <#-buff-len
variable <#-buff <#-buff-len allot
variable <#-cnt

: <# ( -- ) 0 <#-cnt ! ;
: (#) ( u -- c ) dup 9 > if 10 - char a + else char 0 + then ;
: <#-addr ( -- addr ) <#-buff <#-buff-len + <#-cnt @ - ;
: hold ( c -- ) 1 <#-cnt +! <#-addr c! ;
: holds ( addr n -- ) begin dup while 1- 2dup + c@ hold repeat 2drop ;
: sign ( n -- ) 0 < if char - hold then ;
: # ( u -- u ) base @ u/mod swap (#) hold ;
: #s ( u -- u ) begin # dup 0= until ;
: #> ( u -- ) drop <#-addr <#-cnt @ ;
: . ( n -- ) <# dup abs #s swap sign #> type space ;
: u. ( u -- ) <# #s #> type space ;

