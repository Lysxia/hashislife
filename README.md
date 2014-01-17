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

- *parsers*: Parsin utilities. Currently just parsing a rule.

- *runlength*: Run Length Encoder / Decoder.

- *matrix*: Raw text format.

- *conversion*: Converts to and from quad trees. Lossy formats TODO.

- *lifecount*: Counting cells in a quadtree.

- *slowlife*: Naive cellular automaton simulation. (old)

- *definitions*: Misc. declarations (currently just one `typedef`)

- *main*, *Makefile* ...

---

TODO
----

    1. Garbage collecting ? Mark and sweep ? Stop & copy ?

    2. Graphic display (SDL)

