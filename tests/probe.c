#include <packer/probe.h>
#include <assert.h>

int main() {
  Host host;
  Host__init(&host);
  assert(Host__read_into(&host));
  Host__fprintf(stdout, &host);
  Host__clean(&host);
}
