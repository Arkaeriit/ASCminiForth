# ASC miniForth

A simple Forth implementation. Aimed at being embedded in other projects or in embedded systemps.

This is a realy early work in progess.

## Differences with other Forth

### Dictionary

Unlike other Forth implementation, there is only a single entry for each word. This means that if you change the definition of a word, this will impact all the previously defined words that used the word being modified. On the plus side, making a recursive function is very easy to do. For example: `: rec-func ( n -- ) 1 - DUP 0< 0= IF ." Hello! " rec-func ELSE DROP THEN ;`

### Memory management

The traditional memory management of other Forth is completely ignored here. The memory is instead managed like in C. The word `ALLOT` takes a number of bytes and allocate it from the heap. A pointer to that memory is put on the stack. The memory should them be freed using `FREE`.

```Forth
1 CELLS ALLOT \ 1 cell is allocated on the heap
DUP 33 SWAP ! DUP @ . \ The memory is used
FREE \ The memory is freed when not needed anymore
```

For the sake of compatibility with other Forth, the word `HERE` is included but does not do anything. This means that you can use constructs such as `HERE 1 CELLS ALLOT` to write code compatible with other Forth.

### Cell size

To make the language easier, only one type of number is supported. There is no number using two cells. Furthermore, as the cell should be able to contain raw pointers, it should be large enough. The default type for the cell defined in `amf_config.h` is `intptr_t`.

