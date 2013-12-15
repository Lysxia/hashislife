hashislife
==========

C implementation of the Hashlife algorithm

---

Project composition
-------------------

- *hashtbl*: Hashtables implement quad tree "smart constructor".

- *hashlife*: Hashlife algorithm, supports arbitrarily large numbers of steps.

- *bigint*: Big integers.

- *darray*: Dynamic arrays.

- *parsers*: Read/write utilities. Currently contain definitions of data types
representing GoL configurations. (Raw and RLE)

- *rleparser*: Run Length Encoder / Decoder.

- *conversion*: Converts to and from quad trees. Lossy formats TODO.

- *lifecount*: Counting cells in a quadtree.

- *slowlife*: Naive cellular automaton simulation. (old)

- *sparsemap*: A data type adapted to Life 1.06 format
and more generally to sparse configurations (in terms of live cells)

- *main*, *Makefile* ...

---

TODO
----

    1. Garbage collecting ? Mark and sweep ? Stop & copy ?

    2. Graphic display (SDL)

