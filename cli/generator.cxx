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
#include "txt.hxx"

#include "context.hxx"
#include "generator.hxx"
#include "name-processor.hxx"

using namespace std;
using namespace cutl;

using semantics::path;

namespace
{
  static char const cxx_header[] =
    "// -*- C++ -*-\n"
    "//\n"
    "// This file was generated by CLI, a command line interface\n"
    "// compiler for C++.\n"
    "//\n\n";

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

  void
  append (ostream& os,
          vector<string> const& text,
          string const& file,
          semantics::cli_unit& u)
  {
    for (vector<string>::const_iterator i (text.begin ());
         i != text.end (); ++i)
      os << context::substitute (*i, u) << endl;

    if (!file.empty ())
    {
      ifstream ifs;
      open (ifs, file);

      // getline() will set the failbit if it failed to extract anything,
      // not even the delimiter and eofbit if it reached eof before seeing
      // the delimiter.
      //
      for (string s; getline (ifs, s); )
        os << context::substitute (s, u) << endl;
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

    const string& pfx (ops.output_prefix ());
    const string& sfx (ops.output_suffix ());

    bool gen_cxx (ops.generate_cxx ());
    bool gen_man (ops.generate_man ());
    bool gen_html (ops.generate_html ());
    bool gen_txt (ops.generate_txt ());

    if (!gen_cxx && !gen_man && !gen_html && !gen_txt)
      gen_cxx = true;

    if (ops.stdout_ ())
    {
      if (gen_cxx)
      {
        cerr << "error: --stdout cannot be used with C++ output" << endl;
        throw failed ();
      }

      if ((gen_man  && gen_html) ||
          (gen_man  && gen_txt)  ||
          (gen_html && gen_txt))
      {
        cerr << "error: --stdout cannot only be used with one output format"
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

      string hxx_name (pfx + base + sfx + ops.hxx_suffix ());
      string ixx_name (pfx + base + sfx + ops.ixx_suffix ());
      string cxx_name (pfx + base + sfx + ops.cxx_suffix ());

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
        context ctx (hxx, unit, ops);

        string guard (make_guard (gp + hxx_name, ctx));

        hxx << "#ifndef " << guard << endl
            << "#define " << guard << endl
            << endl;

        // Copy prologue.
        //
        hxx << "// Begin prologue." << endl
            << "//" << endl;
        append (hxx, ops.hxx_prologue (), ops.hxx_prologue_file (), unit);
        hxx << "//" << endl
            << "// End prologue." << endl
            << endl;

        {
          // We don't want to indent prologues/epilogues.
          //
          cxx_filter filt (ctx.os);

          if (runtime)
            generate_runtime_header (ctx);

          generate_header (ctx);
        }

        if (inl)
        {
          hxx << "#include " << (br ? '<' : '"') << ip << ixx_name <<
            (br ? '>' : '"') << endl
              << endl;
        }

        // Copy epilogue.
        //
        hxx << "// Begin epilogue." << endl
            << "//" << endl;
        append (hxx, ops.hxx_epilogue (), ops.hxx_epilogue_file (), unit);
        hxx << "//" << endl
            << "// End epilogue." << endl
            << endl;

        hxx << "#endif // " << guard << endl;
      }

      // IXX
      //
      if (inl)
      {
        context ctx (ixx, unit, ops);

        // Copy prologue.
        //
        ixx << "// Begin prologue." << endl
            << "//" << endl;
        append (ixx, ops.ixx_prologue (), ops.ixx_prologue_file (), unit);
        ixx << "//" << endl
            << "// End prologue." << endl
            << endl;

        {
          // We don't want to indent prologues/epilogues.
          //
          cxx_filter filt (ctx.os);

          if (runtime)
            generate_runtime_inline (ctx);

          generate_inline (ctx);
        }

        // Copy epilogue.
        //
        ixx << "// Begin epilogue." << endl
            << "//" << endl;
        append (ixx, ops.ixx_epilogue (), ops.ixx_epilogue_file (), unit);
        ixx << "//" << endl
            << "// End epilogue." << endl;
      }

      // CXX
      //
      {
        context ctx (cxx, unit, ops);

        // Copy prologue.
        //
        cxx << "// Begin prologue." << endl
            << "//" << endl;
        append (cxx, ops.cxx_prologue (), ops.cxx_prologue_file (), unit);
        cxx << "//" << endl
            << "// End prologue." << endl
            << endl;

        cxx << "#include " << (br ? '<' : '"') << ip << hxx_name <<
          (br ? '>' : '"') << endl
            << endl;

        {
          // We don't want to indent prologues/epilogues.
          //
          cxx_filter filt (ctx.os);

          if (runtime && !inl)
            generate_runtime_inline (ctx);

          generate_runtime_source (ctx, runtime);

          if (!inl)
            generate_inline (ctx);

          generate_source (ctx);
        }

        // Copy epilogue.
        //
        cxx << "// Begin epilogue." << endl
            << "//" << endl;
        append (cxx, ops.cxx_epilogue (), ops.cxx_epilogue_file (), unit);
        cxx << "//" << endl
            << "// End epilogue." << endl
            << endl;
      }
    }

    // man output
    //
    if (gen_man)
    {
      ofstream man;

      if (!ops.stdout_ ())
      {
        path man_path (pfx + base + sfx + ops.man_suffix ());

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

      append (os, ops.man_prologue (), ops.man_prologue_file (), unit);

      context ctx (os, unit, ops);
      generate_man (ctx);

      append (os, ops.man_epilogue (), ops.man_epilogue_file (), unit);
    }

    // HTML output
    //
    if (gen_html)
    {
      ofstream html;

      if (!ops.stdout_ ())
      {
        // May have to update link derivation in format_line() if changing
        // this.
        //
        path html_path (pfx + base + sfx + ops.html_suffix ());

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

      append (os, ops.html_prologue (), ops.html_prologue_file (), unit);

      context ctx (os, unit, ops);
      generate_html (ctx);

      append (os, ops.html_epilogue (), ops.html_epilogue_file (), unit);
    }

    // txt output
    //
    if (gen_txt)
    {
      ofstream txt;

      if (!ops.stdout_ ())
      {
        path txt_path (pfx + base + sfx + ops.txt_suffix ());

        if (!ops.output_dir ().empty ())
          txt_path = path (ops.output_dir ()) / txt_path;

        txt.open (txt_path.string ().c_str ());

        if (!txt.is_open ())
        {
          cerr << "error: unable to open '" << txt_path << "' in write mode"
               << endl;
          throw failed ();
        }

        auto_rm.add (txt_path);
      }

      // The explicit cast helps VC++ 8.0 overcome its issues.
      //
      ostream& os (ops.stdout_ () ? cout : static_cast<ostream&> (txt));

      append (os, ops.txt_prologue (), ops.txt_prologue_file (), unit);

      context ctx (os, unit, ops);
      generate_txt (ctx);

      append (os, ops.txt_epilogue (), ops.txt_epilogue_file (), unit);
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
