// file      : cli/name-processor.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <set>
#include <sstream>

#include <cli/context.hxx>
#include <cli/name-processor.hxx>

using namespace std;

namespace
{
  typedef set<string> name_set;
  typedef ::context context_base;

  struct context: context_base
  {
    context (context_base& c): context_base (c) {}
    context (context& c): context_base (c) {}

  public:
    string
    find_name (string const& n, string const& suffix, name_set& set)
    {
      string name (escape (n + suffix));

      for (size_t i (1); set.find (name) != set.end (); ++i)
      {
        ostringstream os;
        os << i;
        name = escape (n + os.str () + suffix);
      }

      set.insert (name);
      return name;
    }

    string
    find_name (string const& n, name_set& set)
    {
      return find_name (n, "", set);
    }
  };

  struct primary_option: traversal::option, context
  {
    primary_option (context& c, name_set& set)
        : context (c), set_ (set)
    {
    }

    virtual void
    traverse (type& o)
    {
      string n (o.name ()), name;

      // Get rid of leading special characters, e.f., -, --, /, etc.
      //
      for (size_t i (0); i < n.size (); ++i)
      {
        if (isalpha (n[i]) || n[i] == '_')
        {
          name.assign (n.c_str (), i, n.size () - i);
          break;
        }
      }

      o.context ().set ("name", find_name (name, set_));
    }

  private:
    name_set& set_;
  };

  struct intermediate_option: traversal::option, context
  {
    intermediate_option (context& c, name_set& set)
        : context (c), set_ (set)
    {
    }

    virtual void
    traverse (type& o)
    {
      if (gen_specifier && o.type ().name () != "bool")
      {
        semantics::context& oc (o.context ());
        string const& base (oc.get<string> ("name"));
        oc.set ("specifier", find_name (base + "_specified", set_));
      }
    }

  private:
    name_set& set_;
  };

  struct secondary_option: traversal::option, context
  {
    secondary_option (context& c, name_set& set)
        : context (c), set_ (set)
    {
    }

    virtual void
    traverse (type& o)
    {
      semantics::context& oc (o.context ());
      string const& base (oc.get<string> ("name"));
      oc.set ("member", find_name (base + "_", set_));

      if (gen_specifier && o.type ().name () != "bool")
      {
        string const& base (oc.get<string> ("specifier"));
        oc.set ("specifier-member", find_name (base + "_", set_));
      }
    }

  private:
    name_set& set_;
  };

  struct class_: traversal::class_, context
  {
    class_ (context& c) : context (c) {}

    virtual void
    traverse (type& c)
    {
      semantics::context& cc (c.context ());

      cc.set ("member-name-set", name_set ());
      name_set& member_set (cc.get<name_set> ("member-name-set"));

      member_set.insert (escape (c.name ()));

      // First assign primary names.
      //
      {
        primary_option option (*this, member_set);
        traversal::names names (option);

        class_::names (c, names);
      }

      // Then assign intermediate names.
      //
      {
        intermediate_option option (*this, member_set);
        traversal::names names (option);

        class_::names (c, names);
      }

      // Finally assign secondary names.
      //
      {
        secondary_option option (*this, member_set);
        traversal::names names (option);

        class_::names (c, names);
      }
    }
  };

  void
  process_names_ (context_base& c)
  {
    context ctx (c);

    traversal::cli_unit unit;
    traversal::names unit_names;
    traversal::namespace_ ns;
    class_ cl (ctx);

    unit >> unit_names >> ns;
    unit_names >> cl;

    traversal::names ns_names;

    ns >> ns_names >> ns;
    ns_names >> cl;

    unit.dispatch (ctx.unit);
  }
}

void
process_names (context_base& c)
{
  process_names_ (c);
}
