==============HOW TO BUILD AND RUN THE PROGRAM===============

type "make" to run the program
type "make text" to run the program with a preconstructed number of inputs
type "make test" to run the test on buisnees_logic.c
type "make clean" if you need to remove .o files

=============================================================
===================TOP FUNCTION CALLS========================
""""""""""""""""""""""Used callgrind"""""""""""""""""""""""""

Top function call when we sent in more than 20 merches, did a
bunch of replenishing and adding to carts a< nd then removed all
of the merches:

TOP OVERALL:
296,976 vfprintf (no surprises here)
283,680 fileops.c:_IO_file_xsputn@@GLIBC_2.2.5 (no idea what this is)
102,398 fileops.c:_IO_file_overflow@@GLIBC_2.2.5 (same with this)
86,579 msort.c:msort_with_tmp (could it be the alphabetical sorter "qsort"?)


TOP OF OUR FUNCTIONS:
42,375 string_hash (not surprising since all but one of the hash tables keys are strings)
36,788 create_array_of_merch (function that sorts the merches alphabetically)
33,544 create_array_of_keys (same function but in interface_inputs (they're static))
23,225 ioopm_list_merchendise (gets called by every function that lists the merches)
19,349 ask_question (we ask a lot)

=============================================================
======================LINE COVERAGE==========================
""""""""""""Line coverage from buisness_logic_tests.c""""""""

File 'buisness_logic.c'
Lines executed: 100% of 178 (of the ioopm functions we we're meant to test, calculated manually)
Branches executed: 76.34% of 93 (number low since we have a lot of static functions we never test)
Taken at least once: 72.04% of 93 (same reason)
Calls executed: 85.29% of 68 (same reason)

============================================================
