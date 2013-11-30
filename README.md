hashislife
==========

C implementation of the Hashlife algorithm

- 30/01/13 Initializing OKAY.

- 30/11/13 Rewrote hashlife.c and Cie.

    1. for a more spaced writing style (`a+b` -> `a + b`)

    2. the computation of `next` (the configuration in 2^d steps) should be
done by an explicit call rather than by a "smart constructor".

---

- TODO:

    1. Basic functionality would consist in reading a GoL configuration,
computing the resulting configuration in t steps (for large t), write it out.

    2. Garbage collecting ? Mark and sweep ? Stop & copy ?

    3. Multithreading. Hashtable is the main bottleneck. Hashtable is
THE bottleneck, hashlife relies on that entirely.

        a. Split the hashtable in n equal parts, access to distinct parts is
parallelizable. Semaphores look like a pain though...

    4. Graphic display (SDL) (why is that only on 4th position ...)
