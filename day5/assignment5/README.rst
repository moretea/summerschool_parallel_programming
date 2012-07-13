====================================================
 Using GNU make for parallel testing / benchmarking
====================================================

:Date: May 2012
:Authors: kena

.. contents::

Introduction
============

Say we have a collection of programs.

Say we can run these programs using a variety of platform
parameters. Let us call a *profile* a given selection of platform
parameters.

Say each program accepts a set of input files. Two programs may accept
different input files, or some input files may be shared among
multiple programs.

Say we want to automate the execution of all programs over their input
files over the different profiles, ie. execute the cross-product of
programs, inputs and profiles. Say we want to be "smarter" than a
3-nested loop, and also support dependency tracking:

- we want to run the executions in parallel, up to N simultaneously
  running programs.

- when a program is modified, only the test/benchmark results
  dependent on this program should be updated.

- when an input file is modified, only the test/benchmark results
  dependent on this file should be updated.

(We assume that profiles are not files)

Say each program reports a non-zero exit status code when it
encounters an error. We want to distinguish between *testing*, where
any error encountered in a single program should fail the entire test;
and *benchmarking* where errors can be ignored.

Also, typically *testing* is only performed over a single profile,
using a subset of all the possible input files for a given program;
often it is assumed that when a program succeeds on one profile and
over a few input files, it will likely succeed on all of them.

Why using GNU make
------------------

The use case calls for a tool which can automate the execution of commands
according to dependency rules, ie some sort of Make_ program. 

.. _Make: https://en.wikipedia.org/wiki/Make_%28software%29

However, the "standard" [1]_ Make implementations do not support
automatic *rule* generation over more than 1 free variable. Here we
have three (program, input file and profile). Therefore, a solution based on a
standard Make would require a phase of pre-processing to generate a
Makefile containing the cross-product. This would be
unsatisfactory. Indeed, the most expensive part of any Make's execution is
reading the Makefile, and a large Makefile would introduce a
proportionally large overhead for *every invocation of Make*. 

In contrast, our use case above allows the user to re-run Make
multiple times after modifying some programs or input files, so as to
re-generate only the subset of results dependent on them. Having a
large Make invocation overhead would hinder this workflow.

.. [1] the "standard" set of Make features is commonly defined as
   the subset of common features between BSD make, GNU make and NMAKE.

The alternative is to delegate to Make the task of performing the
cross-product of rules, "on demand", *after* the Makefile has been
loaded. While the standard Makefile language does not support this, we
can use GNU make functions for this purpose.

Also, GNU make has native support for parallel execution via its
``-j`` command-line parameter.

The solution / how it works
===========================

So we can author a Makefile which uses GNU make syntax to generate
test/benchmark rules and dependency tracking automatically.

This is attached below in section `The code`_.

We exploit two specific features of GNU make:

- the ``$(foreach ...)`` function, together with ``$(subst ...)`` and ``$(word ...)``

- "Second expansion" of Make expressions, using "``.SECONDEXPANSION``", so that
  the dependency list can be generated *after a base pattern has been matched*.

Note that the use of "second expansion" requires extra care when
reading the Makefile (as a human):

- in the body of a rule, called the *recipe* (where the commands are
  specified), a double ``$`` sign indicates that ``$`` is to be passed
  literally to the recipe shell; whereas

- in the dependency list of a rule, a double ``$`` sign indicates that the expansion
  should be deferred (to the second expansion).

For example::

  .SECONDEXPANSION:
  %.out: $$(word 2,$$(subst /, ,$$@)) $$(word 3,$$(subst /, ,$$@))
       dorun $@

This rule expresses that any output file with a name of the form
``xxx/A/B/xxx.out`` is dependent on both the file named ``A`` and the
file named ``B``.

Compare with::

  %.check:
	$(MAKE) $$(for I in $$(cat "$*.tlist"); do echo "benchdata/$*/$$I/check.out"; done)

In this rule, the ``$$`` in the recipe indicates that one ``$`` is to
be passed to the shell literally; meanwhile, ``$*`` is replaced by Make
by the stem of the rule (the word that matches ``%`` in the rule, for example ``PROG`` in ``PROG.check``).

Details
=======

Mapping of programs to inputs
-----------------------------

We support two schemes to map programs to input files:

- an explicit scheme: for each program ``PROG``, a file
  ``PROG.inputs`` can indicate the input files accepted by ``PROG``
  explicitly. Typically this file would contain the filenames of the
  input files, one per line.

  As a "utility" feature we allow shell wildcard patterns in the
  ``.inputs`` file, so that a single line in that file can relate the
  program to multiple inputs. For example a single line with the text
  ``mydata.{0,2,3,4,5,6,7,8,9}{0,1,2,3,4,5,6,7,8,9}`` could specify
  100 different input files at once, each with a name of the form
  ``mydata.NN``.

- an implicit scheme: for each program ``PROG``, if the file
  ``PROG.inputs`` is not specified, then we consider that any file
  with a name of the form ``PROG.dNNN`` is an input file.

Test inputs
-----------

We mark the subset of the input files that should be used during
testing by requiring that they contain the text "``USE IN MAKE
CHECK``" literally.

Benchmarking is not affected by this text, and always runs over all
available inputs.

Rules
-----

The provided Makefile provides 3 "common" rules:

``check``
   run the cross product of all programs with the input files marked
   for testing, using the single profile named "check".

``bench``
   run the cross product of all programs over all their input files
   over all profiles defined via ``$(BENCH_PROFILES)``.

``clean``
   delete all automatically generated files.

Then it also provides more fine-grained rules, of the form:

``PROG.check`` 
   run the cross product of the specific program ``PROG``
   over the input files marked for testing, using the single profile
   named "check".

``PROG.bench``
   run the cross product of the specific program ``PROG`` over all its
   input files and    over all profiles defined via ``$(BENCH_PROFILES)``.

Intermediate files
------------------

The Makefile generates the following intermediate files:

``PROG.ilist``
   effective list of all input files accepted by ``PROG``. This is
   after the patterns have been matched to the files actually
   present. Each file name that can be used for testing is marked with
   an additional suffix "``.check``.

``PROG.blist``
   same as ``PROG.ilist``, with the text "``.check``"
   removed. Suitable for benchmarking.

``PROG.tlist``
   subset of ``PROG.ilist`` marked with ``.check``, with the text
   "``.check``" removed. Suitable for testing.



Examples
========

Programs and inputs
-------------------

For example, let us consider 3 programs ``progA``, ``progB`` and
``progC``. ``progA`` has its own input files ``progA.d20``,
``progA.d30``, ``progA.d40``. Both ``progB` and ``progC`` share a set
of common inputs ``common.d100``, ``common.d200``,
``common.d300``. The input listing for the test/benchmark directory
would thus be::

   Makefile
   common.d100
   common.d200
   common.d300
   progA
   progA.d20
   progA.d30
   progA.d40
   progB
   progB.inputs
   progC
   progC.inputs

Both ``progB.inputs`` and ``progC.inputs`` are identical and contain a
single line of text::

   common.d*


Also we place the comment ``USE IN MAKE CHECK`` in the files
``progA.d20`` and ``common.d100``, to indicate that testing should
only use those 2 files and no the 4 others.


Execution
---------

For example::

  make -j progB.bench BENCH_PROFILES="p1 p4 p16"

May output the following::

  GEN  progB.ilist
  GEN  progB.blist
  GEN  benchdata/progB/common.d200/p1.out
  GEN  benchdata/progB/common.d300/p4.out
  GEN  benchdata/progB/common.d200/p4.out
  GEN  benchdata/progB/common.d100/p4.out
  GEN  benchdata/progB/common.d300/p1.out
  GEN  benchdata/progB/common.d100/p1.out
  GEN  benchdata/progB/common.d100/p16.out
  GEN  benchdata/progB/common.d200/p16.out
  FAIL benchdata/progB/common.d300/p1.out
  GEN  benchdata/progB/common.d300/p16.out
  FAIL benchdata/progB/common.d200/p1.out

Note that we use the rule ``bench`` so failures to not cause Make to fail. 

In contrast if we use::

  make -j progB.check

We may obtain::

  GEN  progB.tlist
  GEN  benchdata/progB/common.d200/check.out
  Exit status: 1
  Error log:
     Running program 'progB' over input 'common.d200' with profile 'check'...
     An error 617 occurred!
  make[1]: *** [benchdata/progB/common.d200/check.out] Error 1
  make: *** [progB.check] Error 2

With the ``.check`` rule the errors during program execution also cause Make to fail.
(the exact text of the error log is as reported by the program on its
standard output/error streams)

Verbose output
--------------

We also support a verbose flag to enable/disable printing the detailed
recipe commands executed. Compare::

  make bench -j

which outputs a summary like::

  GEN  benchdata/progA/progA.d20/p1.out
  GEN  benchdata/progA/progA.d20/p4.out
  GEN  benchdata/progA/progA.d20/p16.out
  GEN  benchdata/progA/progA.d30/p1.out
  GEN  benchdata/progA/progA.d30/p4.out
  ...

with::

   make bench V=1 -j
    
which outputs the commands in detail::

   make  $(for I in $(cat "progA.blist"); do echo "benchdata/progA/$I/p1.out"; done)
     $(for I in $(cat "progA.blist"); do echo "benchdata/progA/$I/p4.out"; done)
     $(for I in $(cat "progA.blist"); do echo "benchdata/progA/$I/p16.out"; done)
   mkdir -p "benchdata/progA/progA.d20/"
   do_run() { [...]; }; do_run "benchdata/progA/progA.d20/p1.out"
   mkdir -p "benchdata/progA/progA.d30/"
   do_run() { [...]; }; do_run "benchdata/progA/progA.d30/p1.out"
   mkdir -p "benchdata/progA/progA.d40/"
   do_run() { [...]; }; do_run "benchdata/progA/progA.d40/p1.out"
   mkdir -p "benchdata/progA/progA.d20/"
   do_run() { [...]; }; do_run "benchdata/progA/progA.d20/p4.out"
   mkdir -p "benchdata/progA/progA.d30/"
   do_run() { [...]; }; do_run "benchdata/progA/progA.d30/p4.out"
   mkdir -p "benchdata/progA/progA.d40/"
   do_run() { [...]; }; do_run "benchdata/progA/progA.d50/p4.out"
   ...


The code
========

The Makefile:

.. include:: Makefile
   :code: makefile

Note how all Make variables passed to the recipe shell are double
quoted, in case their value contains shell punctuation. Think for
example of a file name containing "``?``" or "``;``".

Here is an example run script ``progrun.sh``:

.. include:: progrun.sh
   :code: sh

