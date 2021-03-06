# Fast-Needleman-Wunsch
This project is focused on optimizing the Needleman-Wunsch and Smith-Waterman algorithms for performing pairwise sequence alignment, specifically by leveraging parallel computing. Avenues of investigation include instruction-level parallelism, multithreading, multiprocessing, and GPU optimizations; if time permits, applying speculative execution to the Smith-Waterman algorithm may be another avenue of effective optimization.

## Included Files

The `clean` branch contains only the code that was found to yield good speedup; all other branches have various ideas that didn't work, plus a bunch of clutter like tuning scripts, benchmarking scripts, old data files, and so on.

The `bdna` directory contains a number of sequences. They are randomly generated and do not correspond to any existing species, except possibly by chance. They are formatted to be reasonably space-efficient, storing one base per byte; gaps are represented by 0, and bases A, T, G, and C by 1, 2, 3, and 4, respectively. 

The `src` directory contains implementations of various optimized Needleman-Wunsch algorithms. `driver.cpp`, `needleman-wunsch.hpp`, `helper.cpp`, and `helper.hpp` are all shared between implementations, with the notable exception that `idxArray2.cpp` uses `driver2.cpp`.

## Implementations

- `serial`: a straightforward, single-threaded implementation. This is the basis of parallel implementations, and the benchmark against which they are evaluated.
- `blocked-serial`: single-threaded, but optimized around the cache.
- `simple-mt`: naive multithreaded approach. Loops through each diagonal strip ([i][0], [i-1][1], [i-2][2], ..., [0][i]), farming the individual elements on that strip out to threads.
- `sentinel-mt`: multithreaded approach using a sentinel value to synchronize threads. The table is pre-populated with INT_MIN, and each thread computes one row. At each step, each thread checks the spot above it to ensure it has been computed; otherwise it waits.
- `sentinel-parfill-mt`: identical to sentinel, but the table is populated with `INT_MIN` in parallel.
- `sentinel-otf-mt`: multithreaded approach that seeds each row with `INT_MIN` on-the-fly rather than all at once.
- `sentinel-nofill-mt`: multithreaded approach that doesn't fill the table, but rather updates only the next spot at each iteration
- `idxarray-mt`: multithreaded approach using an auxiliary array to synchronize and thus avoid the cost of populating the table. Each spot in the array stores the last computed index of that row; threads simply read that array to synchronize.
- `idxarray-emb-mt`: same as idxarray, except the indexing information is embedded in the table itself; column 0 stores that index info, and the rest of the table is offset by 1. Improves locality.
- `idxarray-mod-mt`: uses modular arithmetic to reduce the size of the array to `2 * nThreads`. Row `i` is computed by thread `i % nThreads`, so each thread simply needs to look at the thread before it to synchronize; the array is double the number of threads so that information about some row can be preserved while the thread that worked on it moves on.
