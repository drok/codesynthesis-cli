# file      : doc/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2009-2017 Code Synthesis Tools CC
# license   : MIT; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

default  := $(out_base)/
install  := $(out_base)/.install
cleandoc := $(out_base)/.cleandoc

$(default): $(out_base)/cli.xhtml $(out_base)/cli.1 $(out_base)/guide/

# Generated documentation.
#
$(out_base)/cli.xhtml: $(src_root)/cli/options.cli    \
                       $(src_base)/cli-prologue.xhtml \
                       $(src_base)/cli-epilogue.xhtml
	$(call message,cli-html $<,$(out_root)/cli/cli --generate-html \
--output-dir $(out_base) --html-prologue-file $(src_base)/cli-prologue.xhtml \
--html-epilogue-file $(src_base)/cli-epilogue.xhtml $<)
	$(call message,,mv $(out_base)/options.html $(out_base)/cli.xhtml)

$(out_base)/cli.1: $(src_root)/cli/options.cli    \
                       $(src_base)/cli-prologue.1 \
                       $(src_base)/cli-epilogue.1
	$(call message,cli-man $<,$(out_root)/cli/cli --generate-man \
--output-dir $(out_base) --man-prologue-file $(src_base)/cli-prologue.1 \
--man-epilogue-file $(src_base)/cli-epilogue.1 $<)
	$(call message,,mv $(out_base)/options.1 $(out_base)/cli.1)

# Install.
#
$(install): $(out_base)/guide/.install $(out_base)/cli.xhtml $(out_base)/cli.1
	$(call install-data,$(src_base)/default.css,$(install_doc_dir)/cli/default.css)
	$(call install-data,$(out_base)/cli.xhtml,$(install_doc_dir)/cli/cli.xhtml)
	$(call install-data,$(out_base)/cli.1,$(install_man_dir)/man1/cli.1)

# Clean.
#
$(cleandoc): $(out_base)/guide/.cleandoc

$(call include,$(bld_root)/install.make)

$(call import,$(src_base)/guide/makefile)
