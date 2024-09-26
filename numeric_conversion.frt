64 constant <#-buff-len
variable <#-buff <#-buff-len allot
variable <#-cnt

: <# 0 <#-cnt ! ;
: (#) ( u -- c ) dup 9 > if 10 - char a + else char 0 + then ;
: <#-addr ( -- addr ) <#-buff <#-buff-len + <#-cnt @ - ;
: hold 1 <#-cnt +! <#-addr c! ;
: holds begin dup while 1- 2dup + c@ hold repeat 2drop ;
: sign 0 < if char - hold then ;
: # base @ u/mod swap (#) hold ;
: #s begin # dup 0= until ;
: #> drop <#-addr <#-cnt @ ;
: . <# dup abs #s swap sign #> type space ;
: u. <# #s #> type space ;

