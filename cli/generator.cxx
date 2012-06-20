// file      : cli/generator.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include <cctype>  // std::toupper, std::is{alpha,upper,lower}
#include <string>
#include <fstream>
#include <iostream>

#include <cutl/fs/auto-remove.hxx>

#include <cutl/compiler/code-stream.hxx>
#include <cutl/compiler/cxx-indenter.hxx>

#include "header.hxx"
#include "inline.hxx"
#include "source.hxx"

#include "runtime-header.hxx"
#include "runtime-inline.hxx"
#include "runtime-source.hxx"

#include "man.hxx"
#include "html.hxx"

#include "context.hxx"
#include "generator.hxx"
#include "name-processor.hxx"

using namespace std;
using namespace cutl;

using semantics::path;

namespace
{
  static char const cxx_header[] =
    "// This code was generated by CLI, a command line interface\n"
    "// compiler for C++.\n"
    "//\n\n";

  static char const man_header[] =
    ".\\\"\n"
    ".\\\" The following documentation was generated by CLI, a command\n"
    ".\\\" line interface compiler for C++.\n"
    ".\\\"\n";

  static char const html_header[] =
    "\n"
    "<!-- \n"
    "     The following documentation was generated by CLI, a command\n"
    "     line interface compiler for C++.\n"
    "-->\n\n";

  string
  make_guard (string const& file, context& ctx)
  {
    string g (file);

    // Split words, e.g., "FooBar" to "Foo_Bar" and convert everything
    // to upper case.
    //
    string r;
    for (string::size_type i (0), n (g.size ()); i < n - 1; ++i)
    {
      char c1 (g[i]);
      char c2 (g[i + 1]);

      r += toupper (c1);

      if (isalpha (c1) && isalpha (c2) && islower (c1) && isupper (c2))
        r += "_";
    }
    r += toupper (g[g.size () - 1]);

    return ctx.escape (r);
  }

  void
  open (ifstream& ifs, string const& path)
  {
    ifs.open (path.c_str (), ios_base::in | ios_base::binary);

    if (!ifs.is_open ())
    {
      cerr << path << ": error: unable to open in read mode" << endl;
      throw generator::failed ();
    }
  }
}

generator::
generator ()
{
}

void generator::
generate (options const& ops, semantics::cli_unit& unit, path const& p)
{
  try
  {
    path file (p.leaf ());
    string base (file.base ().string ());

    bool gen_cxx (ops.generate_cxx ());
    bool gen_man (ops.generate_man ());
    bool gen_html (ops.generate_html ());

    if (!gen_cxx && !gen_man && !gen_html)
      gen_cxx = true;

    if (ops.stdout_ ())
    {
      if (gen_cxx)
      {
        cerr << "error: --stdout cannot be used with C++ output" << endl;
        throw failed ();
      }

      if (gen_man && gen_html)
      {
        cerr << "error: --stdout cannot be used with man and html output"
             << endl;
        throw failed ();
      }
    }

    fs::auto_removes auto_rm;

    // C++ output.
    //
    if (gen_cxx)
    {
      bool inl (!ops.suppress_inline ());

      string hxx_name (base + ops.hxx_suffix ());
      string ixx_name (base + ops.ixx_suffix ());
      string cxx_name (base + ops.cxx_suffix ());

      path hxx_path (hxx_name);
      path ixx_path (ixx_name);
      path cxx_path (cxx_name);

      if (!ops.output_dir ().empty ())
      {
        path dir (ops.output_dir ());

        hxx_path = dir / hxx_path;
        ixx_path = dir / ixx_path;
        cxx_path = dir / cxx_path;
      }

      // Process names.
      //
      {
        context ctx (cerr, unit, ops);
        process_names (ctx);
      }

      // Check if we need to generate the runtime code. If we include
      // another options file, then we assume the runtime is generated
      // there. However, to reduce the number of standard headers we
      // have to include in the generated header file, we will still
      // need to generate some template code in the source file.
      //
      bool runtime (true);
      for (semantics::cli_unit::includes_iterator i (unit.includes_begin ());
           runtime && i != unit.includes_end ();
           ++i)
      {
        if (i->is_a<semantics::cli_includes> ())
          runtime = false;
      }

      //
      //
      ofstream hxx (hxx_path.string ().c_str ());

      if (!hxx.is_open ())
      {
        cerr << "error: unable to open '" << hxx_path << "' in write mode"
             << endl;
        throw failed ();
      }

      auto_rm.add (hxx_path);

      //
      //
      ofstream ixx;

      if (inl)
      {
        ixx.open (ixx_path.string ().c_str (), ios_base::out);

        if (!ixx.is_open ())
        {
          cerr << "error: unable to open '" << ixx_path << "' in write mode"
               << endl;
          throw failed ();
        }

        auto_rm.add (ixx_path);
      }

      //
      //
      ofstream cxx (cxx_path.string ().c_str ());

      if (!cxx.is_open ())
      {
        cerr << "error: unable to open '" << cxx_path << "' in write mode"
             << endl;
        throw failed ();
      }

      auto_rm.add (cxx_path);

      // Print headers.
      //
      hxx << cxx_header;
      if (inl)
        ixx << cxx_header;
      cxx << cxx_header;

      typedef
        compiler::ostream_filter<compiler::cxx_indenter, char>
        cxx_filter;

      // Include settings.
      //
      bool br (ops.include_with_brackets ());
      string ip (ops.include_prefix ());
      string gp (ops.guard_prefix ());

      if (!ip.empty () && ip[ip.size () - 1] != '/')
        ip.append ("/");

      if (!gp.empty () && gp[gp.size () - 1] != '_')
        gp.append ("_");

      // HXX
      //
      {
        cxx_filter filt (hxx);
        context ctx (hxx, unit, ops);

        string guard (make_guard (gp + hxx_name, ctx));

        hxx << "#ifndef " << guard << endl
            << "#define " << guard << endl
            << endl;

        if (runtime)
          generate_runtime_header (ctx);

        generate_header (ctx);

        if (inl)
        {
          hxx << "#include " << (br ? '<' : '"') << ip << ixx_name <<
            (br ? '>' : '"') << endl
              << endl;
        }

        hxx << "#endif // " << guard << endl;
      }

      // IXX
      //
      if (inl)
      {
        cxx_filter filt (ixx);
        context ctx (ixx, unit, ops);

        if (runtime)
          generate_runtime_inline (ctx);

        generate_inline (ctx);
      }

      // CXX
      //
      {
        cxx_filter filt (cxx);
        context ctx (cxx, unit, ops);

        cxx << "#include " << (br ? '<' : '"') << ip << hxx_name <<
          (br ? '>' : '"') << endl
            << endl;

        if (runtime && !inl)
          generate_runtime_inline (ctx);

        generate_runtime_source (ctx, runtime);

        if (!inl)
          generate_inline (ctx);

        generate_source (ctx);
      }
    }

    // man output
    //
    if (gen_man)
    {
      // Prologue & epilogue.
      //
      ifstream prologue, epilogue;
      {
        string file (ops.man_prologue_file ());

        if (!file.empty ())
          open (prologue, file);
      }

      {
        string file (ops.man_epilogue_file ());

        if (!file.empty ())
          open (epilogue, file);
      }

      ofstream man;

      if (!ops.stdout_ ())
      {
        path man_path (base + ops.man_suffix ());

        if (!ops.output_dir ().empty ())
          man_path = path (ops.output_dir ()) / man_path;

        man.open (man_path.string ().c_str ());

        if (!man.is_open ())
        {
          cerr << "error: unable to open '" << man_path << "' in write mode"
               << endl;
          throw failed ();
        }

        auto_rm.add (man_path);
      }

      // The explicit cast helps VC++ 8.0 overcome its issues.
      //
      ostream& os (ops.stdout_ () ? cout : static_cast<ostream&> (man));

      if (prologue.is_open ())
        os << prologue.rdbuf ();

      os << man_header;

      context ctx (os, unit, ops);
      generate_man (ctx);

      if (epilogue.is_open ())
        os << epilogue.rdbuf ();
    }

    // HTML output
    //
    if (gen_html)
    {
      // Prologue & epilogue.
      //
      ifstream prologue, epilogue;
      {
        string file (ops.html_prologue_file ());

        if (!file.empty ())
          open (prologue, file);
      }

      {
        string file (ops.html_epilogue_file ());

        if (!file.empty ())
          open (epilogue, file);
      }

      ofstream html;

      if (!ops.stdout_ ())
      {
        path html_path (base + ops.html_suffix ());

        if (!ops.output_dir ().empty ())
          html_path = path (ops.output_dir ()) / html_path;

        html.open (html_path.string ().c_str ());

        if (!html.is_open ())
        {
          cerr << "error: unable to open '" << html_path << "' in write mode"
               << endl;
          throw failed ();
        }

        auto_rm.add (html_path);
      }

      // The explicit cast helps VC++ 8.0 overcome its issues.
      //
      ostream& os (ops.stdout_ () ? cout : static_cast<ostream&> (html));

      if (prologue.is_open ())
        os << prologue.rdbuf ();

      os << html_header;

      context ctx (os, unit, ops);
      generate_html (ctx);

      if (epilogue.is_open ())
        os << epilogue.rdbuf ();
    }

    auto_rm.cancel ();
  }
  catch (const generation_failed&)
  {
    // Code generation failed. Diagnostics has already been issued.
    //
    throw failed ();
  }
  catch (semantics::invalid_path const& e)
  {
    cerr << "error: '" << e.path () << "' is not a valid filesystem path"
         << endl;
    throw failed ();
  }
  catch (fs::error const&)
  {
    // Auto-removal of generated files failed. Ignore it.
    //
    throw failed ();
  }
}
