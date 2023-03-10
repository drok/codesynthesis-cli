// file      : specifier/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

// Test specifier functions.
//

#include <string>

#include "test.hxx"

#undef NDEBUG
#include <cassert>

using namespace std;

int
main (int argc, char* argv[])
{
  options o (argc, argv);

  assert (o.a ());
  assert (o.b () == 1 && !o.b_specified ());
  assert (o.c () == "foo" && o.c_specified ());

  o.b_specified (true);
  o.c_specified (false);

  assert (o.b_specified ());
  assert (!o.c_specified ());
}
