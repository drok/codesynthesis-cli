// file      : cli/html.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <vector>
#include <iostream>

#include "html.hxx"

using namespace std;

namespace
{
  static string
  escape_html (string const& s)
  {
    string r;
    r.reserve (s.size ());

    for (size_t i (0), n (s.size ()); i < n; ++i)
    {
      switch (s[i])
      {
      case '<':
        {
          r += "&lt;";
          break;
        }
      case '&':
        {
          r += "&amp;";
          break;
        }
      default:
        {
          r += s[i];
          break;
        }
      }
    }

    return r;
  }

  static void
  wrap_lines (ostream& os, const string& d, size_t indent)
  {
    size_t lim (78 - indent);
    string ind (indent, ' ');

    bool nl (true); // True if last written to os character is a newline.

    size_t b (0), e (0), i (0);
    for (size_t n (d.size ()); i < n; ++i)
    {
      // First handle <pre>.
      //
      if (d.compare (i, 5, "<pre>") == 0)
      {
        // Write what might have already accumulated.
        //
        if (b != i)
        {
          if (nl)
            os << ind;

          os << string (d, b, i - b);
          nl = false;
        }

        // Output everything until (and including) closing </pre> as is.
        //
        e = d.find ("</pre>", i + 5);
        assert (e != string::npos);
        e += 6; // Now points past '>'.

        if (nl)
          os << ind;

        os << string (d, i, e - i);

        b = e;
        i = e - 1; // For ++i in loop header.
        nl = false;
        continue;
      }

      if (d[i] == ' ' || d[i] == '\n')
        e = i;

      if (d[i] == '\n' || (i - b >= lim && e != b))
      {
        if (nl && b != e)
          os << ind;

        os << string (d, b, e - b) << endl;

        b = e = e + 1;
        nl = true;
      }
    }

    // Write the last line.
    //
    if (b != i)
    {
      if (nl)
        os << ind;

      os << string (d, b, i - b);
    }
  }

  struct doc: traversal::doc, context
  {
    doc (context& c) : context (c) {}

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

      string s (format (ot_html, escape_html (translate (d, arg_set)), true));

      wrap_lines (os, s, 2);
      os << endl
         << endl;
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

      os << "    <dt><code><b>";

      for (names::name_iterator i (n.name_begin ()); i != n.name_end (); ++i)
      {
        if (i != n.name_begin ())
          os << "</b></code>|<code><b>";

        os << escape_html (*i);
      }

      os << "</b></code>";

      string type (o.type ().name ());

      std::set<string> arg_set;

      if (type != "bool" || doc.size () >= 3)
      {
        string s (
          translate_arg (
            doc.size () > 0 ? doc[0] : string ("<arg>"), arg_set));

        os << ' ' << format (ot_html, escape_html (s), false);
      }

      os << "</dt>" << endl;

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
      d = format (ot_html, escape_html (translate (d, arg_set)), false);

      wrap_lines (os, "<dd>" + d + "</dd>", 4);
      os << endl
         << endl;
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
      if (!options.exclude_base () && !options.include_base_last ())
        inherits (c, inherits_base_);

      if (!c.names_empty ())
      {
        os << "  <dl class=\"options\">" << endl;
        names (c, names_option_);
        os << "  </dl>" << endl
           << endl;
      }

      if (!options.exclude_base () && options.include_base_last ())
        inherits (c, inherits_base_);
    }

  private:
    traversal::inherits inherits_base_;

    option option_;
    traversal::names names_option_;
  };
}

void
generate_html (context& ctx)
{
  traversal::cli_unit unit;
  traversal::names unit_names;
  traversal::namespace_ ns;
  doc dc (ctx);
  class_ cl (ctx);

  unit >> unit_names;
  unit_names >> dc;
  unit_names >> ns;
  unit_names >> cl;

  traversal::names ns_names;

  ns >> ns_names;
  ns_names >> dc;
  ns_names >> ns;
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
