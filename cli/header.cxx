// file      : cli/header.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2018 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <cli/header.hxx>

using namespace std;

namespace
{
  //
  //
  struct option: traversal::option, context
  {
    option (context& c) : context (c) {}

    virtual void
    traverse (type& o)
    {
      string name (ename (o));
      string type (o.type ().name ());

      os << "const " << type << "&" << endl
         << name << " () const;"
         << endl;

      if (modifier)
      {
        os << type << "&" << endl
           << name << " ();"
           << endl;

        os << "void" << endl
           << name << " (const " << type << "&);"
           << endl;
      }

      if (specifier && type != "bool")
      {
        string spec (especifier (o));

        os << "bool" << endl
           << spec << " () const;"
           << endl;

        if (modifier)
          os << "void" << endl
             << spec << " (bool);"
             << endl;
      }
    }
  };

  //
  //
  struct option_data: traversal::option, context
  {
    option_data (context& c) : context (c) {}

    virtual void
    traverse (type& o)
    {
      string member (emember (o));
      string type (o.type ().name ());

      os << type << " " << member << ";";

      if (specifier && type != "bool")
        os << "bool " << especifier_member (o) << ";";
    }
  };

  //
  //
  struct base: traversal::class_, context
  {
    base (context& c): context (c), first_ (true) {}

    virtual void
    traverse (type& c)
    {
      if (first_)
      {
        os << ": ";
        first_ = false;
      }
      else
        os << "," << endl
           << "  ";

      os << "public " << fq_name (c);
    }

  private:
    bool first_;
  };

  //
  //
  struct class_: traversal::class_, context
  {
    class_ (context& c)
        : context (c),
          option_ (c),
          option_data_ (c)
    {
      names_option_ >> option_;
      names_option_data_ >> option_data_;
    }

    virtual void
    traverse (type& c)
    {
      bool abst (c.abstract ());
      string name (escape (c.name ()));
      string um (cli + "::unknown_mode");

      os << "class " << name;

      {
        base b (*this);
        traversal::inherits i (b);
        inherits (c, i);
      }

      os << "{"
         << "public:" << endl;

      // c-tors
      //
      if (!abst)
      {
        os << name << " ();"
           << endl;

        // Are we generating parsing constructors or parse() functions?
        //
        string n;
        if (options.generate_parse ())
        {
          os << "// Return true if anything has been parsed." << endl
             << "//" << endl;

          n = string ("bool\n") + (name != "parse" ? "parse" : "parse_");
        }
        else
          n = name;

        os << n << " (int& argc," << endl
           << "char** argv," << endl
           << "bool erase = false," << endl
           << um << " option = " << um << "::fail," << endl
           << um << " argument = " << um << "::stop);"
           << endl;

        os << n << " (int start," << endl
           << "int& argc," << endl
           << "char** argv," << endl
           << "bool erase = false," << endl
           << um << " option = " << um << "::fail," << endl
           << um << " argument = " << um << "::stop);"
           << endl;

        os << n << " (int& argc," << endl
           << "char** argv," << endl
           << "int& end," << endl
           << "bool erase = false," << endl
           << um << " option = " << um << "::fail," << endl
           << um << " argument = " << um << "::stop);"
           << endl;

        os << n << " (int start," << endl
           << "int& argc," << endl
           << "char** argv," << endl
           << "int& end," << endl
           << "bool erase = false," << endl
           << um << " option = " << um << "::fail," << endl
           << um << " argument = " << um << "::stop);"
           << endl;

        os << n << " (" << cli << "::scanner&," << endl
           << um << " option = " << um << "::fail," << endl
           << um << " argument = " << um << "::stop);"
           << endl;
      }

      //
      //
      os << "// Option accessors" << (modifier ? " and modifiers." : ".") << endl
         << "//" << endl;

      names (c, names_option_);

      // Usage.
      //
      if (usage != ut_none)
      {
        string up (cli + "::usage_para");
        string const& ost (options.ostream_type ());

        os << "// Print usage information." << endl
           << "//" << endl;

        os << "static " << up << endl
           << "print_usage (" << ost << "&," << endl
           << up << " = " << up << "::none);"
           << endl;

        if (usage == ut_both)
          os << "static " << up << endl
             << "print_long_usage (" << ost << "&," << endl
             << up << " = " << up << "::none);"
             << endl;
      }

      // Description.
      //
      if (options.generate_description ())
      {
        os << "// Option description." << endl
           << "//" << endl
           << "static const " << cli << "::options&" << endl
           << "description ();"
           << endl;
      }

      os << "// Implementation details." << endl
         << "//" << endl
         << "protected:" << endl;

      // default c-tor (abstract)
      //
      if (abst)
        os << name << " ();"
           << endl;

      // fill ()
      //
      if (options.generate_description ())
        os << "friend struct _cli_" + name + "_desc_type;"
           << endl
           << "static void" << endl
           << "fill (" << cli << "::options&);"
           << endl;

      // _parse ()
      //
      os << "bool" << endl
         << "_parse (const char*, " << cli << "::scanner&);"
         << endl;

      // _parse ()
      //
      if (!abst)
        os << "private:" << endl
           << "bool" << endl
           << "_parse (" << cli << "::scanner&," << endl
           << um << " option," << endl
           << um << " argument);"
           << endl;

      // Data members.
      //
      os << "public:" << endl; //@@ tmp

      names (c, names_option_data_);

      os << "};";
    }

  private:
    option option_;
    traversal::names names_option_;

    option_data option_data_;
    traversal::names names_option_data_;
  };

  //
  //
  struct includes_: traversal::cxx_includes,
                    traversal::cli_includes,
                    context
  {
    includes_ (context& c) : context (c) {}

    virtual void
    traverse (semantics::cxx_includes& i)
    {
      generate (i.kind (), i.file ().string ());
    }

    virtual void
    traverse (semantics::cli_includes& i)
    {
      generate (i.kind (),
                (options.output_prefix () +
                 i.file ().base ().string () +
                 options.output_suffix () +
                 options.hxx_suffix ()));
    }

    void
    generate (semantics::includes::kind_type k, string const& f)
    {
      char b, e;
      if (k == semantics::includes::quote)
        b = e = '"';
      else
      {
        b = '<';
        e = '>';
      }

      os << "#include " << b << f << e << endl
         << endl;
    }
  };
}

void
generate_header (context& ctx)
{
  ostream& os (ctx.os);

  traversal::cli_unit unit;
  includes_ includes (ctx);
  traversal::names unit_names;
  namespace_ ns (ctx);
  class_ cl (ctx);

  unit >> includes;
  unit >> unit_names >> ns;
  unit_names >> cl;

  traversal::names ns_names;

  ns >> ns_names >> ns;
  ns_names >> cl;

  unit.dispatch (ctx.unit);

  // Entire page usage.
  //
  if (ctx.usage != ut_none && ctx.options.page_usage_specified ())
  {
    os << "// Print page usage information." << endl
       << "//" << endl;

    const string& qn (ctx.options.page_usage ());
    string n (ctx.escape (ctx.substitute (ctx.ns_open (qn, false))));

    string up (ctx.cli + "::usage_para");
    string const& ost (ctx.options.ostream_type ());

    os << up << endl
       << n << "usage (" << ost << "&," << endl
       << up << " = " << up << "::none);"
       << endl;

    if (ctx.usage == ut_both)
      os << up << endl
         << n << "long_usage (" << ost << "&," << endl
         << up << " = " << up << "::none);"
         << endl;

    ctx.ns_close (qn, false);
  }
}
