1024 constant (repl-buffer-size)
(repl-buffer-size) allocate drop 1+ dup constant (repl-buffer) 0 swap 1- c!
: (rep) ." ok " (repl-buffer) (repl-buffer-size) accept (repl-buffer) swap 2dup
    + 1- c@ 4 = if 1
    else evaluate 0 then ;
: (repl) begin (rep) until ;
(repl)

