#include <mapreduce/executor.hh>
#include <common/context_singleton.hh>
#include <string>

using namespace eclipse;

int main (int argc, char ** argv) {

  string input = argv[1];

  Context context (input);
  context.run ();

  Executor exec (context);
  exec.establish ();

  return context.join ();
}
