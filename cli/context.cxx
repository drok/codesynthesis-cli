// file      : cli/context.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <stack>
#include <vector>
#include <cstring>  // strncmp()
#include <fstream>
#include <sstream>
#include <iostream>

#include "context.hxx"

using namespace std;

namespace
{
  char const* keywords[] =
  {
    "NULL",
    "and",
    "asm",
    "auto",
    "bitand",
    "bitor",
    "bool",
    "break",
    "case",
    "catch",
    "char",
    "class",
    "compl",
    "const",
    "const_cast",
    "continue",
    "default",
    "delete",
    "do",
    "double",
    "dynamic_cast",
    "else",
    "end_eq",
    "enum",
    "explicit",
    "export",
    "extern",
    "false",
    "float",
    "for",
    "friend",
    "goto",
    "if",
    "inline",
    "int",
    "long",
    "mutable",
    "namespace",
    "new",
    "not",
    "not_eq",
    "operator",
    "or",
    "or_eq",
    "private",
    "protected",
    "public",
    "register",
    "reinterpret_cast",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "static_cast",
    "struct",
    "switch",
    "template",
    "this",
    "throw",
    "true",
    "try",
    "typedef",
    "typeid",
    "typename",
    "union",
    "unsigned",
    "using",
    "virtual",
    "void",
    "volatile",
    "wchar_t",
    "while",
    "xor",
    "xor_eq"
  };
}

context::
context (ostream& os_,
         semantics::cli_unit& unit_,
         options_type const& ops)
    : data_ (new (shared) data),
      os (os_),
      unit (unit_),
      options (ops),
      modifier (options.generate_modifier ()),
      specifier (options.generate_specifier ()),
      inl (data_->inl_),
      opt_prefix (options.option_prefix ()),
      opt_sep (options.option_separator ()),
      cli (data_->cli_),
      reserved_name_map (options.reserved_name ()),
      keyword_set (data_->keyword_set_)
{
  if (options.suppress_usage ())
    usage = ut_none;
  else
  {
    if (options.long_usage ())
      usage = options.short_usage () ? ut_both : ut_long;
    else
      usage = ut_short;
  }

  if (!options.suppress_inline ())
    data_->inl_ = "inline ";

  data_->cli_ = options.cli_namespace ();

  if (!cli.empty () && cli[0] != ':')
    data_->cli_ = "::" + data_->cli_;

  for (size_t i (0); i < sizeof (keywords) / sizeof (char*); ++i)
    data_->keyword_set_.insert (keywords[i]);
}

context::
context (context& c)
    : data_ (c.data_),
      os (c.os),
      unit (c.unit),
      options (c.options),
      modifier (c.modifier),
      specifier (c.specifier),
      usage (c.usage),
      inl (c.inl),
      opt_prefix (c.opt_prefix),
      opt_sep (c.opt_sep),
      cli (c.cli),
      reserved_name_map (c.reserved_name_map),
      keyword_set (c.keyword_set)
{
}

string context::
escape (string const& name) const
{
  typedef string::size_type size;

  string r;
  size n (name.size ());

  // In most common cases we will have that many characters.
  //
  r.reserve (n);

  for (size i (0); i < n; ++i)
  {
    char c (name[i]);

    if (i == 0)
    {
      if (!((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_'))
        r = (c >= '0' && c <= '9') ? "cxx_" : "cxx";
    }

    if (!((c >= 'a' && c <= 'z') ||
          (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') ||
          c == '_'))
      r += '_';
    else
      r += c;
  }

  if (r.empty ())
    r = "cxx";

  // Custom reserved words.
  //
  reserved_name_map_type::const_iterator i (reserved_name_map.find (r));

  if (i != reserved_name_map.end ())
  {
    if (!i->second.empty ())
      return i->second;
    else
      r += L'_';
  }

  // Keywords
  //
  if (keyword_set.find (r) != keyword_set.end ())
  {
    r += '_';

    // Re-run custom words.
    //
    i = reserved_name_map.find (r);

    if (i != reserved_name_map.end ())
    {
      if (!i->second.empty ())
        return i->second;
      else
        r += L'_';
    }
  }

  return r;
}

string context::
translate_arg (string const& s, std::set<string>& set)
{
  string r;
  r.reserve (s.size ());
  set.clear ();

  size_t p (string::npos);

  for (size_t i (0), n (s.size ()); i < n; ++i)
  {
    if (p == string::npos && s[i] == '<')
    {
      p = i;
      r += "\\i{";
      continue;
    }

    if (p != string::npos && s[i] == '>')
    {
      set.insert (string (s, p + 1, i - p - 1));
      r += '}';
      p = string::npos;
      continue;
    }

    if (p != string::npos && s[i] == '}' && s[i - 1] != '\\')
    {
      r += "\\}";
      continue;
    }

    r += s[i];
  }

  return r;
}

string context::
translate (string const& s, std::set<string> const& set)
{
  string r;
  r.reserve (s.size ());

  size_t p (string::npos);

  for (size_t i (0), n (s.size ()); i < n; ++i)
  {
    if (p == string::npos && s[i] == '<')
    {
      p = i;
      continue;
    }

    if (p != string::npos)
    {
      if (s[i] == '>')
      {
        string a (s, p + 1, i - p - 1);

        if (set.find (a) != set.end ())
        {
          r += "\\i{";

          for (size_t j (0), n (a.size ()); j < n; ++j)
          {
            if (a[j] == '}' && (j == 0 || a[j - 1] != '\\'))
              r += "\\}";
            else
              r += a[j];
          }

          r += '}';
        }
        else
        {
          r += '<';
          r += a;
          r += '>';
        }
        p = string::npos;
      }
      continue;
    }

    r += s[i];
  }

  // If we found the opening '<' but no closing '>', add the rest.
  //
  if (p != string::npos)
    r.append (s, p, string::npos);

  return r;
}

void context::
format_line (output_type ot, string& r, const char* s, size_t n)
{
  bool color (options.ansi_color ());

  typedef unsigned char block; // Mask.

  const block code = 1;
  const block itlc = 2;
  const block bold = 4;
  const block link = 8;

  vector<block> blocks;

  string link_target;
  string link_section; // If not empty, man section; target is man name.
  bool link_empty; // Link has no text.

  bool escape (false);
  for (size_t i (0); i < n; ++i)
  {
    char c (s[i]);

    if (escape)
    {
      bool new_block (false);

      switch (c)
      {
      case '-':
        {
          // N-dash. If someone wants m-dash, can use \-- with \-{}- as
          // a way to "escape" an n-dash followed by hyphen.
          //
          switch (ot)
          {
          case ot_plain:
            {
              r += "--";
              break;
            }
          case ot_html:
            {
              r += "&#8211;";
              break;
            }
          case ot_man:
            {
              r += "\\(en";
              break;
            }
          }

          break;
        }
      case 'n':
        {
          switch (ot)
          {
          case ot_plain:
            {
              r += '\n';
              break;
            }
          case ot_html:
            {
              if (!r.empty () && r[r.size () - 1] != '\n')
                r += '\n';

              r += "<br/>";
              break;
            }
          case ot_man:
            {
              if (!r.empty () && r[r.size () - 1] != '\n')
                r += '\n';

              // Note that if we have several consecutive breaks, they
              // will be collapsed into a single one. No, .sp doesn't
              // work (or, more exactly, will only work for two breaks).
              //
              r += ".br";
              break;
            }
          }

          // Skip following spaces.
          //
          for (; i + 1 < n && s[i + 1] == ' '; ++i) ;

          switch (ot)
          {
          case ot_plain: break;
          case ot_html:
          case ot_man:
            {
              if (i + 1 < n) // More text in this paragraph?
                r += "\n";

              break;
            }
          }

          break;
        }
      case 'c':
        {
          block b (code);
          size_t j (i + 1);

          if (j < n)
          {
            if (s[j] == 'i')
            {
              b |= itlc;
              j++;

              if (j < n && s[j] == 'b')
              {
                b |= bold;
                j++;
              }
            }
            else if (s[j] == 'b')
            {
              b |= bold;
              j++;

              if (j < n && s[j] == 'i')
              {
                b |= itlc;
                j++;
              }
            }
          }

          if (j < n && s[j] == '{')
          {
            i = j;
            blocks.push_back (b);
            new_block = true;
            break;
          }

          r += 'c';
          break;
        }
      case 'i':
        {
          block b (itlc);
          size_t j (i + 1);

          if (j < n)
          {
            if (s[j] == 'c')
            {
              b |= code;
              j++;

              if (j < n && s[j] == 'b')
              {
                b |= bold;
                j++;
              }
            }
            else if (s[j] == 'b')
            {
              b |= bold;
              j++;

              if (j < n && s[j] == 'c')
              {
                b |= code;
                j++;
              }
            }
          }

          if (j < n && s[j] == '{')
          {
            i = j;
            blocks.push_back (b);
            new_block = true;
            break;
          }

          r += 'i';
          break;
        }
      case 'b':
        {
          block b (bold);
          size_t j (i + 1);

          if (j < n)
          {
            if (s[j] == 'c')
            {
              b |= code;
              j++;

              if (j < n && s[j] == 'i')
              {
                b |= itlc;
                j++;
              }
            }
            else if (s[j] == 'i')
            {
              b |= itlc;
              j++;

              if (j < n && s[j] == 'c')
              {
                b |= code;
                j++;
              }
            }
          }

          if (j < n && s[j] == '{')
          {
            i = j;
            blocks.push_back (b);
            new_block = true;
            break;
          }

          r += 'b';
          break;
        }
      case 'l':
        {
          if (i + 1 < n && s[i + 1] == '{')
          {
            string& t (link_target);

            if (!t.empty ())
            {
              cerr << "error: nested links in documentation paragraph '"
                   << string (s, 0, n) << "'" << endl;
              throw generation_failed ();
            }

            // Extract the link target.
            //
            for (++i; i + 1 < n; ++i)
            {
              char c (s[i + 1]);

              if (c == ' ')
              {
                for (++i; i + 1 < n && s[i + 1] == ' '; ++i) ; // Skip spaces.
                break;
              }

              if (c == '}')
                break;

              t += c;
            }

            if (t.empty ())
            {
              cerr << "error: missing link target in documentation paragraph '"
                   << string (s, 0, n) << "'" << endl;
              throw generation_failed ();
            }

            // See if link_target is a man page name/section.
            //
            if (t.find (':') == string::npos) // No protocol.
            {
              size_t o (t.find ('('));

              if (o == string::npos)
              {
                cerr << "error: missing man section in '" << t << "'" << endl;
                throw generation_failed ();
              }

              link_section.assign (t, o + 1, t.size () - o - 2);

              if (t[t.size () - 1] != ')' || link_section.empty ())
              {
                cerr << "error: missing man section in '" << t << "'" << endl;
                throw generation_failed ();
              }

              string n (t, 0, o);

              if (n.empty ())
              {
                cerr << "error: missing man page in '" << t << "'" << endl;
                throw generation_failed ();
              }

              t = n;
            }
            else
              link_section.clear ();

            link_empty = i + 1 < n && s[i + 1] == '}';

            blocks.push_back (link);
            new_block = true;
            break;
          }

          r += 'l';
          break;
        }
      case '\\':
        {
          switch (ot)
          {
          case ot_man:
            {
              r += "\\e";
              break;
            }
          default:
            {
              r += '\\';
              break;
            }
          }
          break;
        }
      case '"':
        {
          r += '"';
          break;
        }
      case '\'':
        {
          r += '\'';
          break;
        }
      case '}':
        {
          r += '}';
          break;
        }
      case '|':
        {
          r += '|';
          break;
        }
      default:
        {
          cerr << "error: unknown escape sequence '\\" << c << "' in "
               << "documentation paragraph '" << string (s, 0, n) << "'"
               << endl;
          throw generation_failed ();
        }
      }

      // If we just added a new block, add corresponding output markup.
      //
      if (new_block)
      {
        block b (blocks.back ());

        block eb (0); // Effective block.
        for (vector<block>::iterator i (blocks.begin ());
             i != blocks.end ();
             ++i)
          eb |= *i & (code | itlc | bold);

        switch (ot)
        {
        case ot_plain:
          {
            if ((b & link) == 0)
            {
              if (color)
              {
                if (b & bold)
                  r += "\033[1m";

                if (b & itlc)
                  r += "\033[4m";
              }
            }

            break;
          }
        case ot_html:
          {
            if (b & link)
            {
              r += "<a href=\"";

              if (link_section.empty ())
                r += link_target;
              else
                r += link_target + options.html_suffix ();

              r += "\">";
            }
            else
            {
              if (b & code)
                r += "<code>";

              if (b & itlc)
                r += "<i>";

              if (b & bold)
                r += "<b>";
            }

            break;
          }
        case ot_man:
          {
            if ((b & link) == 0)
            {
              if ((eb & itlc) && (eb & bold))
                r += "\\f(BI";
              else if (eb & itlc)
                r += "\\fI";
              else if (eb & bold)
                r += "\\fB";
            }

            break;
          }
        }
      }

      escape = false;
    }
    else // Not escape.
    {
      switch (c)
      {
      case '\\':
        {
          escape = true;
          break;
        }
      case '.':
        {
          if (ot == ot_man)
            r += "\\.";
          else
            r += '.';
          break;
        }
      case '}':
        {
          if (!blocks.empty ())
          {
            block b (blocks.back ());
            blocks.pop_back ();

            block eb (0); // New effective block.
            for (vector<block>::iterator i (blocks.begin ());
                 i != blocks.end ();
                 ++i)
              eb |= *i & (code | itlc | bold);

            switch (ot)
            {
            case ot_plain:
              {
                if (b & link)
                {
                  if (!link_empty)
                    r += " (";

                  if (link_section.empty ())
                    r += link_target;
                  else
                  {
                    if (color)
                      r += "\033[1m";

                    r += link_target;

                    if (color)
                      r += "\033[0m";

                    r += "(" + link_section + ")";
                  }

                  if (!link_empty)
                    r += ")";
                }
                else
                {
                  if (color)
                  {
                    // While there are codes to turn off bold (22) and
                    // underline (24), it is not clear how widely they
                    // are supported.
                    //
                    r += "\033[0m"; // Clear all.

                    if (eb & bold)
                      r += "\033[1m";

                    if (eb & itlc)
                      r += "\033[4m";
                  }
                }

                break;
              }
            case ot_html:
              {
                if (b & link)
                {
                  if (link_empty)
                  {
                    if (link_section.empty ())
                      r += link_target;
                    else
                    {
                      r += "<code>";
                      r += "<b>" + link_target + "</b>";
                      r += "(" + link_section + ")";
                      r += "</code>";
                    }
                  }

                  r += "</a>";
                }
                else
                {
                  if (b & bold)
                    r += "</b>";

                  if (b & itlc)
                    r += "</i>";

                  if (b & code)
                    r += "</code>";
                }

                break;
              }
            case ot_man:
              {
                if (b & link)
                {
                  if (!link_empty)
                    r += " (";

                  if (link_section.empty ())
                    r += link_target;
                  else
                  {
                    r += "\\fB" + link_target + "\\fP";
                    r += "(" + link_section + ")";
                  }

                  if (!link_empty)
                    r += ")";
                }
                else
                {
                  // At first sight, \fP (select previous font) looks like
                  // exactly what we need here. However, it doesn't quite
                  // have the stack semantics that we need.
                  //
                  if ((eb & itlc) && (eb & bold))
                    r += "\\f(BI";
                  else if (eb & itlc)
                    r += "\\fI";
                  else if (eb & bold)
                    r += "\\fB";
                  else
                    r += "\\fR";
                }

                break;
              }
            }

            if (b & link)
              link_target.clear ();

            break;
          }

          // Fall through.
        }
      default:
        r += c;
        break;
      }
    }
  }

  if (escape)
  {
    cerr << "error: unterminated escape sequence in documentation "
         << "paragraph '" << string (s, 0, n) << "'" << endl;
    throw generation_failed ();
  }

  if (!blocks.empty ())
  {
    unsigned char b (blocks.back ());
    string bs;

    if (b & code) bs += 'c';
    if (b & itlc) bs += 'i';
    if (b & bold) bs += 'b';
    if (b & link) bs  = 'l';

    cerr << "error: unterminated formatting block '\\" << bs << "' "
         << "in documentation paragraph '" << string (s, 0, n) << "'" << endl;
    throw generation_failed ();
  }
}

struct block
{
  enum kind_type {h, ul, ol, dl, li, text, pre};

  kind_type kind;
  bool para;     // True if first text fragment should be in own paragraph.

  string header;
  string value;
  string trailer;

  block (kind_type k, bool p, const string& h = "")
      : kind (k), para (p), header (h) {}
};

static const char* block_kind_str[] = {
  "\\h", "\\ul", "\\ol", "\\dl", "\\li", "text", "preformatted text"};

inline ostream&
operator<< (ostream& os, block::kind_type k)
{
  return os << block_kind_str[k];
}

// Given the contents of a block element, convert any leading/trailing <br/>
// elements to top/bottom margins. While conceptually it would seem better to
// translate them to padding, margins actually give better practical results
// since they overlap with container's margins.
//
static string
html_margin (string& v)
{
  size_t top (0), bot (0);

  const char* b (v.c_str ());
  const char* e (v.c_str () + v.size ());

  for (; e - b >= 5 && strncmp (b, "<br/>", 5) == 0; ++top)
  {
    b += 5;

    if (b != e && *b == '\n') // Remove following newline, if any.
      ++b;
  }

  for (; e - b >= 5 && strncmp (e - 5, "<br/>", 5) == 0; ++bot)
  {
    e -= 5;

    if (e != b && *(e - 1) == '\n') // Remove preceding newline, if any.
      --e;
  }

  if (top == 0 && bot == 0)
    return "";

  string t;
  t.swap (v);
  v.assign (b, e - b);

  ostringstream os;

  if (top != 0)
    os << "margin-top:" << top << "em";

  if (bot != 0)
    os << (top != 0 ? ";" : "") << "margin-bottom:" << bot << "em";

  return os.str ();
}


string context::
format (output_type ot, string const& s, bool para)
{
  stack<block> blocks;
  blocks.push (block (block::text, para)); // Top-level.

  // Number of li in ol. Since we don't support nested lists, we don't
  // need to push it into the stack.
  //
  size_t ol_count;

  bool last (false);
  for (size_t b (0), e; !last; b = e + 1)
  {
    bool pre (s[b] == 0x02);

    const char* l;
    size_t n;

    if (pre)
    {
      ++b; // Skip 0x02.

      e = s.find (0x03, b);
      assert (e != string::npos);

      l = s.c_str () + b;
      n = e - b;

      ++e; // Skip newline that follows 0x03.
      last = (e == s.size ());
    }
    else
    {
      e = s.find ('\n', b);
      last = (e == string::npos);

      l = s.c_str () + b;
      n = (last ? s.size () : e) - b;
    }

    const char* ol (l); // Original, full line for diagnostics.
    size_t on (n);

    // First determine what kind of paragraph block this is.
    //
    block::kind_type k;
    size_t pop (0); // Pop count.

    if (pre)
    {
      k = block::pre;
    }
    else
    {
      if (n >= 3 && strncmp (l, "\\h|", 3) == 0)
      {
        k = block::h;
        l += 3;
        n -= 3;
      }
      else if (n >= 4 &&
               (strncmp (l, "\\ul|", 4) == 0 ||
                strncmp (l, "\\ol|", 4) == 0 ||
                strncmp (l, "\\dl|", 4) == 0))
      {
        switch (l[1])
        {
        case 'u': k = block::ul; break;
        case 'o': k = block::ol; break;
        case 'd': k = block::dl; break;
        }

        l += 4;
        n -= 4;
      }
      else if (n >= 4 && strncmp (l, "\\li|", 4) == 0)
      {
        k = block::li;
        l += 4;
        n -= 4;
      }
      else
        k = block::text;

      // Skip leading spaces after opening '|'.
      //
      if (k != block::text)
        while (n != 0 && (*l == 0x20 || *l == 0x0D || *l == 0x09)) {l++; n--;}

      // Next figure out how many blocks we need to pop at the end of this
      // paragraph. Things get a bit complicated since '|' could be escaped.
      //
      for (; n - pop > 0 && l[n - pop - 1] == '|'; ++pop) ;
      if (pop != 0)
      {
        // To determine whether the first '|' is part of an escape sequence
        // we have to find the first non-backslash character and then figure
        // out who escapes whom.
        //
        size_t ec (0); // Escape count.
        for (; n - pop - ec > 0 && l[n - pop - ec - 1] == '\\'; ++ec) ;

        // If we have an odd number of backslashes, then the last '|' is
        // escaped.
        //
        if (ec % 2 != 0)
          --pop;

        n -= pop; // Number of special '|' at the end.

        // Skip trailing spaces before closing '|'.
        //
        while (n != 0 && (l[n - 1] == 0x20 ||
                          l[n - 1] == 0x0D ||
                          l[n - 1] == 0x09)) n--;
      }
    }

    // Outer block kind.
    //
    block::kind_type ok (blocks.top ().kind);

    // Verify that this block type is valid in this context. Ignore
    // empty text blocks (can happen if we just have '|').
    //
    if (k != block::text || n != 0)
    {
      bool good (true);

      switch (ok)
      {
      case block::h: good = false; break;
      case block::ul:
      case block::ol:
      case block::dl: good = (k == block::li); break;
      case block::li: good = (k == block::text || k == block::pre); break;
      case block::text: good = (k != block::li); break;
      case block::pre: assert (false);
      }

      if (!good)
      {
        cerr << "error: " << k << " inside " << ok << " "
             << "in documentation string '" << s << "'" << endl;
        throw generation_failed ();
      }
    }

    // Verify the block itself.
    //
    switch (k)
    {
    case block::h:
      {
        // \h blocks are only valid if we are required to start a new
        // paragraph (first_para is true).
        //
        if (!para)
        {
          cerr << "error: paragraph '" << string (ol, 0, on) << "' "
               << "not allowed in '" << s << "'" << endl;
          throw generation_failed ();
        }

        // \h must be single-paragraph.
        //
        if (pop == 0)
        {
          cerr << "error: '|' expected at the end of paragraph '"
               << string (ol, 0, on) << "'" << endl;
          throw generation_failed ();
        }

        // \h must not be empty.
        //
        if (n == 0)
        {
          cerr << "error: empty paragraph '" << string (ol, 0, on) << "' "
               << "in documentation string '" << s << "'" << endl;
          throw generation_failed ();
        }

        break;
      }
    case block::ul:
    case block::ol:
    case block::dl:
      {
        if (pop != 0)
        {
          cerr << "error: empty list '" << string (ol, 0, on) << "' "
               << "in documentation string '" << s << "'" << endl;
          throw generation_failed ();
        }

        if (n != 0)
        {
          cerr << "error: unexpected text after " << k << "| "
               << "in paragraph '" << string (ol, 0, on) << "'" << endl;
          throw generation_failed ();
        }

        break;
      }
    case block::li:
      {
        if (ok == block::dl)
        {
          if (n == 0)
          {
            cerr << "error: term text missing in paragraph '"
                 << string (ol, 0, on) << "'" << endl;
            throw generation_failed ();
          }
        }

        break;
      }
    case block::text:
    case block::pre:
      break;
    }

    // Push the block into the stack.
    //
    switch (k)
    {
    case block::h: blocks.push (block (k, false)); break;
    case block::ul:
    case block::ol: ol_count = 0; // Fall through.
    case block::dl: blocks.push (block (k, true)); break;
    case block::li:
      {
        string h;

        switch (blocks.top ().kind)
        {
        case block::ol:
          {
            ostringstream os;
            os << ++ol_count;
            h = os.str ();
            break;
          }
        case block::dl:
          {
            format_line (ot, h, l, n);
            n = 0;
            break;
          }
        default:
          break;
        }

        blocks.push (block (k, false, h));
        break;
      }
    case block::text: break; // No push.
    case block::pre: break;  // No push.
    }

    // Output paragraph text.
    //
    if (n != 0)
    {
      block& b (blocks.top ());
      string& v (b.value);
      bool first (v.empty ());

      switch (ot)
      {
      case ot_plain:
        {
          // Separate paragraphs with a blank line.
          //
          if (!first)
            v += "\n\n";

          if (k == block::pre)
            v.append (l, n);
          else
          {
            /*
            // Strip a single "\n" at the beginning of the paragraph since we
            // already wrote one newline as part of starting the paragraph. An
            // example where this might be useful:
            //
            // li|
            //
            //   \
            //   ...
            //   \
            //
            //   \n|
            //
            if (n >= 2 && l[0] == '\\' && l[1] == 'n')
            {
              l += 2;
              n -= 2;
            }

            if (n != 0)
            */

            format_line (ot, v, l, n);
          }

          break;
        }
      case ot_html:
        {
          // Separate paragraphs with a blank line.
          //
          if (!first)
            v += "\n\n";

          if (k == block::pre)
          {
            v += "<pre>";
            v.append (l, n);
            v += "</pre>";
          }
          else
          {
            if (!first || b.para)
            {
              string t;
              format_line (ot, t, l, n);
              string s (html_margin (t));

              v += (s.empty () ? "<p>" : "<p style=\"" + s + "\">");
              v += t;
              v += "</p>";
            }
            else
              format_line (ot, v, l, n);
          }

          break;
        }
      case ot_man:
        {
          if (b.para)
          {
            if (!first)
              v += "\n";

            v += ".PP\n";
          }
          else
          {
            if (!first)
              v += "\n\n";
          }

          if (k == block::pre)
          {
            v += ".nf\n";

            // Note that if we have several consecutive blank lines, they
            // will be collapsed into a single one. No, .sp doesn't work.
            //
            char c, p ('\n'); // Current and previous characters.
            for (size_t i (0); i != n; p = c, ++i)
            {
              switch (c = l[i])
              {
              case '\\': v += '\\'; break;
              case '.':  v += p != '\n' ? "\\" : "\\&\\"; break;
              }

              v += c;
            }

            v += "\n.fi";
          }
          else
          {
            format_line (ot, v, l, n);
          }

          break;
        }
      }
    }

    // Pop paragraph blocks.
    //
    if (pop >= blocks.size ()) // >= to account for top-level.
    {
      cerr << "error: extraneous '|' at the end of paragraph '"
           << string (ol, 0, on) << "'" << endl;
      throw generation_failed ();
    }

    for (; pop != 0; --pop)
    {
      block pb (blocks.top ()); // move
      string& pv (pb.value);
      string& ph (pb.header);

      blocks.pop ();

      block& b (blocks.top ());
      string& v (b.value);

      switch (ot)
      {
      case ot_plain:
        {
          const char* sn (v.empty () ? "" : "\n");
          const char* dn (v.empty () ? "" : "\n\n");

          switch (pb.kind)
          {
          case block::h:
            {
              v += dn;

              if (options.ansi_color ())
                v += "\033[1m"; // Bold.

              v += pv;

              if (options.ansi_color ())
                v += "\033[0m";

              break;
            }
          case block::ul:
          case block::ol:
          case block::dl: v += dn + pv; break;
          case block::li:
            {
              v += sn;
              size_t ind (0), vn (pv.size ());

              switch (b.kind)
              {
              case block::ul:
                v += "* ";
                ind = 2;
                break;
              case block::ol:
                v += ph + ". ";
                ind = ph.size () + 2;
                break;
              case block::dl:
                v += ph + "\n" + (vn != 0 && pv[0] != '\n' ? "    " : "");
                ind = 4;
                break;
              default:
                break;
              }

              // Add value with indentation for subsequent paragraphs.
              //
              char c, p ('\0'); // Current and previous characters.
              for (size_t i (0); i != vn; p = c, ++i)
              {
                c = pv[i];

                if (p == '\n' && c != '\n') // Don't indent blanks.
                  v += string (ind, ' ');

                v += c;
              }

              break;
            }
          case block::text:
          case block::pre: assert (false);
          }

          break;
        }
      case ot_html:
        {
          if (!v.empty ())
            v += "\n\n";

          switch (pb.kind)
          {
          case block::h:  v += "<h1>" + pv + "</h1>"; break;
          case block::ul: v += "<ul>\n" + pv + "\n</ul>"; break;
          case block::ol: v += "<ol>\n" + pv + "\n</ol>"; break;
          case block::dl: v += "<dl>\n" + pv + "\n</dl>"; break;
          case block::li:
            {
              string pvs (html_margin (pv));

              if (b.kind == block::dl)
              {
                string phs (html_margin (ph));

                v += (phs.empty () ? "<dt>" : "<dt style=\"" + phs + "\">")
                  + ph + "</dt>\n";

                v += (pvs.empty () ? "<dd>" : "<dd style=\"" + pvs + "\">")
                  + pv + "</dd>";
              }
              else
                v += (pvs.empty () ? "<li>" : "<li style=\"" + pvs + "\">")
                  + pv + "</li>";

              break;
            }
          case block::text:
          case block::pre: assert (false);
          }

          break;
        }
      case ot_man:
        {
          // Seeing that we always write a macro, one newline is enough.
          //
          if (!v.empty ())
            v += "\n";

          switch (pb.kind)
          {
          case block::h:  v += ".SH \"" + pv + "\""; break;
          case block::ul:
          case block::ol:
          case block::dl:
            {
              if (!b.para) // First list inside .IP.
              {
                // .IP within .IP? Just shoot me in the head already! We
                // have to manually indent it with .RS/.RE *and* everything
                // that comes after it (since .PP resets the indent). Why
                // not just indent the whole list content? Because then the
                // first line will never start on the same line as the term.
                //
                v += ".RS\n";
                b.trailer = "\n.RE";
                b.para = true; // Start emitting .PP from now on.
              }

              v += pv;
              break;
            }
          case block::li:
            {
              switch (b.kind)
              {
              case block::ul: v += ".IP \\(bu 2em\n" + pv; break;
              case block::ol: v += ".IP " + ph + ". 4em\n" + pv; break;
              case block::dl: v += ".IP \"" + ph + "\"\n" + pv; break;
              default: break;
              }

              break;
            }
          case block::text:
          case block::pre: assert (false);
          }

          break;
        }
      }
    }
  }

  assert (!blocks.empty ()); // Should have top-level.

  if (blocks.size () > 1)
  {
    cerr << "error: unterminated paragraph " << blocks.top ().kind << " "
         << "in documentation string '" << s << "'" << endl;
    throw generation_failed ();
  }

  block& b (blocks.top ());

  switch (ot)
  {
  case ot_man: return b.value + b.trailer;
  default: return b.value;
  }
}

string context::
substitute (const string& s, semantics::cli_unit& u, const path* d)
{
  string r;

  // Scan the string looking for variables ($var$) or paths.
  //
  size_t b (0), e (b);
  for (size_t n (s.size ()); e != n; ++e)
  {
    if (s[e] == '$' && e + 1 != n)
    {
      if (s[e + 1] == '$') // Escape.
      {
        r.append (s, b, ++e - b); // Keep one, skip the other.
        b = e + 1;
        continue;
      }

      bool file (false);

      // Scan for as long as it is a C identifier or a file.
      //
      size_t p (e + 1); // Position of the second '$'.
      for (; p != n; ++p)
      {
        char c (s[p]);
        bool f (p == e + 1); // First.

        if (d != 0 && f && c == '.' && p + 1 != n &&
            (s[p + 1] == '/' ||                                   // "./"
             (s[p + 1] == '.' && p + 2 != n && s[p + 2] == '/'))) // "../"
          file = true;

        if (file)
        {
          if (c == '$')
            break;
        }
        else
        {
          if (!(c == '_' ||
                ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z') ||
                (!f && '0' <= c && c <= '9')))
            break;
        }
      }

      // Note: check that the second '$' is not escaped.
      //
      if (p != n && s[p] == '$' && (p + 1 == n || s[p + 1] != '$'))
      {
        r.append (s, b, e - b); // Save what came before the expansion.

        // Var/file name.
        //
        ++e;
        string v (s, e, p - e);

        if (file)
        {
          path p (*d / path (v));
          p.normalize (); // Apply '.' and '..'.

          ifstream ifs (p.string ().c_str (), ifstream::in | ifstream::binary);
          if (!ifs.is_open ())
          {
            cerr << p << ": error: unable to open in read mode" << endl;
            throw generation_failed ();
          }

          // We don't expect our file to contain '\0' so use that as the
          // delimiter to read the entire file with getline().
          //
          string s;
          getline (ifs, s, '\0');
          r += s;
        }
        else
        {
          // Lookup and substiute the variable.
          //
          if (semantics::doc* d = u.lookup<semantics::doc> ("", "var: " + v))
            r += d->front ();
          else
          {
            cerr << "error: undefined variable '" << v << "' in '" << s << "'"
                 << endl;
            throw generation_failed ();
          }
        }

        e = p;
        b = e + 1;
      }
    }
  }

  r.append (s, b, e - b); // Last chunk.
  return r;
}

string context::
fq_name (semantics::nameable& n, bool cxx_name)
{
  using namespace semantics;

  string r;

  if (dynamic_cast<cli_unit*> (&n))
  {
    return ""; // Map to global namespace.
  }
  else
  {
    r = fq_name (n.scope ());
    r += "::";
    r += cxx_name ? escape (n.name ()) : n.name ();
  }

  return r;
}

string context::
ns_open (const string& qn, bool last)
{
  string::size_type b (0), e;

  do
  {
    e = qn.find ("::", b);
    string n (qn, b, e == string::npos ? e : e - b);

    if (!n.empty () && (last || e != string::npos))
      os << "namespace " << n << "{";

    b = e;

    if (b == string::npos)
      return n;

    b += 2;

  } while (true);
}

void context::
ns_close (const string& qn, bool last)
{
  string::size_type b (0), e;

  do
  {
    e = qn.find ("::", b);
    string n (qn, b, e == string::npos ? e : e - b);

    if (!n.empty () && (last || e != string::npos))
      os << "}";

    b = e;

    if (b == string::npos)
      return;

    b += 2;

  } while (true);
}

class_doc context::
class_doc (semantics::class_& c)
{
  typedef map<string, string> map;
  const map& m (options.class_doc ());

  string n (c.fq_name ());

  map::const_iterator i (m.find (n));

  if (i == m.end ())
  {
    n = string (n, 2, string::npos); // Try without leading '::'.
    i = m.find (n);
  }

  if (i == m.end ())
    return cd_default;

  const string& k (i->second);

  if (k == "exclude")
    return cd_exclude;
  else if (k == "short")
    return cd_short;
  else if (k == "long")
    return cd_long;
  else
  {
    cerr << "error: unknown --class-doc kind value '" << k << "'" << endl;
    throw generation_failed ();
  }
}

string context::
first_sentence (string const& s)
{
  size_t p (s.find ('.'));

  // Add some heuristics here: check that there is a space
  // (or end of text) after the period.
  //
  while (p != string::npos &&
         p + 1 <= s.size () &&
         s[p + 1] != ' ' &&
         s[p + 1] != '\n')
    p = s.find ('.', p + 1);

  return p == string::npos ? s : string (s, 0, p + 1);
}

// namespace
//

void namespace_::
pre (type& ns)
{
  string name (ns.name ());

  if (!name.empty ())
    os << "namespace " << escape (name)
       << "{";
}

void namespace_::
post (type& ns)
{
  if (!ns.name ().empty ())
    os << "}";
}
