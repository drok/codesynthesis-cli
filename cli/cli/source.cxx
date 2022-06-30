// file      : cli/source.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// license   : MIT; see accompanying LICENSE file

#include <iostream>

#include <cli/source.hxx>
#include <cli/txt.hxx>    // txt_size(), txt_wrap_lines()

using namespace std;

namespace
{
  struct option_init: traversal::option, context
  {
    option_init (context& c) : context (c), comma_ (false) {}

    virtual void
    traverse (type& o)
    {
      if (comma_)
        os << "," << endl
           << "  ";
      else
      {
        os << endl
           << ": ";
        comma_ = true;
      }

      os << emember (o);

      if (o.initialized_p ())
      {
        using semantics::expression;
        expression const& i (o.initializer ());

        switch (i.type ())
        {
        case expression::string_lit:
        case expression::char_lit:
        case expression::bool_lit:
        case expression::int_lit:
        case expression::float_lit:
        case expression::identifier:
            {
            os << " (" << i.value () << ")";
            break;
          }
        case expression::call_expr:
          {
            os << " " << i.value ();
            break;
          }
        }
      }
      else
        os << " ()";

      if (gen_specifier && o.type ().name () != "bool")
        os << "," << endl
           << "  " << especifier_member (o) << " (false)";
    }

  private:
    bool comma_;
  };

  struct option_merge: traversal::option, context
  {
    option_merge (context& c) : context (c) {}

    virtual void
    traverse (type& o)
    {
      string type (o.type ().name ());
      bool b (type == "bool");

      string member (emember (o));
      string spec_member (b ? member : especifier_member (o));

      os << "if (a." << spec_member << ")"
         << "{"
         <<   cli << "::parser< " << type << ">::merge (" << endl
         <<     "this->" << member << ", a." << member << ");";
      if (!b)
        os << "this->" << spec_member << " = true;";
      os << "}";
    }
  };

  //
  //
  struct option_map: traversal::option, context
  {
    option_map (context& c) : context (c) {}

    virtual void
    traverse (type& o)
    {
      using semantics::names;

      string member (emember (o));
      string type (o.type ().name ());
      string scope (escape (o.scope ().name ()));
      string map ("_cli_" + scope + "_map_");

      names& n (o.named ());

      for (names::name_iterator i (n.name_begin ()); i != n.name_end (); ++i)
      {
        os << "_cli_" << scope << "_map_[\"" << *i << "\"] =" << endl
           << "&" << cli << "::thunk< " << scope;

        if (type != "bool")
          os << ", " << type;

        os << ", " << "&" << scope << "::" << member;

        if (gen_specifier && type != "bool")
          os << "," << endl
             << "  &" << scope << "::" << especifier_member (o);

        os << " >;";
      }
    }
  };

  //
  //
  struct option_desc: traversal::option, context
  {
    option_desc (context& c) : context (c) {}

    virtual void
    traverse (type& o)
    {
      using semantics::names;

      names& n (o.named ());

      os << "// " << o.name () << endl
         << "//" << endl
         << "{"
         << cli << "::option_names a;";

      for (names::name_iterator b (n.name_begin ()), i (b);
           i != n.name_end (); ++i)
      {
        if (i == b) // Skip the primary name.
          continue;

        os << "a.push_back (\"" << *i << "\");";
      }

      if (o.initialized_p ())
      {
        using semantics::expression;
        expression const& i (o.initializer ());

        switch (i.type ())
        {
        case expression::string_lit:
          {
            os << "std::string dv (" << i.value () << ");";
            break;
          }
        case expression::char_lit:
          {
            os << "std::string dv (1, " << i.value () << ");";
            break;
          }
        case expression::bool_lit:
        case expression::int_lit:
        case expression::float_lit:
          {
            os << "std::string dv (\"" << i.value () << "\");";
            break;
          }
        case expression::identifier:
        case expression::call_expr:
          {
            os << "std::string dv;";
            break;
          }
        }
      }
      else
        os << "std::string dv;";


      os << cli << "::option o (\"" << o.name () << "\", a, " <<
        (o.type ().name () == "bool" ? "true" : "false") << ", dv);"
         << "os.push_back (o);"
         << "}";
    }
  };

  static string
  escape_str (string const& s)
  {
    string r;

    for (size_t i (0), n (s.size ()); i < n; ++i)
    {
      switch (s[i])
      {
      case '\\':
        {
          r += "\\\\";
          break;
        }
      case '"':
        {
          r += "\\\"";
          break;
        }
      case '\033':
        {
          r += "\\033";
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

  inline void
  wrap_lines (ostream& os,
              string& d,
              size_t indent = 0,
              size_t first = 0)
  {
    txt_wrap_lines (os,
                    d,
                    indent,
                    first,
                    "   << \"",          // line_start
                    "\" << ::std::endl", // line_end
                    "   << ::std::endl", // line_blank
                    &escape_str);
  }

  enum paragraph {para_unknown, para_text, para_option};

  struct doc: traversal::doc, context
  {
    doc (context& c, usage_type u, paragraph& p)
        : context (c), usage_ (u), para_ (p) {}

    virtual void
    traverse (type& ds)
    {
      if (ds.name ().compare (0, 3, "doc") != 0) // Ignore doc variables.
        return;

      // Figure out which documentation string we should use.
      //
      // n = 1 - common doc string
      // n = 2 - arg string, common doc string
      // n > 2 - arg string, short string, long string
      //
      size_t n (ds.size ());
      string d;

      if (gen_usage == ut_both && usage_ == ut_long)
      {
        d = n > 2                     // Have both short and long?
          ? ds[2]                     // Then use long.
          : (n == 1 ? ds[0] : ds[1]); // Else, use common.
      }
      else // Short or long.
      {
        d = n > 2                     // Have both short and long?
          ? ds[1]                     // Then use short,
          : (n == 1 ? ds[0] : ds[1]); // Else, use common (no first sentence).
      }

      if (options.ascii_tree ())
        preprocess_ascii_tree (d);

      std::set<string> arg_set;
      if (n > 1 && options.ansi_color ())
        translate_arg (ds[0], arg_set);

      d = format (ds.scope (), translate (d, arg_set), true);

      if (d.empty ())
        return;

      string up (cli + "::usage_para");

      if (para_ == para_unknown)
        os << "if (p != " << up << "::none)" << endl
           << "os << ::std::endl;"
           << endl
           << "os << \"";
      else
        os << "os << std::endl" << endl
           << "   << \"";

      wrap_lines (os, d);
      os << ";"
         << endl;

      para_ = para_text;
    }

  private:
    usage_type usage_;
    paragraph& para_;
  };

  struct option_length: traversal::option, context
  {
    option_length (context& c, size_t& l)
        : context (c), length_ (l), option_ (0) {}

    option_length (context& c, size_t& l, type*& o)
        : context (c), length_ (l), option_ (&o) {}

    virtual void
    traverse (type& o)
    {
      using semantics::names;

      semantics::doc_strings const& doc (o.doc ());

      if (options.suppress_undocumented () && doc.empty ())
        return;

      size_t l (0);
      names& n (o.named ());

      for (names::name_iterator i (n.name_begin ()); i != n.name_end (); ++i)
      {
        if (l != 0)
          l++; // '|' seperator.

        l += i->size ();
      }

      string type (o.type ().name ());

      if (type != "bool" || doc.size () >= 3)
      {
        l++; // ' ' seperator

        // Note: we naturally assume this doesn't need --ascii-tree treatment.
        //
        string s (doc.size () > 0 ? doc[0] : string ("<arg>"));

        if (options.ansi_color ())
        {
          std::set<string> arg_set;
          s = translate_arg (s, arg_set);
        }

        l += txt_size (format (o.scope (), s, false));
      }

      if (l > length_)
      {
        length_ = l;
        if (option_ != 0)
          *option_ = &o;
      }
    }

  private:
    size_t& length_;
    type** option_;
  };

  //
  //
  struct option_usage: traversal::option, context
  {
    option_usage (context& c, size_t l, usage_type u, paragraph& p)
        : context (c), length_ (l), usage_ (u), para_ (p) {}

    virtual void
    traverse (type& o)
    {
      using semantics::names;

      semantics::doc_strings const& doc (o.doc ());

      if (options.suppress_undocumented () && doc.empty ())
        return;

      bool color (options.ansi_color ());

      size_t l (0);
      names& n (o.named ());

      string up (cli + "::usage_para");

      // For long usage we want options separated by blank lines.
      //
      if (para_ == para_unknown)
      {
        if (usage_ == ut_long)
          os << "if (p != " << up << "::none)" << endl;
        else
          os << "if (p == " << up << "::text)" << endl;

        os << "os << ::std::endl;"
           << endl
           << "os << \"";
      }
      else if (para_ == para_text || usage_ == ut_long)
        os << "os << std::endl" << endl
           << "   << \"";
      else
        os << "os << \"";

      for (names::name_iterator i (n.name_begin ()); i != n.name_end (); ++i)
      {
        if (l != 0)
        {
          os << '|';
          l++;
        }

        if (color)
          os << "\\033[1m"; // Bold.

        os << escape_str (*i);

        if (color)
          os << "\\033[0m";

        l += i->size ();
      }

      string type (o.type ().name ());

      std::set<string> arg_set;
      if (type != "bool" || doc.size () >= 3)
      {
        os << ' ';
        l++;

        // Note: we naturally assume this doesn't need --ascii-tree treatment.
        //
        string s (doc.size () > 0 ? doc[0] : string ("<arg>"));

        if (color)
          s = translate_arg (s, arg_set);

        s = format (o.scope (), s, false);

        os << escape_str (s);
        l += txt_size (s);
      }

      // Figure out which documentation string we should use.
      //
      string d;
      {
        size_t i (type == "bool" && doc.size () < 3 ? 0 : 1);

        if (doc.size () > i) // Have at least one.
        {
          if (gen_usage == ut_both && usage_ == ut_long)
          {
            d = doc.size () > i + 1 // Have both short and long?
              ? doc[i + 1]          // Then use long.
              : doc[i];
          }
          else // Short or long.
          {
            d = doc.size () > i + 1     // Have both short and long?
              ? doc[i]                  // Then use short,
              : (gen_usage == ut_long   // Otherwise, if asked for long,
                 ? doc[i]               // Then use long,
                 : first_sentence (doc[i])); // Else first sentence of long.
          }
        }
      }

      if (options.ascii_tree ())
        preprocess_ascii_tree (d);

      // Format the documentation string.
      //
      if (color)
        d = translate (d, arg_set);

      d = format (o.scope (), d, false);

      if (!d.empty ())
        wrap_lines (os, d, length_ + 1, l); // +1 for extra space after arg.
      else
        os << "\" << std::endl";

      os << ";"
         << endl;

      para_ = para_option;
    }

  private:
    size_t length_;
    usage_type usage_;
    paragraph& para_;
  };

  //
  //
  struct base_parse: traversal::class_, context
  {
    base_parse (context& c): context (c) {}

    virtual void
    traverse (type& c)
    {
      os << "// " << escape (c.name ()) << " base" << endl
         << "//" << endl
         << "if (" << fq_name (c) << "::_parse (o, s))" << endl
         << "return true;"
         << endl;
    }
  };

  //
  //
  struct base_merge: traversal::class_, context
  {
    base_merge (context& c): context (c) {}

    virtual void
    traverse (type& c)
    {
      os << "// " << escape (c.name ()) << " base" << endl
         << "//" << endl
         << fq_name (c) << "::merge (a);"
         << endl;
    }
  };

  //
  //
  struct base_desc: traversal::class_, context
  {
    base_desc (context& c): context (c) {}

    virtual void
    traverse (type& c)
    {
      os << "// " << escape (c.name ()) << " base" << endl
         << "//" << endl
         << fq_name (c) << "::fill (os);"
         << endl;
    }
  };

  struct base_usage: traversal::class_, context
  {
    base_usage (context& c, usage_type u): context (c), usage_ (u) {}

    virtual void
    traverse (type& c)
    {
      class_doc_type cd (class_doc (c));

      if (cd == cd_exclude || cd == cd_exclude_base)
        return;

      const char* t (
        (cd == cd_default
         ? gen_usage != ut_both || usage_ == ut_short
         : cd == cd_short) ? "" : "long_");

      os << "// " << escape (c.name ()) << " base" << endl
         << "//" << endl
         << "p = " << fq_name (c) << "::print_" << t << "usage (os, p);"
         << endl;
    }

  private:
    usage_type usage_;
  };

  //
  //
  struct class_: traversal::class_, context
  {
    class_ (context& c)
        : context (c),
          base_parse_ (c),
          base_merge_ (c),
          base_desc_ (c),
          option_merge_ (c),
          option_map_ (c),
          option_desc_ (c)
    {
      inherits_base_parse_ >> base_parse_;
      inherits_base_merge_ >> base_merge_;
      inherits_base_desc_ >> base_desc_;
      names_option_merge_ >> option_merge_;
      names_option_map_ >> option_map_;
      names_option_desc_ >> option_desc_;
    }

    virtual void
    traverse (type& c)
    {
      string name (escape (c.name ()));

      bool abst (c.abstract ());
      bool ho (has<semantics::option> (c));
      bool hb (c.inherits_begin () != c.inherits_end ());

      os << "// " << name << endl
         << "//" << endl
         << endl;

      // c-tors
      //
      string um (cli + "::unknown_mode");

      os << name << "::" << endl
         << name << " ()";
      {
        option_init init (*this);
        traversal::names names_init (init);
        names (c, names_init);
      }
      os << "{"
         << "}";

      if (!abst)
      {
        bool p (gen_parse);

        string n, res, ret;
        if (p)
        {
          n = "bool " + name + "::\n" + (name != "parse" ? "parse" : "parse_");
          res = "bool r = ";
          ret = "return r;";
        }
        else
          n = name + "::\n" + name;

        os << n << " (int& argc," << endl
           << "char** argv," << endl
           << "bool erase," << endl
           << um << " opt," << endl
           << um << " arg)";
        if (!p)
        {
          option_init init (*this);
          traversal::names names_init (init);
          names (c, names_init);
        }
        os << "{"
           << cli << "::argv_scanner s (argc, argv, erase);"
           << res << "_parse (s, opt, arg);"
           << ret
           << "}";

        os << n << " (int start," << endl
           << "int& argc," << endl
           << "char** argv," << endl
           << "bool erase," << endl
           << um << " opt," << endl
           << um << " arg)";
        if (!p)
        {
          option_init init (*this);
          traversal::names names_init (init);
          names (c, names_init);
        }
        os << "{"
           << cli << "::argv_scanner s (start, argc, argv, erase);"
           << res << "_parse (s, opt, arg);"
           << ret
           << "}";

        os << n << " (int& argc," << endl
           << "char** argv," << endl
           << "int& end," << endl
           << "bool erase," << endl
           << um << " opt," << endl
           << um << " arg)";
        if (!p)
        {
          option_init init (*this);
          traversal::names names_init (init);
          names (c, names_init);
        }
        os << "{"
           << cli << "::argv_scanner s (argc, argv, erase);"
           << res << "_parse (s, opt, arg);"
           << "end = s.end ();"
           << ret
           << "}";

        os << n << " (int start," << endl
           << "int& argc," << endl
           << "char** argv," << endl
           << "int& end," << endl
           << "bool erase," << endl
           << um << " opt," << endl
           << um << " arg)";
        if (!p)
        {
          option_init init (*this);
          traversal::names names_init (init);
          names (c, names_init);
        }
        os << "{"
           << cli << "::argv_scanner s (start, argc, argv, erase);"
           << res << "_parse (s, opt, arg);"
           << "end = s.end ();"
           << ret
           << "}";

        os << n << " (" << cli << "::scanner& s," << endl
           << um << " opt," << endl
           << um << " arg)";
        if (!p)
        {
          option_init init (*this);
          traversal::names names_init (init);
          names (c, names_init);
        }
        os << "{"
           << res << "_parse (s, opt, arg);"
           << ret
           << "}";
      }

      // merge()
      //
      if (gen_merge)
      {
        os << "void " << name << "::" << endl
           << "merge (const " << name << "& a)"
           << "{"
           <<   "CLI_POTENTIALLY_UNUSED (a);"
           << endl;

        // First merge all our bases.
        //
        inherits (c, inherits_base_merge_);

        // Then our options.
        //
        names (c, names_option_merge_);

        os << "}";
      }

      // Usage.
      //
      if (gen_usage != ut_none)
      {
        bool b (hb && !options.exclude_base ());

        // Calculate option length.
        //
        size_t len (0);
        {
          semantics::option* o (0);
          size_t max (options.option_length ());

          // We need to go into our bases unless --exclude-base was
          // specified.
          //
          {
            traversal::class_ ct;
            option_length olt (*this, len, o);
            traversal::inherits i;
            traversal::names n;

            if (b)
              ct >> i >> ct;

            ct >> n >> olt;
            ct.traverse (c);

            // Now do the same for each base and issue a warning if any
            // base has shorter option length than derived.
            //
            if (b && max == 0)
            {
              size_t d_len (len);
              semantics::option* d_o (o);

              for (type::inherits_iterator i (c.inherits_begin ());
                   i != c.inherits_end (); ++i)
              {
                type& b (i->base ());

                len = 0;
                ct.traverse (b);

                if (len == d_len)
                  continue;

                cerr << c.file () << ":" << c.line () << ":" << c.column ()
                     << " warning: derived class option length is greater "
                     << "than that of a base class '" << b.name () << "'"
                     << endl;

                cerr << b.file () << ":" << b.line () << ":" << b.column ()
                     << " note: class '" << b.name () << "' is defined here"
                     << endl;

                cerr << c.file () << ":" << c.line () << ":" << c.column ()
                     << " note: use --option-length to specify uniform length"
                     << endl;
              }

              len = d_len;
              o = d_o;
            }
          }

          if (len != 0 && max != 0)
          {
            if (len > max)
            {
              cerr << o->file () << ":" << o->line () << ":" << o->column ()
                   << " error: option length " << len << " is greater than "
                   << max << " specified with --option-length" << endl;
              throw generation_failed ();
            }

            len = max;
          }

          verify_id_ref ();
        }

        string up (cli + "::usage_para");
        string const& ost (options.ostream_type ());

        os << up << " " << name << "::" << endl
           << "print_usage (" << ost << "& os, " << up << " p)"
           << "{"
           << "CLI_POTENTIALLY_UNUSED (os);"
           << endl;
        {
          usage_type u (gen_usage == ut_both ? ut_short : gen_usage);

          base_usage bu (*this, u);
          traversal::inherits i (bu);

          if (b && !options.include_base_last ())
            inherits (c, i);

          paragraph p (para_unknown);

          doc dc (*this, u, p);
          option_usage ou (*this, len, u, p);
          traversal::names n;
          n >> dc;
          n >> ou;

          names (c, n);

          if (p != para_unknown)
            os << "p = " << up << (p == para_text ? "::text;" : "::option;")
               << endl;

          if (b && options.include_base_last ())
            inherits (c, i);
        }

        os << "return p;"
           << "}";

        verify_id_ref ();

        // Long version.
        //
        if (gen_usage == ut_both)
        {
          os << up << " " << name << "::" << endl
             << "print_long_usage (" << ost << "& os, " << up << " p)"
             << "{"
             << "CLI_POTENTIALLY_UNUSED (os);"
             << endl;

          base_usage bu (*this, ut_long);
          traversal::inherits i (bu);

          if (b && !options.include_base_last ())
            inherits (c, i);

          paragraph p (para_unknown);

          doc dc (*this, ut_long, p);
          option_usage ou (*this, len, ut_long, p);
          traversal::names n;
          n >> dc;
          n >> ou;

          names (c, n);

          if (p != para_unknown)
            os << "p = " << up << (p == para_text ? "::text;" : "::option;")
               << endl;

          if (b && options.include_base_last ())
            inherits (c, i);

          os << "return p;"
             << "}";

          verify_id_ref ();
        }
      }

      // Description.
      //
      if (options.generate_description ())
      {
        string desc ("_cli_" + name + "_desc");

        os << "struct " << desc << "_type: " << cli << "::options"
           << "{"
           << desc << "_type ()"
           << "{"
           << fq_name (c) << "::fill (*this);"
           << "}"
           << "};";

        if (options.std () < cxx_version::cxx11)
          os << "static " << desc << "_type " << desc << "_;"
             << endl;

        os << "void " << name << "::" << endl
           << "fill (" << cli << "::options&" << (ho || hb ? " os)" : ")")
           << "{";

        // Add the entries from our bases first so that our entires
        // override any conflicts.
        //
        inherits (c, inherits_base_desc_);
        names (c, names_option_desc_);

        os << "}";

        os << "const " << cli << "::options& " << name << "::" << endl
           << "description ()"
           << "{";

        if (options.std () >= cxx_version::cxx11)
          os << "static " << desc << "_type " << desc << "_;";

        os << "return " << desc << "_;"
           << "}";
      }

      // _parse ()
      //
      string map ("_cli_" + name + "_map");

      os << "typedef" << endl
         << "std::map<std::string, void (*) (" <<
        name << "&, " << cli << "::scanner&)>" << endl
         << map << ";"
         << endl
         << "static " << map << " " << map << "_;"
         << endl;

      os << "struct " << map << "_init"
         << "{"
         << map << "_init ()"
         << "{";

      names (c, names_option_map_);

      os << "}"
         << "};"
         << "static " << map << "_init " << map << "_init_;"
         << endl;

      os << "bool " << name << "::" << endl
         << "_parse (const char* o, " << cli << "::scanner& s)"
         << "{"
         << map << "::const_iterator i (" << map << "_.find (o));"
         << endl
         << "if (i != " << map << "_.end ())"
         << "{"
         << "(*(i->second)) (*this, s);"
         << "return true;"
         << "}";

      // Try our bases, from left-to-right.
      //
      inherits (c, inherits_base_parse_);

      os << "return false;"
         << "}";

      if (!abst)
      {
        bool pfx (!opt_prefix.empty ());
        bool sep (!opt_sep.empty ());

        bool comb_flags (pfx && !options.no_combined_flags ());
        bool comb_values (pfx && !options.no_combined_values ());

        os << "bool " << name << "::" << endl
           << "_parse (" << cli << "::scanner& s," << endl
           << um << (pfx ? " opt_mode" : "") << "," << endl
           << um << " arg_mode)"
           << "{";

        if (comb_flags)
          os << "// Can't skip combined flags (--no-combined-flags)." << endl
             << "//" << endl
             << "assert (opt_mode != " << cli << "::unknown_mode::skip);"
             << endl;

        os << "bool r = false;";

        if (sep)
          os << "bool opt = true;" // Still recognizing options.
             << endl;

        os << "while (s.more ())"
           << "{"
           << "const char* o = s.peek ();";

        if (sep)
        {
          os << endl
             << "if (std::strcmp (o, \"" << opt_sep << "\") == 0)"
             << "{"
             << "opt = false;";
          if (!options.keep_separator ())
          {
            os << "s.skip ();" // We don't want to erase the separator.
               << "r = true;"
               << "continue;";
          }
          os << "}";
        }

        if (sep)
          os << "if (opt)"
             << "{";

        // First try the argument as is.
        //
        os << "if (_parse (o, s))"
           << "{"
           << "r = true;"
           << "continue;"
           << "}";

        if (pfx)
        {
          size_t n (opt_prefix.size ());

          os << "if (std::strncmp (o, \"" << opt_prefix << "\", " <<
            n << ") == 0 && o[" << n << "] != '\\0')"
             << "{";

          if (comb_values)
          {
            os << "// Handle combined option values." << endl
               << "//" << endl
               << "std::string co;" // Need to live until next block.
               << "if (const char* v = std::strchr (o, '='))"
               << "{"
               <<   "co.assign (o, 0, v - o);"
               <<   "++v;"
               <<                                                          endl
               <<   "int ac (2);"
               <<   "char* av[] ="
               <<   "{"
               <<   "const_cast<char*> (co.c_str ())," << endl
               <<   "const_cast<char*> (v)"
               <<   "};"
               <<   cli << "::argv_scanner ns (0, ac, av);"
               <<                                                          endl
               <<   "if (_parse (co.c_str (), ns))"
               <<   "{"
               <<     "// Parsed the option but not its value?" << endl
               <<     "//" << endl
               <<     "if (ns.end () != 2)" << endl
               <<       "throw " << cli << "::invalid_value (co, v);"
               <<                                                          endl
               <<     "s.next ();"
               <<     "r = true;"
               <<     "continue;"
               <<   "}"
               <<   "else"
               <<   "{"
               <<     "// Set the unknown option and fall through." << endl
               <<     "//" << endl
               <<     "o = co.c_str ();"
               <<   "}"
               << "}";
          }

          if (comb_flags)
          {
            os << "// Handle combined flags." << endl
               << "//" << endl
               << "char cf[" << n + 2 << "];" // Need to live until next block.
               << "{"
               << "const char* p = o + " << n << ";"
               << "for (; *p != '\\0'; ++p)"
               << "{"
               <<   "if (!((*p >= 'a' && *p <= 'z') ||" << endl
               <<         "(*p >= 'A' && *p <= 'Z') ||" << endl
               <<         "(*p >= '0' && *p <= '9')))" << endl
               <<     "break;"
               << "}"
               << "if (*p == '\\0')"
               << "{"
               <<   "for (p = o + " << n << "; *p != '\\0'; ++p)"
               <<   "{"
               <<     "std::strcpy (cf, \"" << opt_prefix << "\");"
               <<     "cf[" << n << "] = *p;"
               <<     "cf[" << n + 1 << "] = '\\0';"
               <<                                                          endl
               <<     "int ac (1);"
               <<     "char* av[] ="
               <<     "{"
               <<     "cf"
               <<     "};"
               <<     cli << "::argv_scanner ns (0, ac, av);"
               <<                                                          endl
               <<     "if (!_parse (cf, ns))" << endl
               <<       "break;"
               <<   "}"
               <<   "if (*p == '\\0')"
               <<   "{"
               <<     "// All handled." << endl
               <<     "//" << endl
               <<     "s.next ();"
               <<     "r = true;"
               <<     "continue;"
               <<   "}"
               <<   "else"
               <<   "{"
               <<     "// Set the unknown option and fall through." << endl
               <<     "//" << endl
               <<     "o = cf;"
               <<   "}"
               << "}"
               << "}";
          }

          // Unknown option.
          //
          os << "switch (opt_mode)"
             << "{"
             << "case " << cli << "::unknown_mode::skip:" << endl
             << "{"
             << "s.skip ();"
             << "r = true;"
             << "continue;"
             << "}"
             << "case " << cli << "::unknown_mode::stop:" << endl
             << "{"
             << "break;"
             << "}"
             << "case " << cli << "::unknown_mode::fail:" << endl
             << "{"
             << "throw " << cli << "::unknown_option (o);"
             << "}"
             << "}" // switch
             << "break;" // The stop case.
             << "}";
        }

        if (sep)
          os << "}";

        // Unknown argument.
        //
        os << "switch (arg_mode)"
           << "{"
           << "case " << cli << "::unknown_mode::skip:" << endl
           << "{"
           << "s.skip ();"
           << "r = true;"
           << "continue;"
           << "}"
           << "case " << cli << "::unknown_mode::stop:" << endl
           << "{"
           << "break;"
           << "}"
           << "case " << cli << "::unknown_mode::fail:" << endl
           << "{"
           << "throw " << cli << "::unknown_argument (o);"
           << "}"
           << "}" // switch
           << "break;" // The stop case.

           << "}" // for
           << "return r;"
           << "}";
      }
    }

  private:
    base_parse base_parse_;
    traversal::inherits inherits_base_parse_;

    base_merge base_merge_;
    traversal::inherits inherits_base_merge_;

    base_desc base_desc_;
    traversal::inherits inherits_base_desc_;

    option_merge option_merge_;
    traversal::names names_option_merge_;

    option_map option_map_;
    traversal::names names_option_map_;

    option_desc option_desc_;
    traversal::names names_option_desc_;
  };

  // Page usage.
  //
  struct class_usage: traversal::class_, context
  {
    class_usage (context& c, usage_type u, paragraph& p)
        : context (c), usage_ (u), para_ (p) {}

    virtual void
    traverse (type& c)
    {
      class_doc_type cd (class_doc (c));

      if (cd == cd_exclude)
        return;

      const char* t (
        (cd == cd_default || cd == cd_exclude_base
         ? gen_usage != ut_both || usage_ == ut_short
         : cd == cd_short) ? "" : "long_");

      string p (
        para_ == para_unknown
        ? "p"
        : cli + "::usage_para::" + (para_ == para_text ? "text" : "option"));

      os << "p = " << fq_name (c) << "::print_" << t << "usage (os, " <<
        p << ");"
         << endl;

      para_ = para_unknown;
    }

  private:
    usage_type usage_;
    paragraph& para_;
  };
}

void
generate_source (context& ctx)
{
  ostream& os (ctx.os);

  os << "#include <map>" << endl
     << endl;

  traversal::cli_unit unit;
  traversal::names unit_names;
  namespace_ ns (ctx);
  class_ cl (ctx);

  unit >> unit_names >> ns;
  unit_names >> cl;

  traversal::names ns_names;

  ns >> ns_names >> ns;
  ns_names >> cl;

  unit.dispatch (ctx.unit);

  // Entire page usage.
  //
  if (ctx.gen_usage != ut_none && ctx.options.page_usage_specified ())
  {
    const string& qn (ctx.options.page_usage ());
    string n (ctx.escape (ctx.substitute (ctx.ns_open (qn, false))));

    usage u (ctx.gen_usage);
    string up (ctx.cli + "::usage_para");
    string const& ost (ctx.options.ostream_type ());

    {
      os << up << endl
         << n << "usage (" << ost << "& os, " << up << " p)"
         << "{"
         << "CLI_POTENTIALLY_UNUSED (os);"
         << endl;

      paragraph p (para_unknown);

      traversal::cli_unit unit;
      traversal::names unit_names;
      traversal::namespace_ ns;
      doc dc (ctx, u == ut_both ? ut_short : u, p);
      class_usage cl (ctx, u == ut_both ? ut_short : u, p);

      unit >> unit_names;
      unit_names >> dc;
      unit_names >> ns;
      unit_names >> cl;

      traversal::names ns_names;

      ns >> ns_names;
      ns_names >> dc;
      ns_names >> ns;
      ns_names >> cl;

      unit.dispatch (ctx.unit);

      if (p != para_unknown)
        os << "p = " << up << (p == para_text ? "::text;" : "::option;")
           << endl;

      os << "return p;"
         << "}";

      ctx.verify_id_ref ();
    }

    // Long version.
    //
    if (u == ut_both)
    {
      os << up << endl
         << n << "long_usage (" << ost << "& os, " << up << " p)"
         << "{"
         << "CLI_POTENTIALLY_UNUSED (os);"
         << endl;

      paragraph p (para_unknown);

      traversal::cli_unit unit;
      traversal::names unit_names;
      traversal::namespace_ ns;
      doc dc (ctx, ut_long, p);
      class_usage cl (ctx, ut_long, p);

      unit >> unit_names;
      unit_names >> dc;
      unit_names >> ns;
      unit_names >> cl;

      traversal::names ns_names;

      ns >> ns_names;
      ns_names >> dc;
      ns_names >> ns;
      ns_names >> cl;

      unit.dispatch (ctx.unit);

      if (p != para_unknown)
        os << "p = " << up << (p == para_text ? "::text;" : "::option;")
           << endl;

      os << "return p;"
         << "}";

      ctx.verify_id_ref ();
    }

    ctx.ns_close (qn, false);
  }
}
