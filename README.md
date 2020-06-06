# CLI

CLI is a command line interface compiler for C++.

The development setup for CLI uses two configurations, for example:

```
git clone .../cli.git
cd cli

bdep init --empty

bdep config create @host ../cli-host --type host cc config.cxx=g++
bdep config create @target ../cli-target cc config.cxx=g++

bdep init @host -d cli
bdep init @target -d cli-tests -d cli-examples

```

To generate the documentation in the `.ps` and `.pdf` formats, `html2ps` and
`ps2pdf14` programs are required (the latter is from `ghostscript`). A warning
is issued in the development mode if these programs are not available.

Note that the development build cannot be installed (due to the bootstrap
process; see `cli/build/root.build` for details). To test installation, a
seperate, non-development build can be created:

```
bdep config create @install ../cli-install --type host cc config.cxx=g++ \
  config.install.root=/tmp/install

bdep init @install -d cli config.cli.develop=false
b install: ../cli-install/cli/
```
