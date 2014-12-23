hashislife
==========

C implementation of the Hashlife algorithm

---

Usage
-----

Use the `make` command in the `src/` directory to compile `hashlife`:

Usage:

    ./hashlife (filename) (t:integer) [h:integer]

where `t`, and optionally `h`, are integer arguments.
(`t` can be arbitrarily big, while `h` must hold on 32-bit)

This will simulate the game of life (with Conway's b3/s23 rule) for `t`
time steps, and display the final state with a (de)zoom level `h`
where one character represents a 2^`h` by 2^`h` area.
(using hex to show density when `h > 0`)

Currently this displayed area is limited to a 32x80 grid, and the top-left
corner is at the same position as in the input file.

The currently supported input formats are:

- raw text matrices (`.txt`), using `'o'` and `'.'` to
    denote respectively alive and dead cells.
    The file can start with any number of comment lines beginning with an
    `'!'`, and is limited to 80 columns overall;

    Example `glider.txt` file:
    
        ! Glider example
        .....
        ..o..
        ...o.
        .ooo.
        .....

- run length encodings (`.rle`).

---

Project composition
-------------------

- `include/`: header files
- `lex/`: `.l` (`lex`) source files
- `main/`: main program entry point
- `patterns/`: input examples
- `src/`: `.c` source files
- `test/`: test programs

<!--
`src/`
------

- *hashtbl*: Hashtables implement quad tree "smart constructor".

- *hashlife*: Hashlife algorithm, supports arbitrarily large numbers of steps.

- *bigint*: Big integers.

- *darray*: Dynamic arrays.

- *parsers*: Parsin utilities. Currently just parsing a rule.

- *runlength*: Run Length Encoder / Decoder.

- *matrix*: Raw text format.

- *conversion*: Converts to and from quad trees.

- *lifecount*: Counting cells in a quadtree.

- *slowlife*: Naive cellular automaton simulation. (old)

- *definitions*: Misc. declarations (currently just one `typedef`)

- *main*, *Makefile* ...
-->

---

Example
-------

    $ cd src
    $ make
    (...)
    $ ./hashlife
    usage: ./hashlife (filename) (t:integer) [h:integer]
    $ ./hashlife ../patterns/glider_gun.txt 1789
    ................................................................................
    ............................O...................................................
    ...........................OOOO.................................................
    ..........OO..............OO.O.O...OO...........................................
    ..O.......O..O...........OOO.O..O..OO...........................................
    .O...OO.......O...........OO.O.O................................................
    .O.....O......O............OOOO.................................................
    ..OOOOO.......O.......O.....O...................................................
    ..........O..O.........O........................................................
    ..........OO.........OOO........................................................
    ................................................................................
    ................................................................................
    ................................................................................
    ................................................................................
    ................................................................................
    ..............................O.................................................
    ............................O.O.................................................
    .............................OO.................................................
    ................................................................................
    ................................................................................
    ................................................................................
    ................................................................................
    .....................................O..........................................
    ......................................O.........................................
    ....................................OOO.........................................
    ................................................................................
    ................................................................................
    ................................................................................
    ................................................................................
    ................................................................................
    .............................................O..................................
    ...........................................O.O..................................

---

TODO
----

1. Garbage collecting ? Mark and sweep ? Stop & copy ?

2. Graphical display (SDL)

