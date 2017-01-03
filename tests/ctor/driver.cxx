// file      : tests/ctor/driver.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include "test.hxx"

int
main (int argc, char* argv[])
{
  // Test that we can call all the c-tors unambiguously.
  //
  {
    options o1 (argc, argv);
    options o2 (argc, argv, true);
    options o3 (argc, argv, true,
                cli::unknown_mode::fail);
    options o4 (argc, argv, true,
                cli::unknown_mode::fail,
                cli::unknown_mode::stop);
  }

  {
    options o1 (1, argc, argv);
    options o2 (1, argc, argv, true);
    options o3 (1, argc, argv, true,
                cli::unknown_mode::fail);
    options o4 (1, argc, argv, true,
                cli::unknown_mode::fail,
                cli::unknown_mode::stop);
  }

  {
    int end;
    options o1 (argc, argv, end);
    options o2 (argc, argv, end, true);
    options o3 (argc, argv, end, true,
                cli::unknown_mode::fail);
    options o4 (argc, argv, end, true,
                cli::unknown_mode::fail,
                cli::unknown_mode::stop);
  }

  {
    int end;
    options o1 (1, argc, argv, end);
    options o2 (1, argc, argv, end, true);
    options o3 (1, argc, argv, end, true,
                cli::unknown_mode::fail);
    options o4 (1, argc, argv, end, true,
                cli::unknown_mode::fail,
                cli::unknown_mode::stop);
  }

  {
    cli::argv_scanner s (argc, argv);
    options o1 (s);
    options o2 (s, cli::unknown_mode::fail);
    options o3 (s, cli::unknown_mode::fail, cli::unknown_mode::stop);
  }
}
