// file      : cli/html.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
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
    doc (context& c, class_doc_type cd, bool& l)
        : context (c), cd_ (cd), list_ (l) {}

    virtual void
    traverse (type& ds)
    {
      if (ds.name ().compare (0, 3, "doc") != 0) // Ignore doc variables.
        return;

      // n = 1 - common doc string
      // n = 2 - arg string, common doc string
      // n > 2 - arg string, short string, long string
      //
      size_t n (ds.size ());
      string d (
        n == 1
        ? (cd_ == cd_short ? first_sentence (ds[0]) : ds[0])
        : (n == 2
           ? (cd_ == cd_short ? first_sentence (ds[1]) : ds[1])
           : ds[cd_ == cd_short ? 1 : 2]));

      if (options.ascii_tree ())
        preprocess_ascii_tree (d);

      std::set<string> arg_set;
      if (n > 1)
        translate_arg (ds[0], arg_set);

      unsigned short t (toc); // Detect the switch to/from TOC mode.

      string s (
        format (ds.scope (), escape_html (translate (d, arg_set)), true));

      if (s.empty ())
        return;

      if (list_)
      {
        os << "  </dl>" << endl
           << endl;
        list_ = false;
      }

      wrap_lines (os, s, (t || toc) ? 0 : 2); // TOC mode does its own thing.

      if (!toc) // TOC mode does its own thing.
        os << endl
           << endl;
    }

  private:
    class_doc_type cd_;
    bool& list_; // True if we are currently in <dl>.
  };

  struct option: traversal::option, context
  {
    option (context& c, class_doc_type cd, bool& l)
        : context (c), cd_ (cd), list_ (l) {}

    virtual void
    traverse (type& o)
    {
      using semantics::names;

      semantics::doc_strings const& doc (o.doc ());

      if (options.suppress_undocumented () && doc.empty ())
        return;

      if (toc)
        return; // No option documentation in the TOC mode.

      if (!list_)
      {
        os << "  <dl class=\"options\">" << endl;
        list_ = true;
      }
      else
        os << endl; // Separate from the previous <dl>.

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
        // Note: we naturally assume this doesn't need --ascii-tree treatment.
        //
        string s (
          translate_arg (
            doc.size () > 0 ? doc[0] : string ("<arg>"), arg_set));

        os << ' ' << format (o.scope (), escape_html (s), false);
      }

      os << "</dt>" << endl;

      string d;
      if (type == "bool" && doc.size () < 3)
      {
        if (doc.size () > 1)
          d = doc[cd_ == cd_short ? 0 : 1];
        else if (doc.size () > 0)
          d = (cd_ == cd_short ? first_sentence (doc[0]) : doc[0]);
      }
      else
      {
        if (doc.size () > 2)
          d = doc[cd_ == cd_short ? 1 : 2];
        else if (doc.size () > 1)
          d = (cd_ == cd_short ? first_sentence (doc[1]) : doc[1]);
      }

      if (options.ascii_tree ())
        preprocess_ascii_tree (d);

      // Format the documentation string.
      //
      d = format (o.scope (), escape_html (translate (d, arg_set)), false);

      wrap_lines (os, "<dd>" + d + "</dd>", 4);
      os << endl;
    }

  private:
    class_doc_type cd_;
    bool& list_; // True if we are currently in <dl>.
  };

  //
  //
  struct class_: traversal::class_, context
  {
    class_ (context& c, bool& l): context (c), list_ (l), base_ (false)
    {
      *this >> inherits_ >> *this;
    }

    virtual void
    traverse (type& c)
    {
      class_doc_type cd (class_doc (c));

      if (cd == cd_exclude || (base_ && cd == cd_exclude_base))
        return;

      if (!options.exclude_base () && !options.include_base_last ())
      {
        bool ob (base_);
        base_ = true;
        inherits (c);
        base_ = ob;
      }

      doc dc (*this, cd, list_);
      option op (*this, cd, list_);
      traversal::names n;
      n >> dc;
      n >> op;
      names (c, n);

      if (!options.exclude_base () && options.include_base_last ())
      {
        bool ob (base_);
        base_ = true;
        inherits (c);
        base_ = ob;
      }
    }

  private:
    bool& list_;
    bool base_;
    traversal::inherits inherits_;
  };
}

void
generate_html (context& ctx)
{
  bool list (false);

  traversal::cli_unit unit;
  traversal::names unit_names;
  traversal::namespace_ ns;
  doc dc (ctx, cd_default, list);
  class_ cl (ctx, list);
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

  if (list)
    ctx.os << "  </dl>" << endl
           << endl;
}
