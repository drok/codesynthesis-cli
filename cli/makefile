# file      : cli/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
# license   : MIT; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

cxx_tun := cli.cxx lexer.cxx parser.cxx

cxx_tun +=               \
option-types.cxx         \
options.cxx              \
context.cxx              \
header.cxx               \
inline.cxx               \
source.cxx               \
runtime-header.cxx       \
runtime-inline.cxx       \
runtime-source.cxx       \
man.cxx                  \
html.cxx                 \
generator.cxx            \
name-processor.cxx

cxx_tun +=               \
semantics/class.cxx      \
semantics/doc.cxx        \
semantics/elements.cxx   \
semantics/expression.cxx \
semantics/namespace.cxx  \
semantics/option.cxx     \
semantics/unit.cxx

cxx_tun +=               \
traversal/class.cxx      \
traversal/elements.cxx   \
traversal/namespace.cxx  \
traversal/option.cxx     \
traversal/unit.cxx

cli_tun := options.cli

#
#
cxx_obj  := $(addprefix $(out_base)/,$(cxx_tun:.cxx=.o) $(cli_tun:.cli=.o))
cxx_od   := $(cxx_obj:.o=.o.d)

cli      := $(out_base)/cli
clean    := $(out_base)/.clean
install  := $(out_base)/.install

# Import.
#
$(call import,\
  $(scf_root)/import/libcutl/stub.make,\
  l: cutl.l,cpp-options: cutl.l.cpp-options)

# Build.
#
$(cli): $(cxx_obj) $(cutl.l)

$(cxx_obj) $(cxx_od): cpp_options := -I$(src_base) -I$(out_base)
$(cxx_obj) $(cxx_od): $(cutl.l.cpp-options)

genf := $(cli_tun:.cli=.hxx) $(cli_tun:.cli=.ixx) $(cli_tun:.cli=.cxx)
gen  := $(addprefix $(out_base)/,$(genf))

$(gen): cli := $(out_root)/cli/cli
$(gen): cli_options += --generate-file-scanner --generate-specifier \
--guard-prefix CLI --reserved-name stdout

$(call include-dep,$(cxx_od),$(cxx_obj),$(gen))

# Alias for default target.
#
$(out_base)/: $(cli)

# Install.
#
$(install): $(cli)
	$(call install-exec,$<,$(install_bin_dir)/cli)

# Clean.
#
$(clean):                            \
  $(cli).o.clean                     \
  $(addsuffix .cxx.clean,$(cxx_obj)) \
  $(addsuffix .cxx.clean,$(cxx_od))

# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(cli): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := cli
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif

# how to
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)
$(call include,$(scf_root)/cli/cli-cxx.make)
$(call include,$(bld_root)/install.make)
