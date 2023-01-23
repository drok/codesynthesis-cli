// file      : cli/lexer.test.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <fstream>
#include <iostream>

#include "token.hxx"
#include "lexer.hxx"

#undef NDEBUG
#include <cassert>

using namespace std;

const char* keywords[] =
{
  "source",
  "include",
  "namespace",
  "class",
  "signed",
  "unsigned",
  "bool",
  "char",
  "wchar_t",
  "short",
  "int",
  "long",
  "float",
  "double"
};

const char* punctuation[] = {
  ";", ",", ":", "::", "{", "}", /*"(", ")",*/ "=", "|"};

int
main (int argc, char* argv[])
{
  if (argc != 2)
  {
    cerr << "usage: " << argv[0] << " file.cli" << endl;
    return 1;
  }

  ifstream ifs;
  ifs.exceptions (ifstream::failbit | ifstream::badbit);
  ifs.open (argv[1]);

  lexer l (ifs, argv[1]);

  while (true)
  {
    token t (l.next ());

    switch (t.type ())
    {
    case token::t_eos:
      {
        cout << "<EOS>" << endl;
        return 0;
      }
    case token::t_keyword:
      {
        cout << "keyword: " << keywords[t.keyword ()] << endl;
        break;
      }
    case token::t_identifier:
      {
        cout << "identifier: " << t.identifier () << endl;
        break;
      }
    case token::t_punctuation:
      {
        cout << punctuation[t.punctuation ()] << endl;
        break;
      }
    case token::t_cxx_path_lit:
      {
        cout << "c++ path: " << t.literal () << endl;
        break;
      }
    case token::t_cli_path_lit:
      {
        cout << "cli path: " << t.literal () << endl;
        break;
      }
    case token::t_string_lit:
      {
        cout << t.literal () << endl;
        break;
      }
    case token::t_char_lit:
      {
        cout << t.literal () << endl;
        break;
      }
    case token::t_bool_lit:
      {
        cout << t.literal () << endl;
        break;
      }
    case token::t_int_lit:
      {
        cout << t.literal () << endl;
        break;
      }
    case token::t_float_lit:
      {
        cout << t.literal () << endl;
        break;
      }
    case token::t_call_expr:
      {
        cout << t.expression () << endl;
        break;
      }
    case token::t_template_expr:
      {
        cout << t.expression () << endl;
        break;
      }
    }
  }
}
