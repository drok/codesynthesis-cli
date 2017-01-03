# file      : buildfile
# copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
# license   : MIT; see accompanying LICENSE file

d = cli/ unit-tests/

# Building examples/ and tests/ while bootstrapping the compiler is tricky.
# What we are going to do is omit these two directories if there is no cli
# compiler yet. Once it's built the user can reconfigure the project which
# will enable tests and examples. Alternatively, the user can install the
# compiler and test the installation with out-of-tree builds of tests/ and
# examples/.
#
# Note that to make sure we don't pick up some system-installed cli, we
# default (in bootstrap.build) to leaving the cli module unconfigured.
#
if $cli.configured
  d += tests/ examples/

./: $d doc{INSTALL LICENSE NEWS README version} file{manifest}
include $d

# Don't install examples, tests or the INSTALL file.
#
dir{examples/}:   install = false
dir{tests/}:      install = false
dir{unit-tests/}: install = false
doc{INSTALL}@./:  install = false
