// file      : tests/inheritance/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

// Test option class inheritance.
//

#include <string>
#include <cassert>
#include <iostream>

#include "test.hxx"

using namespace std;

int
main (int argc, char* argv[])
{
  const cli::options& d (options::description ());

  assert (d.size () == 4);
  assert (d[0].name () == "--very-long-flag");
  assert (d[1].name () == "-i");
  assert (d[2].name () == "-s");
  assert (d[3].name () == "--string");

  options o (argc, argv);

  assert (o.very_long_flag ());
  assert (o.s () == "short");
  assert (o.i () == 123);
  assert (o.string () == "long");

  options::print_usage (cout);
}
