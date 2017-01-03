// file      : cli/txt.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#ifndef CLI_TXT_HXX
#define CLI_TXT_HXX

#include <iosfwd>  // ostream
#include <string>
#include <cstddef> // size_t

#include <cli/context.hxx>

void
generate_txt (context&);

// Return the number of "text characters", ignoring any escape sequences
// (e.g., ANSI color).
//
std::size_t
txt_size (const std::string&,
          std::size_t p = 0,
          std::size_t n = std::string::npos);

// This function assumes that the initial opening part has already been
// written with the 'first' argument being the number of characters already
// written in the first line (e.g., an option name). The 'indent' argument
// specified how many spaces to indent each line. The 'escape' argument is
// the optional escape function (e.g., for the string literal output). The
// line_{start, end, blank} arguments specify optional extra text for the
// start/end of the non-empty line as well as for the blank (empty) line.
//
void
txt_wrap_lines (std::ostream& os,
                const std::string&,
                std::size_t indent = 0,
                std::size_t first = 0,
                const char* line_start = "",
                const char* line_end = "",
                const char* line_blank = "",
                std::string (*escape) (std::string const&) = 0);

#endif // CLI_TXT_HXX
