
: test-strlen ." Printing 12: " S" 123456789ABC" strlen . CR ;
: test-print ." Testing print: " S" OK" print CR ;

: benchmark test-strlen test-print ;
benchmark

