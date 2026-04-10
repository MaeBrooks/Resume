#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define TLMGR "/Library/tex/distributions/Programs/texbin/tlmgr"
#define XELATEX "/Library/tex/distributions/texlive-2025-basic.texdist/contents/programs/arm64/xelatex"

#define TLMGR_DEPS "tikz", "changepage", "textpos", "babel", "german", "titlesec", "substr", "isodate"
static char* LOG_FILE = "nob.log";

int main(int argc, char** argv) {
  NOB_GO_REBUILD_URSELF_PLUS(argc, argv, "resume.tex");

  LOG_FILE = NULL;
  if (file_exists(LOG_FILE)) {
    if (!delete_file(LOG_FILE)) {
      nob_log(NOB_WARNING, "Unable to delete: '%s'", LOG_FILE);
    }
  }

  Cmd cmd = {0};
  cmd_append(&cmd, TLMGR, "list", "--only-installed", TLMGR_DEPS);

  if (!cmd_run(&cmd, .stdout_path = LOG_FILE)) {

    // If the packages cannot be found, You might want to take a look at: tlmgr option repository ctan
    cmd_append(&cmd, "sudo", TLMGR, "install", TLMGR_DEPS);
    if (!cmd_run(&cmd)) {
      nob_log(NOB_ERROR, "%s", "Failed to install packages?");
      return 1;
    }
  }

  cmd_append(&cmd, XELATEX, "-no-file-line-error", "resume.tex");
  if (!cmd_run(&cmd, .stdout_path = LOG_FILE)) {
    // This command basically ALWAYS fails - even when successful, so we dont treat it as an actual error. Check the logs if its not working right
    nob_log(NOB_WARNING, "%s", "Xelatex Returned non 0 error! It may have failed!");
  }

  cmd_append(&cmd, "mv", "resume.pdf", "Mae Brooks Resume.pdf");
  if (!cmd_run(&cmd)) {
    nob_log(NOB_ERROR, "%s", "Failed to rename resume.pdf");
    return 1;
  }

  return 0;
}
