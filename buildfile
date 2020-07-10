# file      : buildfile
# license   : MIT; see accompanying LICENSE file

# Glue buildfile that "pulls" all the packages.

import pkgs = */
./: $pkgs
