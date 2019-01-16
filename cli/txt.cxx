// file      : cli/txt.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <vector>
#include <cstring>  // strlen()
#include <iostream>

#include <cli/txt.hxx>

using namespace std;

size_t
txt_size (const string& s, size_t p, size_t n)
{
  size_t r (0);

  n = n == string::npos ? s.size () : n + p;

  // The start position (p) might be pointing half-way into the
  // escape sequence. So we always have to scan from the start.
  //
  for (size_t i (0), m (s.size ()); i < n; ++i)
  {
    if (s[i] == '\033') // ANSI escape: "\033[Nm"
    {
      i += 3;
      assert (i < m && s[i] == 'm');
      continue;
    }

    if (i >= p)
      ++r;
  }

  return r;
}

void
txt_wrap_lines (ostream& os,
                string& d,
                size_t indent,
                size_t first,
                const char* line_start,
                const char* line_end,
                const char* line_blank,
                string (*escape) (string const&))
{
  assert (!d.empty ());

  os << string (indent - first, ' ');

  // Count the number of leading spaces at the beginning of each line.
  // Then use it as an extra indentation if we are breaking this line.
  // This makes multi-line definition lists look decent. Note that this
  // doesn't work for ordered/unordered lists because they start on the
  // same line as number/bullet. However, there is hack to make it work:
  // break the first line at the 80 characters boundary explicitly with
  // \n.
  //
  size_t wc (0), wi (0); // Count and count-based indentation.
  bool cws (true);       // Count flag.

  // @@ TODO: while this should support notes inside lists, lists inside
  //    notes will require splitting indentation into before/after '|'.
  //
  const char* ne ("");   // Note.
  const char* np ("");   // Note prefix.

  size_t b (0), e (0), i (0);
  for (size_t n (d.size ()); i < n; ++i)
  {
    char& c (d[i]);

    // Translate note character.
    //
    if (c == 0x07)
    {
      c = '|';
      ne = "| ";
    }

    if (c == ' ' || c == '\n')
      e = i;

    if (c == '\n' || txt_size (d, b, i - b) == 79 - indent - wi - strlen (np))
    {
      if (b != 0) // Not a first line.
        os << endl
           << line_start << string (indent + wi, ' ') << np;

      string s (d, b, (e != b ? e : i) - b);

      if (escape != 0)
        s = escape (s);

      os << s << line_end;

      // Handle consecutive newlines (e.g., pre, paragraph separator).
      //
      if (c == '\n')
      {
        for (; i + 1 < n && d[i + 1] == '\n'; e = ++i)
          os << endl
             << line_blank;
      }

      b = e = (e != b ? e : i) + 1;

      // Start indenting/noting beginning with the next break.
      //
      wi = wc;
      np = ne;
    }

    if (c == '\n')
    {
      // Reset counters, clear notes, and start counting.
      //
      wc = wi = 0;
      ne = np = "";
      cws = true;
    }
    else if (cws)
    {
      if (c == ' ')
        ++wc;
      else
        cws = false;
    }
  }

  // Flush the last line.
  //
  if (b != i)
  {
    if (b != 0)
      os << endl
         << line_start << string (indent + wi, ' ') << np;

    string s (d, b, i - b);

    if (escape != 0)
      s = escape (s);

    os << s << line_end;
  }
}

namespace
{
  struct doc: traversal::doc, context
  {
    doc (context& c, class_doc_type cd): context (c), cd_ (cd) {}

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
      const string& d (
        n == 1
        ? (cd_ == cd_short ? first_sentence (ds[0]) : ds[0])
        : (n == 2
           ? (cd_ == cd_short ? first_sentence (ds[1]) : ds[1])
           : ds[cd_ == cd_short ? 1 : 2]));

      std::set<string> arg_set;
      if (n > 1 && options.ansi_color ())
        translate_arg (ds[0], arg_set);

      string s (format (ds.scope (), translate (d, arg_set), true));

      if (s.empty ())
        return;

      txt_wrap_lines (os, s);
      os << endl; //@@ ??
    }

  private:
    class_doc_type cd_;
  };

  struct option: traversal::option, context
  {
    option (context& c, class_doc_type) : context (c)/*, cd_ (cd)*/ {}

    virtual void
    traverse (type& o)
    {
      using semantics::names;

      semantics::doc_strings const& doc (o.doc ());

      if (options.suppress_undocumented () && doc.empty ())
        return;

      //@@ TODO
    }

  private:
    // class_doc_type cd_;
  };

  //
  //
  struct class_: traversal::class_, context
  {
    class_ (context& c): context (c), base_ (false)
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

      doc dc (*this, cd);
      option op (*this, cd);
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
    bool base_;
    traversal::inherits inherits_;
  };
}

void
generate_txt (context& ctx)
{
  traversal::cli_unit unit;
  traversal::names unit_names;
  traversal::namespace_ ns;
  doc dc (ctx, cd_default);
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
