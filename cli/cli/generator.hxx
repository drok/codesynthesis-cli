// file      : cli/generator.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_GENERATOR_HXX
#define CLI_GENERATOR_HXX

#include <vector>

#include <cli/options.hxx>
#include <cli/semantics/unit.hxx>

class generator
{
public:
  generator ();

  class failed {};

  void
  generate (options&,
            semantics::cli_unit&&,
            std::vector<semantics::path>&& dependencies,
            semantics::path const&);

private:
  generator (generator const&);

  generator&
  operator= (generator const&);
};

#endif // CLI_GENERATOR_HXX
