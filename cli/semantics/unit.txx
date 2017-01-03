// file      : cli/semantics/unit.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

namespace semantics
{
  template <typename T>
  T* cli_unit::
  lookup (std::string const& ss, std::string const& name, bool outer)
  {
    using std::string;

    // Resolve the starting scope in this unit, if any.
    //
    string::size_type b (0), e;
    scope* s (0);

    do
    {
      e = ss.find ("::", b);
      string n (ss, b, e == string::npos ? e : e - b);

      if (n.empty ())
        s = this;
      else
      {
        scope::names_iterator_pair ip (s->find (n));

        for (s = 0; ip.first != ip.second; ++ip.first)
          if ((s = dynamic_cast<scope*> (&ip.first->named ())))
            break;

        if (s == 0)
          break; // No such scope in this unit.
      }

      b = e;

      if (b == string::npos)
        break;

      b += 2;
    } while (true);

    // If we have the starting scope, then try to resolve the name in it.
    //
    if (s != 0)
    {
      b = 0;

      do
      {
        e = name.find ("::", b);
        string n (name, b, e == string::npos ? e : e - b);

        scope::names_iterator_pair ip (s->find (n));

        // If this is the last name, then see if we have the desired type.
        //
        if (e == string::npos)
        {
          for (; ip.first != ip.second; ++ip.first)
            if (T* r = dynamic_cast<T*> (&ip.first->named ()))
              return r;
        }
        // Otherwise, this should be a scope.
        //
        else
        {
          for (s = 0; ip.first != ip.second; ++ip.first)
            if ((s = dynamic_cast<scope*> (&ip.first->named ())))
              break;

          if (s == 0)
            break; // No such inner scope.
        }

        b = e;

        if (b == string::npos)
          break;

        b += 2;
      } while (true);
    }

    // If we are here, then that means the lookup didn't find anything in
    // this unit. The next step is to examine all the included units.
    //
    for (includes_iterator i (includes_begin ()); i != includes_end (); ++i)
    {
      if (cli_includes* ci = dynamic_cast<cli_includes*> (&*i))
        if (T* r = ci->includee ().lookup<T> (ss, name, false))
          return r;
    }

    // If we still haven't found anything, then the next step is to search
    // one-outer scope, unless it is the global namespace.
    //
    if (outer && !ss.empty ())
    {
      string n (ss, 0, ss.rfind ("::"));
      return lookup<T> (n, name, true);
    }

    return 0;
  }
}
