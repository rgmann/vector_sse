require 'mkmf'

# Give it a name
extension_name = 'vector_sse'

$CFLAGS << ' -O3'

# Check for dependencies
have_header( 'emmintrin.h' )

# Do the work
create_makefile "vector_sse/vector_sse"

