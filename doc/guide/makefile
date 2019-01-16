# file      : doc/guide/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
# license   : MIT; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../build/bootstrap.make

default  := $(out_base)/
install  := $(out_base)/.install
cleandoc := $(out_base)/.cleandoc

# Build.
#
$(default): $(out_base)/cli-guide.ps $(out_base)/cli-guide.pdf


$(out_base)/cli-guide.ps: $(src_base)/index.xhtml   \
                          $(src_base)/guide.html2ps \
                          | $(out_base)/.
	$(call message,html2ps $<,html2ps -f $(src_base)/guide.html2ps -o $@ $<)

$(out_base)/cli-guide.pdf: $(out_base)/cli-guide.ps | $(out_base)/.
	$(call message,ps2pdf $<,ps2pdf14 $< $@)

# Install.
#
$(install): $(out_base)/cli-guide.ps $(out_base)/cli-guide.pdf
	$(call install-data,$(src_base)/index.xhtml,$(install_doc_dir)/cli/guide/index.xhtml)
	$(call install-data,$(src_base)/cli-guide.ps,$(install_doc_dir)/cli/guide/cli-guide.ps)
	$(call install-data,$(src_base)/cli-guide.pdf,$(install_doc_dir)/cli/guide/cli-guide.pdf)

# Clean.
#
$(cleandoc):
	$(call message,rm $$1,rm -f $$1,$(out_base)/cli-guide.ps)
	$(call message,rm $$1,rm -f $$1,$(out_base)/cli-guide.pdf)

# How to.
#
$(call include,$(bld_root)/install.make)
