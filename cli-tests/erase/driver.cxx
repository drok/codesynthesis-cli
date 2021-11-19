// file      : erase/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

// Test argument erasing.
//

#include <string>

#include "test.hxx"

#undef NDEBUG
#include <cassert>

using namespace std;

int
main (int argc, char* argv[])
{
  options o (argc, argv, true,
             cli::unknown_mode::skip,
             cli::unknown_mode::skip);

  assert (o.a ());
  assert (o.b () == 123);

  // We should have 'foo bar --arg -- -b 234'.
  //
  assert (argc == 7);
  assert (argv[1] == string ("foo"));
  assert (argv[2] == string ("bar"));
  assert (argv[3] == string ("--arg"));
  assert (argv[4] == string ("--"));
  assert (argv[5] == string ("-b"));
  assert (argv[6] == string ("234"));
}
