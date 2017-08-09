# dvdpacking

This is a simple command line util that solves the (1d bin packing) problem for trying to efficiently organise a set of files/directories so that they use the least amount of DVD/CDs.

The util will accept list of files/directories and recursively sum each of the input - with all data sizes determined, the tool then applies a set of standard 1d bin packing algorithms (`first fit`, `worst fit` and `best fit` along with their `descending` counterparts) on the input file/directores.

The output is a report of the:
- total number of required bins (as per capacity ie DVD or CD), 
- each bin's contents 
- packing pattern used.

The util will also report if any file/directory can not fit into the target bin capacity.

The user can also specify one of the inbuilt algorithms (via `-p`) to use if they wish.

Target bin *capacity*, via `-c`, is the only mandatory flag and can be specified in bytes or can use shorthands `dvd` or `cd`.

A *reserve*, via `-r`, specified in bytes or as a percentage of the overal capacity, can be requested when performing the bin packing; such a reserve would be useful if you intend to add other documents or generate indexing (such as html index page generated from the data set https://github.com/whatdoineed2do/imgcat) to the final output.

## Example Usage
#### Most likely usage
This is my typical usage - allow the util to apply all inbuilt algorithms to determine least / most suitable bin packing suggestions.
```
$ dvdpacking -r 5% -c dvd _pending 2017*[0-9]
dvdpacking: ttl items=7  ttl size=18542384361 (17.27GiB)  lower bounds=5
dvdpacking:   6119016216  _pending
dvdpacking:   2102344092  2017-02
dvdpacking:   1527565446  2017-03
dvdpacking:   1181908894  2017-04
dvdpacking:   3661438150  2017-05
dvdpacking:   1876670069  2017-06
dvdpacking:   2073441494  2017-07
dvdpacking: ---
dvdpacking: First Fit #bins=4  #unhandled=1 size=6119016216 (5.699GiB)
dvdpacking: [warn] unhandled items = {
dvdpacking: [warn]   6119016216  _pending
dvdpacking: [warn] }
dvdpacking:   bin# 0  size=3629909538 (3.381GiB)  remain=842044280 (803MiB)  {
dvdpacking:     "2017-02"
dvdpacking:     "2017-03"
dvdpacking:   }
dvdpacking:   bin# 1  size=3058578963 (2.849GiB)  remain=1413374855 (1.316GiB)  {
dvdpacking:     "2017-04"
dvdpacking:     "2017-06"
dvdpacking:   }
dvdpacking:   bin# 2  size=3661438150 (3.41GiB)  remain=810515668 (773MiB)  {
dvdpacking:     "2017-05"
dvdpacking:   }
dvdpacking:   bin# 3  size=2073441494 (1.931GiB)  remain=2398512324 (2.234GiB)  {
dvdpacking:     "2017-07"
dvdpacking:   }
dvdpacking: ---
dvdpacking: First Fit Descending #bins=4  #unhandled=1 size=6119016216 (5.699GiB)
dvdpacking: [warn] unhandled items = {
dvdpacking: [warn]   6119016216  _pending
dvdpacking: [warn] }
dvdpacking:   bin# 0  size=2709474340 (2.523GiB)  remain=1762479478 (1.641GiB)  {
dvdpacking:     "2017-04"
dvdpacking:     "2017-03"
dvdpacking:   }
dvdpacking:   bin# 1  size=3950111563 (3.679GiB)  remain=521842255 (497.7MiB)  {
dvdpacking:     "2017-06"
dvdpacking:     "2017-07"
dvdpacking:   }
dvdpacking:   bin# 2  size=2102344092 (1.958GiB)  remain=2369609726 (2.207GiB)  {
dvdpacking:     "2017-02"
dvdpacking:   }
dvdpacking:   bin# 3  size=3661438150 (3.41GiB)  remain=810515668 (773MiB)  {
dvdpacking:     "2017-05"
dvdpacking:   }
dvdpacking: ---
dvdpacking: Worst Fit #bins=4  #unhandled=1 size=6119016216 (5.699GiB)
dvdpacking: [warn] unhandled items = {
dvdpacking: [warn]   6119016216  _pending
dvdpacking: [warn] }
dvdpacking:   bin# 0  size=2073441494 (1.931GiB)  remain=2398512324 (2.234GiB)  {
dvdpacking:     "2017-07"
dvdpacking:   }
dvdpacking:   bin# 1  size=3058578963 (2.849GiB)  remain=1413374855 (1.316GiB)  {
dvdpacking:     "2017-04"
dvdpacking:     "2017-06"
dvdpacking:   }
dvdpacking:   bin# 2  size=3629909538 (3.381GiB)  remain=842044280 (803MiB)  {
dvdpacking:     "2017-02"
dvdpacking:     "2017-03"
dvdpacking:   }
dvdpacking:   bin# 3  size=3661438150 (3.41GiB)  remain=810515668 (773MiB)  {
dvdpacking:     "2017-05"
dvdpacking:   }
dvdpacking: ---
dvdpacking: Worst Fit Descending #bins=4  #unhandled=1 size=6119016216 (5.699GiB)
dvdpacking: [warn] unhandled items = {
dvdpacking: [warn]   6119016216  _pending
dvdpacking: [warn] }
dvdpacking:   bin# 0  size=2102344092 (1.958GiB)  remain=2369609726 (2.207GiB)  {
dvdpacking:     "2017-02"
dvdpacking:   }
dvdpacking:   bin# 1  size=2709474340 (2.523GiB)  remain=1762479478 (1.641GiB)  {
dvdpacking:     "2017-04"
dvdpacking:     "2017-03"
dvdpacking:   }
dvdpacking:   bin# 2  size=3661438150 (3.41GiB)  remain=810515668 (773MiB)  {
dvdpacking:     "2017-05"
dvdpacking:   }
dvdpacking:   bin# 3  size=3950111563 (3.679GiB)  remain=521842255 (497.7MiB)  {
dvdpacking:     "2017-06"
dvdpacking:     "2017-07"
dvdpacking:   }
dvdpacking: ---
dvdpacking: Best Fit #bins=4  #unhandled=1 size=6119016216 (5.699GiB)
dvdpacking: [warn] unhandled items = {
dvdpacking: [warn]   6119016216  _pending
dvdpacking: [warn] }
dvdpacking:   bin# 0  size=2073441494 (1.931GiB)  remain=2398512324 (2.234GiB)  {
dvdpacking:     "2017-07"
dvdpacking:   }
dvdpacking:   bin# 1  size=3058578963 (2.849GiB)  remain=1413374855 (1.316GiB)  {
dvdpacking:     "2017-04"
dvdpacking:     "2017-06"
dvdpacking:   }
dvdpacking:   bin# 2  size=3629909538 (3.381GiB)  remain=842044280 (803MiB)  {
dvdpacking:     "2017-02"
dvdpacking:     "2017-03"
dvdpacking:   }
dvdpacking:   bin# 3  size=3661438150 (3.41GiB)  remain=810515668 (773MiB)  {
dvdpacking:     "2017-05"
dvdpacking:   }
dvdpacking: ---
dvdpacking: Best Fit Descending #bins=4  #unhandled=1 size=6119016216 (5.699GiB)
dvdpacking: [warn] unhandled items = {
dvdpacking: [warn]   6119016216  _pending
dvdpacking: [warn] }
dvdpacking:   bin# 0  size=2102344092 (1.958GiB)  remain=2369609726 (2.207GiB)  {
dvdpacking:     "2017-02"
dvdpacking:   }
dvdpacking:   bin# 1  size=2709474340 (2.523GiB)  remain=1762479478 (1.641GiB)  {
dvdpacking:     "2017-04"
dvdpacking:     "2017-03"
dvdpacking:   }
dvdpacking:   bin# 2  size=3661438150 (3.41GiB)  remain=810515668 (773MiB)  {
dvdpacking:     "2017-05"
dvdpacking:   }
dvdpacking:   bin# 3  size=3950111563 (3.679GiB)  remain=521842255 (497.7MiB)  {
dvdpacking:     "2017-06"
dvdpacking:     "2017-07"
dvdpacking:   }
dvdpacking: ------
dvdpacking: summary:
dvdpacking:     bins= 4  Best Fit
dvdpacking:     bins= 4  Best Fit Descending
dvdpacking:     bins= 4  First Fit
dvdpacking:     bins= 4  First Fit Descending
dvdpacking:     bins= 4  Worst Fit
dvdpacking:     bins= 4  Worst Fit Descending
```
#### Alternative usage
No reserve is needed and we only want to consider the results of bestfit bin packing
```
$ dvdpacking -p bestfit -c dvd _pending 2017*[0-9]
dvdpacking: ttl items=7  ttl size=18542384361 (17.27GiB)  lower bounds=4
dvdpacking:   6119016216  _pending
dvdpacking:   2102344092  2017-02
dvdpacking:   1527565446  2017-03
dvdpacking:   1181908894  2017-04
dvdpacking:   3661438150  2017-05
dvdpacking:   1876670069  2017-06
dvdpacking:   2073441494  2017-07
dvdpacking: ---
dvdpacking: Best Fit #bins=4  #unhandled=1 size=6119016216 (5.699GiB)
dvdpacking: [warn] unhandled items = {
dvdpacking: [warn]   6119016216  _pending
dvdpacking: [warn] }
dvdpacking:   bin# 0  size=2073441494 (1.931GiB)  remain=2633878314 (2.453GiB)  {
dvdpacking:     "2017-07"
dvdpacking:   }
dvdpacking:   bin# 1  size=3058578963 (2.849GiB)  remain=1648740845 (1.536GiB)  {
dvdpacking:     "2017-04"
dvdpacking:     "2017-06"
dvdpacking:   }
dvdpacking:   bin# 2  size=3629909538 (3.381GiB)  remain=1077410270 (1.003GiB)  {
dvdpacking:     "2017-02"
dvdpacking:     "2017-03"
dvdpacking:   }
dvdpacking:   bin# 3  size=3661438150 (3.41GiB)  remain=1045881658 (997.4MiB)  {
dvdpacking:     "2017-05"
dvdpacking:   }
```

## Further Enhancements
If you wish to add other bin packing algorithms to this util, look at the abstract interface `Packer.h` - create a child and implement the `pack(..)` method and finally _register_ an instance of the object to the `bps` and `allbps` objects in `main()` and your algorithm will be automatically used.
