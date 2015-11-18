// file      : cli/man.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <vector>
#include <iostream>

#include "man.hxx"

using namespace std;

namespace
{
  static string
  escape_line (const string& s, size_t b, size_t e)
  {
    string r;
    size_t n (e - b);

    // Escaping leading '.' with '\' is not sufficient.
    //
    if (n > 1 && s[b] == '\\' && s[b + 1] == '.')
      r = "\\&";

    r.append (s, b, n);
    return r;
  }

  static void
  wrap_lines (ostream& os, const string& d)
  {
    size_t b (0), e (0), i (0);
    for (size_t n (d.size ()); i < n; ++i)
    {
      // First handle preformatted text (.nf/.fi).
      //
      if (d.compare (i, 4, ".nf\n") == 0 && (i == 0 || d[i - 1] == '\n'))
      {
        assert (b == i); // We should have nothing accumulated.

        // Output everything until (and including) closing .fi as is.
        //
        e = d.find ("\n.fi", i + 4);
        assert (e != string::npos);
        e += 4; // Now points past 'i'.

        os << string (d, i, e - i);

        b = e;
        i = e - 1; // For ++i in loop header.
        continue;
      }

      if (d[i] == ' ' || d[i] == '\n')
        e = i;

      if (d[i] == '\n' || (i - b >= 78 && e != b))
      {
        os << escape_line (d, b, e) << endl;
        b = e = e + 1;
      }
    }

    // Flush the last line.
    //
    if (b != i)
      os << escape_line (d, b, i);
  }

  struct doc: traversal::doc, context
  {
    doc (context& c): context (c) {}

    virtual void
    traverse (type& ds)
    {
      if (ds.name ().compare (0, 3, "doc") != 0) // Ignore doc variables.
        return;

      // n = 1 - common doc string
      // n = 2 - arg string, common doc string
      // n > 2 - arg string, usage string, man string
      //
      size_t n (ds.size ());
      const string& d (n == 1 ? ds[0] : n == 2 ? ds[1] : ds[2]);

      if (d.empty ())
        return;

      std::set<string> arg_set;
      if (n > 1)
        translate_arg (ds[0], arg_set);

      string s (format (ot_man, translate (d, arg_set), true));

      wrap_lines (os, s);
      os << endl;
    }
  };

  struct option: traversal::option, context
  {
    option (context& c) : context (c) {}

    virtual void
    traverse (type& o)
    {
      using semantics::names;

      semantics::doc_strings const& doc (o.doc ());

      if (options.suppress_undocumented () && doc.empty ())
        return;

      names& n (o.named ());

      os << ".IP \"\\fB";

      for (names::name_iterator i (n.name_begin ()); i != n.name_end (); ++i)
      {
        if (i != n.name_begin ())
          os << "\\fR|\\fB";

        os << *i;
      }

      os << "\\fR";

      string type (o.type ().name ());

      std::set<string> arg_set;

      if (type != "bool" || doc.size () >= 3)
      {
        string s (
          translate_arg (
            doc.size () > 0 ? doc[0] : string ("<arg>"), arg_set));

        os << ' ' << format (ot_man, s, false);
      }

      os << "\"" << endl;

      string d;

      // If we have both the long and the short descriptions, use
      // the long one.
      //
      if (type == "bool" && doc.size () < 3)
      {
        if (doc.size () > 1)
          d = doc[1];
        else if (doc.size () > 0)
          d = doc[0];
      }
      else
      {
        if (doc.size () > 2)
          d = doc[2];
        else if (doc.size () > 1)
          d = doc[1];
      }

      // Format the documentation string.
      //
      d = format (ot_man, translate (d, arg_set), false);

      wrap_lines (os, d);
      os << endl;
    }
  };

  //
  //
  struct class_: traversal::class_, context
  {
    class_ (context& c)
        : context (c), option_ (c)
    {
      *this >> inherits_base_ >> *this;
      names_option_ >> option_;
    }

    virtual void
    traverse (type& c)
    {
      if (!options.exclude_base ())
        inherits (c, inherits_base_);

      names (c, names_option_);
    }

  private:
    traversal::inherits inherits_base_;

    option option_;
    traversal::names names_option_;
  };
}

void
generate_man (context& ctx)
{
  traversal::cli_unit unit;
  traversal::names unit_names;
  traversal::namespace_ ns;
  doc dc (ctx);
  class_ cl (ctx);

  unit >> unit_names;
  unit_names >> ns;
  unit_names >> dc;
  unit_names >> cl;

  traversal::names ns_names;

  ns >> ns_names;
  ns_names >> ns;
  ns_names >> dc;
  ns_names >> cl;

  if (ctx.options.class_ ().empty ())
    unit.dispatch (ctx.unit);
  else
  {
    for (vector<string>::const_iterator i (ctx.options.class_ ().begin ());
         i != ctx.options.class_ ().end (); ++i)
    {
      string n (*i);

      // Strip leading :: if present.
      //
      if (n.size () > 2 && n[0] == ':' && n[1] == ':')
        n = string (n, 2, string::npos);

      if (semantics::class_* c = ctx.unit.lookup<semantics::class_> ("", n))
        cl.traverse (*c);
      else
      {
        cerr << "error: class '" << *i << "' not found" << endl;
        throw generation_failed ();
      }
    }
  }
}
