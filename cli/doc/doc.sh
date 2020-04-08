#! /usr/bin/env bash

version=1.2.0-b.6

trap 'exit 1' ERR
set -o errtrace # Trap in functions.

function info () { echo "$*" 1>&2; }
function error () { info "$*"; exit 1; }

date="$(date +"%B %Y")"
copyright="$(sed -n -re 's%^Copyright \(c\) (.+)\.$%\1%p' ../LICENSE)"

while [ $# -gt 0 ]; do
  case $1 in
    --clean)
      rm -f cli.xhtml cli.1
      rm -f guide/cli-guide.ps guide/cli-guide.pdf
      exit 0
      ;;
    *)
      error "unexpected $1"
      ;;
  esac
done

function compile () # <input-name> <output-name>
{
  local i=$1; shift
  local o=$1; shift

  # Use a bash array to handle empty arguments.
  #
  local ops=()
  while [ $# -gt 0 ]; do
    ops=("${ops[@]}" "$1")
    shift
  done

  # --html-suffix .xhtml
  ../cli/cli -I .. \
-v project="cli" \
-v version="$version" \
-v date="$date" \
-v copyright="$copyright" \
"${ops[@]}" --generate-html  --stdout \
--html-prologue-file cli-prologue.xhtml \
--html-epilogue-file cli-epilogue.xhtml \
"../cli/$i.cli" >"$o.xhtml"

  # --man-suffix .1
  ../cli/cli -I .. \
-v project="cli" \
-v version="$version" \
-v date="$date" \
-v copyright="$copyright" \
"${ops[@]}" --generate-man  --stdout \
--man-prologue-file cli-prologue.1 \
--man-epilogue-file cli-epilogue.1 \
"../cli/$i.cli" >"$o.1"
}

compile options cli --suppress-undocumented

# Manual.
#

#function compile_doc ()
#{
#  html2ps -f doc.html2ps:a4.html2ps -o "$n-a4.ps" "$n.xhtml"
#  ps2pdf14 -sPAPERSIZE=a4 -dOptimize=true -dEmbedAllFonts=true "$n-a4.ps" "$n-a4.pdf"
#
#  html2ps -f doc.html2ps:letter.html2ps -o "$n-letter.ps" "$n.xhtml"
#  ps2pdf14 -sPAPERSIZE=letter -dOptimize=true -dEmbedAllFonts=true "$n-letter.ps" "$n-letter.pdf"
#}

html2ps -f guide/guide.html2ps -o guide/cli-guide.ps guide/index.xhtml
ps2pdf14 -dOptimize=true -dEmbedAllFonts=true guide/cli-guide.ps guide/cli-guide.pdf
