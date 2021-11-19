// file      : merge/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

// Test parsed options merging.
//

#include <string>

#include "test.hxx"

#undef NDEBUG
#include <cassert>

using namespace std;

template <typename T, int N1, int N2>
static T
merge (const char* (&av1)[N1], const char* (&av2)[N2])
{
  int ac1 (N1);
  int ac2 (N2);
  T o1 (ac1, const_cast<char**> (av1));
  T o2 (ac2, const_cast<char**> (av2));
  o1.merge (o2);
  return o1;
}

int
main ()
{
  // Basics.
  //
  {
    const char* a1[] = {"",       "-i=123",           "-v=1", "-v=2"};
    const char* a2[] = {"", "-b", "-i=456", "-s=xyz", "-v=3", "-v=4"};
    derived r (merge<derived> (a1, a2));

    assert (r.b ());
    assert (r.i_specified () && r.i () == 456);
    assert (r.s_specified () && r.s () == "xyz");
    assert (r.v_specified () && r.v ().size () == 4 &&
            r.v ()[0] == 1 &&
            r.v ()[1] == 2 &&
            r.v ()[2] == 3 &&
            r.v ()[3] == 4);
  }

  // Default value does not override.
  //
  {
    const char* a1[] = {"", "-i=456"};
    const char* a2[] = {""          };
    derived r (merge<derived> (a1, a2));
    assert (r.i_specified () && r.i () == 456);
  }
}
