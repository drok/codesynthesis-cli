// file      : cli/context.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <stack>

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

string context::
format (output_type ot, string const& s, bool para)
{
  string r;

  if (para && ot == ot_html)
    r += "<p>";

  bool escape (false);
  std::stack<unsigned char> blocks; // Bit 0: code; 1: italic; 2: bold.

  for (size_t i (0), n (s.size ()); i < n; ++i)
  {
    if (escape)
    {
      bool block (false);

      switch (s[i])
      {
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
      case 'c':
        {
          unsigned char b (1);
          size_t j (i + 1);

          if (j < n)
          {
            if (s[j] == 'i')
            {
              b |= 2;
              j++;

              if (j < n && s[j] == 'b')
              {
                b |= 4;
                j++;
              }
            }
            else if (s[j] == 'b')
            {
              b |= 4;
              j++;

              if (j < n && s[j] == 'i')
              {
                b |= 2;
                j++;
              }
            }
          }

          if (j < n && s[j] == '{')
          {
            i = j;
            blocks.push (b);
            block = true;
            break;
          }

          r += 'c';
          break;
        }
      case 'i':
        {
          unsigned char b (2);
          size_t j (i + 1);

          if (j < n)
          {
            if (s[j] == 'c')
            {
              b |= 1;
              j++;

              if (j < n && s[j] == 'b')
              {
                b |= 4;
                j++;
              }
            }
            else if (s[j] == 'b')
            {
              b |= 4;
              j++;

              if (j < n && s[j] == 'c')
              {
                b |= 1;
                j++;
              }
            }
          }

          if (j < n && s[j] == '{')
          {
            i = j;
            blocks.push (b);
            block = true;
            break;
          }

          r += 'i';
          break;
        }
      case 'b':
        {
          unsigned char b (4);
          size_t j (i + 1);

          if (j < n)
          {
            if (s[j] == 'c')
            {
              b |= 1;
              j++;

              if (j < n && s[j] == 'i')
              {
                b |= 2;
                j++;
              }
            }
            else if (s[j] == 'i')
            {
              b |= 2;
              j++;

              if (j < n && s[j] == 'c')
              {
                b |= 1;
                j++;
              }
            }
          }

          if (j < n && s[j] == '{')
          {
            i = j;
            blocks.push (b);
            block = true;
            break;
          }

          r += 'b';
          break;
        }
      case '}':
        {
          r += '}';
          break;
        }
      }

      if (block)
      {
        unsigned char b (blocks.top ());

        switch (ot)
        {
        case ot_plain:
          {
            if (b & 1)
              r += "'";
            break;
          }
        case ot_html:
          {
            if (b & 1)
              r += "<code>";

            if (b & 2)
              r += "<i>";

            if (b & 4)
              r += "<b>";

            break;
          }
        case ot_man:
          {
            if ((b & 6) == 6)
              r += "\\f(BI";
            else if (b & 2)
              r += "\\fI";
            else if (b & 4)
              r += "\\fB";

            break;
          }
        }
      }

      escape = false;
    }
    else if (s[i] == '\\')
    {
      escape = true;
    }
    else if (s[i] == '\n')
    {
      switch (ot)
      {
      case ot_plain:
      case ot_man:
        {
          r += '\n';
          break;
        }
      case ot_html:
        {
          if (para)
            r += "</p>";
          else
            para = true;

          r += "\n<p>";
          break;
        }
      }
    }
    else if (s[i] == '.')
    {
      if (ot == ot_man)
        r += "\\.";
      else
        r += '.';
    }
    else if (!blocks.empty () && s[i] == '}')
    {
      unsigned char b (blocks.top ());

      switch (ot)
      {
      case ot_plain:
        {
          if (b & 1)
            r += "'";
          break;
        }
      case ot_html:
        {
          if (b & 4)
            r += "</b>";

          if (b & 2)
            r += "</i>";

          if (b & 1)
            r += "</code>";

          break;
        }
      case ot_man:
        {
          if (b & 6)
            r += "\\fP";
        }
      default:
        break;
      }

      blocks.pop ();
    }
    else
      r += s[i];
  }

  if (para)
    r += "</p>";

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

void context::
cli_open ()
{
  string::size_type b (0), e;

  do
  {
    e = cli.find ("::", b);
    string n (cli, b, e == string::npos ? e : e - b);

    if (!n.empty ())
      os << "namespace " << n << "{";

    b = e;

    if (b == string::npos)
      break;

    b += 2;

  } while (true);
}

void context::
cli_close ()
{
  string::size_type b (0), e;

  do
  {
    e = cli.find ("::", b);
    string n (cli, b, e == string::npos ? e : e - b);

    if (!n.empty ())
      os << "}";

    b = e;

    if (b == string::npos)
      break;

    b += 2;

  } while (true);
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
