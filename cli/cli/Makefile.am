
bin_PROGRAMS = cli

cli_SOURCES = \
    cli.cxx \
    context.cxx \
    generator.cxx \
    header.cxx \
    html.cxx \
    inline.cxx \
    lexer.cxx \
    man.cxx \
    name-processor.cxx \
    option-types.cxx \
    parser.cxx \
    pregenerated/cli/options.cxx \
    runtime-header.cxx \
    runtime-inline.cxx \
    runtime-source.cxx \
    semantics/class.cxx \
    semantics/doc.cxx \
    semantics/elements.cxx \
    semantics/expression.cxx \
    semantics/namespace.cxx \
    semantics/option.cxx \
    semantics/unit.cxx \
    source.cxx \
    traversal/class.cxx \
    traversal/elements.cxx \
    traversal/namespace.cxx \
    traversal/option.cxx \
    traversal/unit.cxx \
    txt.cxx


cli_CXXFLAGS = \
    -I$(top_builddir) \
    @CUTL_CFLAGS@

cli_LDADD = \
    @CUTL_LIBS@

# lexer_test_SOURCES = lexer.test.cxx
# parser_test_SOURCES = parser.test.cxx
