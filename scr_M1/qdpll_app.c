/*
 This file is part of DepQBF.

 DepQBF, a solver for quantified boolean formulae (QBF).        
 Copyright 2013, 2014, 2015, 2016, 2017 Florian Lonsing,
   Vienna University of Technology, Vienna, Austria.
 Copyright 2010, 2011, 2012 Florian Lonsing,
   Johannes Kepler University, Linz, Austria.
 Copyright 2012 Aina Niemetz,
   Johannes Kepler University, Linz, Austria.

 DepQBF is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or (at
 your option) any later version.

 DepQBF is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with DepQBF.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/time.h>
#include <chrono>
#include "qdpll.h"
#include "qdpll_internals.h"

#define VERSION                                                         \
  "DepQBF 6.03\n"                                                        \
  "Copyright 2013, 2014, 2015, 2016, 2017 Florian Lonsing,\n"           \
  "  Vienna University of Technology, Vienna, Austria.\n"               \
  "Copyright 2010, 2011, 2012 Florian Lonsing,\n"                       \
  "  Johannes Kepler University, Linz, Austria.\n"                       \
  "Copyright 2012 Aina Niemetz,\n"                                       \
  "  Johannes Kepler University, Linz, Austria.\n"                        \
  "This is free software; see COPYING for copying conditions.\n"        \
  "There is NO WARRANTY, to the extent permitted by law.\n"

#define USAGE1 \
"usage: depqbf [ <option> ... ] [ <in-file> ]\n"\
"\n"\
"  where <in-file> is a file in (Q)DIMACS format (default: stdin)\n"\
"  and <option> is any combination of the following:\n"\
"\n"\
"  Note: see function 'qdpll_configure' in file 'qdpll.c' for further, undocumented\n"\
"    options which are not listed here. However, many of those are experimental.\n"\
"\n"\
"  -h, --help                      display usage information\n"\
"  -v                              enable verbosity incrementally (at most '-v -v')\n"\
"  --pretty-print                  print the parsed formula after cleaning up (delete tautologies,\n"\
"                                    superfluous variables, quantifier blocks, and literals; sort \n"\
"                                    literals in clauses by quantifier nesting levels and IDs).\n"\
"  --incremental-use               Relevant ONLY for API use: enable incremental solving. \n"\
 "                                   Must be combined with '--dep-man=simple'. \n"      \
"  --qdo                           QDIMACS output generation (partial certificate):\n"\
"                                    If the outermost (i.e. leftmost) quantifier block\n"\
"                                    of a satisfiable QBF is existentially quantified,\n"\
"                                    then print an assignment to the variables of this\n"\
"                                    block (and dual for unsatisfiable QBFs and\n"\
"                                    universal variables from the outermost block,\n"\
"                                    if that block is universally quantified).\n"\
"                                    IMPORTANT: must be combined with '--no-dynamic-nenofex'\n"\
"  --traditional-qcdcl             apply a traditional variant of clause and cube learning (QCDCL),\n"\
"                                    which was applied in previous versions of DepQBF.\n"\
"                                    In this version, by default lazy QPUP-based QCDCL is applied.\n"\
"  --long-dist-res                 Apply long-distance resolution in constraint learning. \n"\
"                                    Should be combined with '--dep-man=simple'.\n"\
"                                    Optionally, '--traditional-qcdcl' may be specified which enables\n"\
"                                    long-distance resolution where intermediate resolutions are carried\n"\
"                                    out starting from the empty clause.\n" \
"  --no-lazy-qpup                  disable lazy QPUP-based QCDCL and carry out all resolution\n"\
"                                    steps starting from a separating cut in the implication graph.\n"\
"                                    This works also with long-distance resolution.\n"                   \
"  --no-qpup-cdcl                  apply traditional QCDCL for clause learning (instead of QPUP).\n"\
"  --no-qpup-sdcl                  apply traditional QCDCL for cube learning (instead of QPUP).\n"\
"  --trace[=<format>]              dump trace in <format> to <stdout>\n"\
"                                    format: qrp  ... ascii QRP format (default)\n"\
"                                            bqrp ... binary QRP format\n"\
"                                    NOTE: tracing must be combined with options '--dep-man=simple' and\n"\
"                                          either '--traditional-qcdcl' (which disables QPUP-based QCDCL)\n"\
"                                          or '--no-lazy-qpup' (to enable tracing with QPUP-based QCDCL).\n"

#define USAGE2 \
"  --dep-man=<val>                 set dependency manager: if <val>=qdag (default) then the solver\n"\
"                                    uses the standard dependency scheme; if <val>=simple then the\n"\
"                                    solver uses the given quantifier prefix of the input formula\n"	\
"  --no-cdcl                       disable conflict-driven clause learning and\n"\
"                                    backtrack chronologically from conflicts\n"\
"  --no-sdcl                       disable solution-driven cube learning and\n"\
"                                    backtrack chronologically from solutions\n"\
"  --no-pure-literals              disable pure literal detection\n"\
"  --no-spure-literals             DEPRECATED: include ALL constraints for pure literal detection\n"\
"  --no-unit-mtf                   no move-to-front (MTF) of learnt constraints which became unit\n"\
"  --no-res-mtf                    no move-to-front (MTF) of learnt constraints which became empty\n"\
"  --lclauses-init-size=<val>      initially allow <val> clauses to be learned before resizing the clause list\n"\
"  --lcubes-init-size=<val>        initially allow <val> cubes to be learned before resizing the cube list\n"\
"  --lclauses-resize-value=<val>   increase capacity of learned clauses list by <val>\n"\
"  --lcubes-resize-value=<val>     increase capacity of learned cubes list by <val>\n"\
"  --orestart-dist-init=<val>      set initial distance of outer restarts to <val> (default 10)\n"\
"  --orestart-dist-inc=<val>       increase distance of outer restarts by <val> (default 5)\n"\
"  --irestart-dist-init=<val>      set initial distance of inner restarts to <val> (default 100)\n"\
"  --irestart-dist-inc=<val>       increase distance of inner restarts by <val> (default 10)\n"\
"  --max-dec=<val>                 Abort after <val> assignments by decision making.\n"\
"  --max-btracks=<val>             Abort after <val> backtracks.\n"\
"  --max-secs=<val>                Abort after <val> seconds.\n"\
"\n"\
"Options that control QBCE and applications of generalized axioms:"\
"\n"\
"  --no-qbce-dynamic               disable dynamic QBCE (enabled by default)\n"\
"  --qbce-preprocessing            enable QBCE preprocessing (must be preceded by '--no-qbce-dynamic')\n"\
"  --qbce-inprocessing             enable QBCE inprocessing (must be preceded by '--no-qbce-dynamic')\n"\
"  --qbce-witness-max-occs=<val>   maximum number <val> of occurrences considered in QBCE (default: 50)\n"\
"  --qbce-max-clause-size=<val>    maximum length <val> of clauses considered in QBCE (default: 50)\n"\
"  --no-empty-formula-watching     disable empty formula watching (default: enabled)\n"\
"  --no-dynamic-nenofex            disable dynamic nenofex tests (default: enabled)\n"\
"  --dyn-nenofex-ignore-unsat      ignore unsatisfiable branch detected by nenofex\n"\
"  --dyn-nenofex-ignore-sat        ignore satisfiable branch detected by nenofex\n"\
"  --no-trivial-falsity            disable trivial falsity tests (default: enabled)\n"\
"  --no-trivial-truth              disable trivial truth tests (default: enabled)\n"\
"\n"



#define QDPLL_ABORT_APP(cond,msg) \
  do {                                  \
    if (cond)                                                                \
      {                                                                        \
        fprintf (stderr, "[QDPLL-APP] %s at line %d: %s\n", __func__,        \
                 __LINE__, msg);                                        \
        fflush (stderr);                                                \
        abort();                                                        \
      }                                                                        \
  } while (0)


struct QDPLLApp
{
  struct
  {
    char *in_filename;
    FILE *in;
    int pretty_print;
    int qdimacs_output;
    int deps_only;
    int print_deps;
    int dump_dep_graph;
    int trace;
    unsigned int max_time;
    unsigned int verbosity;
    unsigned int print_usage;
    unsigned int print_version;
    unsigned int seed;
  } options;
};

typedef struct QDPLLApp QDPLLApp;

/* We keep a static pointer to the library object. Currently, this is
   used for calling library functions from within a signal handler. */
static QDPLL *qdpll = 0;

static void
print_abort_err (QDPLLApp * app, char *msg, ...)
{
  va_list list;
  assert (msg != NULL);
  fprintf (stderr, "qdpll-app: ");
  va_start (list, msg);
  vfprintf (stderr, msg, list);
  va_end (list);
  fflush (stderr);
  abort ();
}



/* -------------------- START: PARSING -------------------- */
#define PARSER_READ_NUM(num, c)                        \
  assert (isdigit (c));                                \
  num = 0;					       \
  do						       \
    {						       \
      num = num * 10 + (c - '0');		       \
    }						       \
  while (isdigit ((c = getc (in))));

#define PARSER_SKIP_SPACE_DO_WHILE(c)		     \
  do						     \
    {                                                \
      c = getc (in);				     \
    }                                                \
  while (isspace (c));

#define PARSER_SKIP_SPACE_WHILE(c)		     \
  while (isspace (c))                                \
    c = getc (in);

static void
parse (QDPLLApp * app, QDPLL * qdpll, FILE * in, int trace)
{
  int col = 0, line = 0, neg = 0, preamble_found = 0;
  LitID num = 0;
  QDPLLQuantifierType scope_type = QDPLL_QTYPE_UNDEF;

  assert (in);

  int c;
  while ((c = getc (in)) != EOF)
    {
      PARSER_SKIP_SPACE_WHILE (c);

      while (c == 'c')
        {
          while ((c = getc (in)) != '\n' && c != EOF)
            ;
          c = getc (in);
        }

      PARSER_SKIP_SPACE_WHILE (c);

      if (c == 'p')
        {
          /* parse preamble */
          PARSER_SKIP_SPACE_DO_WHILE (c);
          if (c != 'c')
            goto MALFORMED_PREAMBLE;
          PARSER_SKIP_SPACE_DO_WHILE (c);
          if (c != 'n')
            goto MALFORMED_PREAMBLE;
          PARSER_SKIP_SPACE_DO_WHILE (c);
          if (c != 'f')
            goto MALFORMED_PREAMBLE;
          PARSER_SKIP_SPACE_DO_WHILE (c);
          if (!isdigit (c))
            goto MALFORMED_PREAMBLE;

          /* read number of variables */
          PARSER_READ_NUM (num, c);
          if (trace == TRACE_QRP)
            fprintf (stdout, "p qrp %u", num);  
          else if (trace == TRACE_BQRP)
            fprintf (stdout, "p bqrp %u", num); 

          PARSER_SKIP_SPACE_WHILE (c);
          if (!isdigit (c))
            goto MALFORMED_PREAMBLE;

          /* read number of clauses */
          PARSER_READ_NUM (num, c);

          /* NOTE: number of steps is number of orig. clauses, as we can't tell
             the actual, future number of steps when tracing on-the-fly! */
          if (trace)
            fprintf (stdout, " %u%c", num, trace == TRACE_QRP ? '\n' : 0);

          preamble_found = 1;
          goto PARSE_SCOPE_OR_CLAUSE;

        MALFORMED_PREAMBLE:
          QDPLL_ABORT_APP (1, "malformed preamble!\n");
          return;
        }
      else
        {
          QDPLL_ABORT_APP (1, "expecting preamble!\n");
          return;
        }

    PARSE_SCOPE_OR_CLAUSE:

      PARSER_SKIP_SPACE_WHILE (c);

      if (c == 'a' || c == 'e')
        {
          /* open a new scope */
          if (c == 'a')
            scope_type = QDPLL_QTYPE_FORALL;
          else
            scope_type = QDPLL_QTYPE_EXISTS;

          qdpll_new_scope (qdpll, scope_type);

          PARSER_SKIP_SPACE_DO_WHILE (c);
        }

      if (!isdigit (c) && c != '-')
        {
          if (c == EOF)
            return;
          QDPLL_ABORT_APP (1, "expecting digit or '-'!\n");
          return;
        }
      else
        {
          if (c == '-')
            {
              neg = 1;
              if (!isdigit ((c = getc (in))))
                {
                  QDPLL_ABORT_APP (1, "expecting digit!\n");
                  return;
                }
            }

          /* parse a literal or variable ID */
          PARSER_READ_NUM (num, c);
          num = neg ? -num : num;

          qdpll_add (qdpll, num);

          neg = 0;

          goto PARSE_SCOPE_OR_CLAUSE;
        }
    }

  if (!preamble_found)
    QDPLL_ABORT_APP (1, "preamble missing!\n");
}

/* -------------------- END: PARSING -------------------- */



static void
check_options (QDPLLApp * app)
{
  if (app->options.print_deps && !app->options.deps_only)
    print_abort_err (app, "must not use option '%s' without option'%s'!\n\n",
                     "--print-deps", "--deps-only");

  if (app->options.dump_dep_graph && !app->options.deps_only)
    print_abort_err (app, "must not use option '%s' without option'%s'!\n\n",
                     "--dump-dep-graph", "--deps-only");
}


static void
set_default_options (QDPLLApp * app)
{
  app->options.in_filename = 0;
  app->options.in = stdin;
  app->options.pretty_print = 0;
  app->options.deps_only = 0;
  app->options.print_deps = 0;
  app->options.dump_dep_graph = 0;
  app->options.print_usage = 0;
}


static int
isnumstr (char *str)
{
  /* Empty string is not considered as number-string. */
  if (!*str)
    return 0;
  char *p;
  for (p = str; *p; p++)
    {
      if (!isdigit (*p))
        return 0;
    }
  return 1;
}


static void
parse_cmd_line_options (QDPLLApp * app, QDPLL * qdpll, int argc, char **argv)
{
  char *result;
  int opt_cnt;
  for (opt_cnt = 1; opt_cnt < argc; opt_cnt++)
    {
      char *opt_str = argv[opt_cnt];

      if (!strcmp (opt_str, "-h") || !strcmp (opt_str, "--help"))
        {
          app->options.print_usage = 1;
        }
      else if (!strcmp(opt_str, "-seed"))
      {
          opt_cnt++;
          app->options.seed = atoi(argv[opt_cnt]);
      }
      else if (!strcmp (opt_str, "--version"))
        {
          app->options.print_version = 1;
        }
      else if (!strcmp (opt_str, "--pretty-print"))
        {
          app->options.pretty_print = 1;
        }
      else if (!strcmp (opt_str, "--qdo"))
        {
          app->options.qdimacs_output = 1;
        }
      else if (!strcmp (opt_str, "--deps-only"))
        {
          app->options.deps_only = 1;
        }
      else if (!strcmp (opt_str, "--print-deps"))
        {
          app->options.print_deps = 1;
        }
      else if (!strcmp (opt_str, "--dump-dep-graph"))
        {
          app->options.dump_dep_graph = 1;
        }
      else if (!strcmp (opt_str, "--qdag-print-deps-by-search"))
        {
          app->options.print_deps = 1;
          if ((result = qdpll_configure (qdpll, opt_str)))
            print_abort_err (app, "%s!\n\n", result);
        }
      else if (!strcmp (opt_str, "--trace")
               || !strcmp (opt_str, "--trace=qrp"))
        {
          app->options.trace = TRACE_QRP;
          qdpll_configure (qdpll, opt_str);
        }
      else if (!strcmp (opt_str, "--trace=bqrp"))
        {
          app->options.trace = TRACE_BQRP;
          qdpll_configure (qdpll, opt_str);
        }
      else if (isnumstr (opt_str))
        {
          app->options.max_time = atoi (opt_str);
          if (app->options.max_time == 0)
            {
              result = "Expecting non-zero value for max-time";
              print_abort_err (app, "%s!\n\n", result);
            }
        }
      else if (!strncmp (opt_str, "-", 1) || !strncmp (opt_str, "--", 2))
        {
          /* Handle QDPLL-options. */
          if (!strcmp (opt_str, "-v"))
            {
              app->options.verbosity++;
            }
          if ((result = qdpll_configure (qdpll, opt_str)))
            print_abort_err (app, "%s!\n\n", result);
        }
      else if (!app->options.in_filename)
        {
          app->options.in_filename = opt_str;
          /* Check input file. */
          DIR *dir;
          if ((dir = opendir (app->options.in_filename)) != NULL)
            {
              closedir (dir);
              print_abort_err (app, "input file '%s' is a directory!\n\n",
                               app->options.in_filename);
            }
          FILE *input_file = fopen (app->options.in_filename, "r");
          if (!input_file)
            {
              print_abort_err (app, "could not open input file '%s'!\n\n",
                               app->options.in_filename);
            }
          else
            app->options.in = input_file;
        }
      else
        {
          print_abort_err (app, "unknown option '%s'!\n\n", opt_str);
        }
    }
}


static void
print_deps (QDPLLApp * app, QDPLL * qdpll)
{
  VarID i, max;
  for (i = 1, max = qdpll_get_max_declared_var_id (qdpll); i <= max; i++)
    {
      fprintf (stdout, "%u: ", i);
      qdpll_print_deps (qdpll, i);
      fprintf (stdout, "\n");
    }
  fprintf (stdout, "\n");
}


static void
sig_handler (int sig)
{
  fprintf (stderr, "\n\n SIG RECEIVED\n\n");
#if (COMPUTE_STATS || COMPUTE_TIMES)
  qdpll_print_stats (qdpll);
#endif
  signal (sig, SIG_DFL);
  raise (sig);
}


static void
sigalrm_handler (int sig)
{
  fprintf (stderr, "\n\n SIGALRM RECEIVED\n\n");
#if (COMPUTE_STATS || COMPUTE_TIMES)
  qdpll_print_stats (qdpll);
#endif
  signal (sig, SIG_DFL);
  raise (sig);
}


static void
set_signal_handlers (QDPLLApp * app)
{
  signal (SIGINT, sig_handler);
  signal (SIGTERM, sig_handler);
  signal (SIGALRM, sigalrm_handler);
  signal (SIGXCPU, sigalrm_handler);
}


static void
print_result_message (QDPLLApp * app, QDPLL * qdpll, QDPLLResult result, FILE *stream)
{
  /* Print result message; this may not always be desired and depends on the 
     current usage of the solver library. */
  if (!app->options.qdimacs_output)
    {
      
      if (result == QDPLL_RESULT_SAT)  exit(10);
      else if (result == QDPLL_RESULT_UNSAT)  exit(20);
      else  exit(0);
      
      fprintf(stream, "%s ", app->options.in_filename);
      /* Print own output format. */
      if (result == QDPLL_RESULT_SAT)
        fprintf (stream, "SAT");
      else if (result == QDPLL_RESULT_UNSAT)
        fprintf (stream, "UNSAT");
      else if (!app->options.pretty_print && !app->options.deps_only)
        {
          assert (result == QDPLL_RESULT_UNKNOWN);
          fprintf (stream, "UNKNOWN");
        }

      printf(" %.6lf\n", qdpll->run_time);
      /*
      printf(" %.6lf %.0lf %.6lf %.6lf %.0lf %.6lf %.6lf %.2lf\n", qdpll->run_time, 
          qdpll->num_lclause+0.001, qdpll->len_lclause/qdpll->num_lclause, qdpll->lbd_lclause / qdpll->num_lclause,
          qdpll->num_lcube + 0.001, qdpll->len_lcube / qdpll->num_lcube, qdpll->lbd_lcube / qdpll->num_lcube,
          qdpll->num_discarded_clause_cube);
      */
      /*
      printf(" %.6lf %d %d %d %d %d", qdpll->run_time,qdpll->num_false, qdpll->num_true, qdpll->num_undef,qdpll->state.clause_resizes, qdpll->state.cube_resizes);
      //printf(" %d %d %d %d %d %d\n", qdpll->glue1, qdpll->glue2, qdpll->glue3, qdpll->glue4, qdpll->glue5, qdpll->glue6);
      //printf("  qdpll->avg_lclause =%2lf    qdpll->num_lclause = %d\n", qdpll->avg_lclause, qdpll->num_lclause);
      qdpll->avg_lclause /= (double)qdpll->num_lclause;
      qdpll->avg_lcube /= (double)qdpll->num_lcube;
      printf(" %.2lf %.2lf -1 ",qdpll->avg_lclause, qdpll->avg_lcube);
      printf(" %d %d %d", qdpll->state.num_restarts, qdpll->state.num_inner_restarts, qdpll->num_backtrack_zero);
      printf(" %d %d %d %d %.2lf %d %d %.2lf %d %d\n", qdpll->all_call_ls_num, qdpll->ls_find_true, qdpll->ls_find_false, qdpll->state.cnt_state_undetermined_after_qbcp, qdpll->time_of_call_ls, qdpll->state.lclauses_size, qdpll->state.lcubes_size, qdpll->sort_time, qdpll->delete_num, qdpll->use_clause_cube_delete_after_restart);


      
      FILE* file = fopen("qbf_result.txt", "a");  // 打开文件用于写入

      // 方法1: fprintf
      fprintf(file, "%s ", qdpll->in_filename);
      if (result == QDPLL_RESULT_SAT)
          fprintf(file, "SAT ");
      else if (result == QDPLL_RESULT_UNSAT)
          fprintf(file, "UNSAT ");
      else if (!app->options.pretty_print && !app->options.deps_only)
      {
          assert(result == QDPLL_RESULT_UNKNOWN);
          fprintf(file, "UNKNOWN ");
      }
      fprintf(file, "%d %.6lf %d %d %d %d %d",qdpll->seed, qdpll->run_time, qdpll->num_false, qdpll->num_true, qdpll->num_undef, qdpll->state.clause_resizes, qdpll->state.cube_resizes);
      //fprintf(file, " %d %d %d %d %d %d\n", qdpll->glue1, qdpll->glue2, qdpll->glue3, qdpll->glue4, qdpll->glue5, qdpll->glue6);
      //fprintf(file, " %.2lf %.2lf -1 ", qdpll->avg_lclause, qdpll->avg_lcube);
      fprintf(file, " %d %d %d", qdpll->state.num_restarts, qdpll->state.num_inner_restarts, qdpll->num_backtrack_zero);
      fprintf(file, " %d %d %d %d %.2lf %d %d %.2lf %d %d\n", qdpll->all_call_ls_num, qdpll->ls_find_true, qdpll->ls_find_false, qdpll->state.cnt_state_undetermined_after_qbcp, qdpll->time_of_call_ls, qdpll->state.lclauses_size, qdpll->state.lcubes_size, qdpll->sort_time, qdpll->delete_num, qdpll->use_clause_cube_delete_after_restart);

      fclose(file);  // 关闭文件
      */
      
    }
  else
    {
      /* Print qdimacs-compliant output format. */
      qdpll_print_qdimacs_output (qdpll);
    }
}


static void
print_usage ()
{
  //fprintf (stdout, USAGE1);
  //fprintf (stdout, USAGE2);
}


static void
print_version ()
{
  //fprintf (stderr, VERSION);
}


static void
cleanup (QDPLL * qdpll, QDPLLApp * app)
{
  qdpll_delete (qdpll);

  if (app->options.in_filename)
    fclose (app->options.in);
}


//-------start------
#define PREPROC_TIMEOUT "300"

/* 运行 shell 命令，返回退出码，-1 表示调用失败 */
static int run_command(const char* cmd) {
    int status = system(cmd);
    if (status == -1) {
        perror("system");
        return -1;
    }
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
}

/* 执行 HQSPre，输入文件 in_file，输出写入 out_file */
static int run_hqspre(const char* in_file, const char* out_file) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "timeout %s ./hqspre --pipe %s > %s 2>/dev/null",
        PREPROC_TIMEOUT, in_file, out_file);
    //fprintf(stderr, "[preprocessor] Starting HQSPre...\n");
    int ret = run_command(cmd);
    /*
    if (ret == 0) {
        fprintf(stderr, "[preprocessor] HQSPre finished.\n");
    }
    else {
        fprintf(stderr, "[preprocessor] HQSPre timed out or failed.\n");
    }*/
    return ret;
}

/* 执行 Bloqqer，输入文件 in_file，输出写入 out_file。
   with_qdo 非 0 时启用证书输出到 cert_file */
static int run_bloqqer(const char* in_file, const char* out_file,
    int with_qdo, const char* cert_file) {
    char cmd[1024];
    if (with_qdo) {
        snprintf(cmd, sizeof(cmd),
            "timeout %s ./bloqqer-qdo --partial-assignment=1 %s > %s 2>%s",
            PREPROC_TIMEOUT, in_file, out_file, cert_file);
    }
    else {
        snprintf(cmd, sizeof(cmd),
            "timeout %s ./bloqqer --keep=0 %s > %s 2>/dev/null",
            PREPROC_TIMEOUT, in_file, out_file);
    }
    //fprintf(stderr, "[preprocessor] Starting Bloqqer%s...\n", with_qdo ? " (qdo)" : "");
    int ret = run_command(cmd);
    /*
    if (ret == 0) {
        fprintf(stderr, "[preprocessor] Bloqqer%s finished.\n",
            with_qdo ? " (qdo)" : "");
    }
    else {
        fprintf(stderr, "[preprocessor] Bloqqer%s timed out or failed.\n",
            with_qdo ? " (qdo)" : "");
    }*/
    return ret;
}

double get_wtime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1e-6;
}

/* 预处理主函数：
   - 输入原始文件路径 input_file
   - 输出最终使用的临时文件路径，调用者需要释放该字符串
   - 返回值：0 成功，-1 失败
   - 不处理证书（depqBF 无此需求）
*/
static int apply_preprocessing(const char* input_file, char** output_file) {
    char tmp_template[] = "/tmp/depqbf_preproc_XXXXXX";
    char* hqspre_out = NULL;
    char* bloqqer_out = NULL;
    int fd;

    //double tt1 = get_wtime();

    /* 创建 HQSPre 输出临时文件 */
    fd = mkstemp(tmp_template);
    if (fd == -1) {
        perror("mkstemp");
        return -1;
    }
    close(fd);
    hqspre_out = strdup(tmp_template);
    if (!hqspre_out) return -1;

    int hqspre_ok = (run_hqspre(input_file, hqspre_out) == 0);

    /* 决定 Bloqqer 的输入 */
    const char* bloqqer_input = hqspre_ok ? hqspre_out : input_file;

    /* 创建 Bloqqer 输出临时文件 */
    strcpy(tmp_template, "/tmp/depqbf_preproc_XXXXXX");
    fd = mkstemp(tmp_template);
    if (fd == -1) {
        perror("mkstemp");
        free(hqspre_out);
        return -1;
    }
    close(fd);
    bloqqer_out = strdup(tmp_template);
    if (!bloqqer_out) {
        free(hqspre_out);
        return -1;
    }

    int bloqqer_ok = (run_bloqqer(bloqqer_input, bloqqer_out, 0, NULL) == 0);

    if (bloqqer_ok) {
        *output_file = bloqqer_out;
        free(hqspre_out);
    }
    else {
        *output_file = strdup(bloqqer_input);
        free(bloqqer_out);
        free(hqspre_out);
        if (!*output_file) return -1;
    }

    //double tt2 = get_wtime();

    //printf("time = %.6lf\n", (tt2 - tt1) );

    return 0;
}
//-------end-----


static int apply_preprocessing_bloqqer_hqspre(const char* input_file, char** output_file) {
    char tmp_template[] = "/tmp/depqbf_preproc_XXXXXX";
    char* bloqqer_out = NULL;
    char* hqspre_out = NULL;
    int fd;

    /* 创建 Bloqqer 输出临时文件 */
    fd = mkstemp(tmp_template);
    if (fd == -1) {
        perror("mkstemp");
        return -1;
    }
    close(fd);
    bloqqer_out = strdup(tmp_template);
    if (!bloqqer_out) return -1;

    int bloqqer_ok = (run_bloqqer(input_file, bloqqer_out, 0, NULL) == 0);

    /* 决定 HQSPre 的输入 */
    const char* hqspre_input = bloqqer_ok ? bloqqer_out : input_file;

    /* 创建 HQSPre 输出临时文件 */
    strcpy(tmp_template, "/tmp/depqbf_preproc_XXXXXX");
    fd = mkstemp(tmp_template);
    if (fd == -1) {
        perror("mkstemp");
        free(bloqqer_out);
        return -1;
    }
    close(fd);
    hqspre_out = strdup(tmp_template);
    if (!hqspre_out) {
        free(bloqqer_out);
        return -1;
    }

    int hqspre_ok = (run_hqspre(hqspre_input, hqspre_out) == 0);

    if (hqspre_ok) {
        *output_file = hqspre_out;
        free(bloqqer_out);
    }
    else {
        *output_file = strdup(hqspre_input);
        free(hqspre_out);
        free(bloqqer_out);
        if (!*output_file) return -1;
    }

    return 0;
}


static int apply_preprocessing_only_HQSPRE(const char* input_file, char** output_file) {
    char tmp_template[] = "/tmp/depqbf_preproc_XXXXXX";
    int fd;

    /* 创建 HQSPre 输出临时文件 */
    fd = mkstemp(tmp_template);
    if (fd == -1) {
        perror("mkstemp");
        return -1;
    }
    close(fd);
    char* hqspre_out = strdup(tmp_template);
    if (!hqspre_out) return -1;

    int hqspre_ok = (run_hqspre(input_file, hqspre_out) == 0);

    if (hqspre_ok) {
        *output_file = hqspre_out;
    }
    else {
        /* HQSPre 超时或失败，使用原始输入 */
        free(hqspre_out);
        *output_file = strdup(input_file);
        if (!*output_file) return -1;
    }
    return 0;
}

static int apply_preprocessing_BLOQQER(const char* input_file, char** output_file) {
    char tmp_template[] = "/tmp/depqbf_preproc_XXXXXX";
    int fd;

    /* 创建 Bloqqer 输出临时文件 */
    fd = mkstemp(tmp_template);
    if (fd == -1) {
        perror("mkstemp");
        return -1;
    }
    close(fd);
    char* bloqqer_out = strdup(tmp_template);
    if (!bloqqer_out) return -1;

    int bloqqer_ok = (run_bloqqer(input_file, bloqqer_out, 0, NULL) == 0);

    if (bloqqer_ok) {
        *output_file = bloqqer_out;
    }
    else {
        /* Bloqqer 超时或失败，使用原始输入 */
        free(bloqqer_out);
        *output_file = strdup(input_file);
        if (!*output_file) return -1;
    }
    return 0;
}

/*
int qdpll_main (int argc, char **argv)
{
    double strat1 = get_wtime();
  QDPLLResult result = QDPLL_RESULT_UNKNOWN;
  QDPLLApp app;
  memset (&app, 0, sizeof (QDPLLApp));
  set_default_options (&app);

  qdpll = qdpll_create ();

  parse_cmd_line_options (&app, qdpll, argc, argv);
  check_options (&app);
  set_signal_handlers (&app);

  qdpll->seed = app.options.seed;
  qdpll->in_filename = app.options.in_filename;
  qdpll->start = clock();

  

  if (app.options.print_usage)
    {
      print_usage ();
      cleanup (qdpll, &app);
      return result;
    }
  if (app.options.print_version)
    {
      print_version ();
      cleanup (qdpll, &app);
      return result;
    }

  if (app.options.max_time)
    alarm (app.options.max_time);

  
  //-----------start-------
  // ========== 预处理阶段 ========== 
  char* preproc_file = NULL;
  if (app.options.in_filename != NULL) {
      //if (apply_preprocessing(app.options.in_filename, &preproc_file) == 0) {
      //if (apply_preprocessing_only_HQSPRE(app.options.in_filename, &preproc_file) == 0) {
      //if (apply_preprocessing_BLOQQER(app.options.in_filename, &preproc_file) == 0) {
      if (apply_preprocessing_bloqqer_hqspre(app.options.in_filename, &preproc_file) == 0) {
          // 关闭原始输入文件，并打开预处理后的文件 
          fclose(app.options.in);
          app.options.in = fopen(preproc_file, "r");
          if (app.options.in == NULL) {
              fprintf(stderr, "Error: cannot open preprocessed file\n");
              free(preproc_file);
              cleanup(qdpll, &app);
              return -1;
          }
          // 可选：保存临时文件路径以便清理，例如 app.preproc_tmp_file = preproc_file; 
          // 如果暂时不清理，可以直接 free(preproc_file); 但文件会保留在磁盘上 
      }
      else {
          fprintf(stderr, "Warning: preprocessing failed, continuing with original input\n");
      }
  }
  // ================================== 
  //-----------end---------
  
  parse (&app, qdpll, app.options.in, app.options.trace);
  

  if (app.options.pretty_print)
    {
      qdpll_gc (qdpll);
      qdpll_print (qdpll, stdout);
    }
  else if (app.options.deps_only)
    {
      qdpll_init_deps (qdpll);
      if (app.options.print_deps)
        print_deps (&app, qdpll);
      if (app.options.dump_dep_graph)
        qdpll_dump_dep_graph (qdpll);
    }
  else
    {
        result = qdpll_sat (qdpll);
#if (COMPUTE_STATS || COMPUTE_TIMES)
        qdpll_print_stats (qdpll);
#endif
    }

  if (app.options.trace == TRACE_QRP)
    fprintf (stdout, "r ");
  else if (app.options.trace == TRACE_BQRP)
    fprintf (stdout, "%cr ", 0);

  double end = get_wtime();
  qdpll->run_time = (end - strat1);// / CLOCKS_PER_SEC;

  print_result_message (&app, qdpll, result, stdout);

  cleanup (qdpll, &app);

  return result;
}*/


//gallery 2026 HybridQBF_CAQE
int qdpll_main(int argc, char** argv)
{
    double strat1 = clock();
    strat1 = get_wtime();
    QDPLLResult result = QDPLL_RESULT_UNKNOWN;
    QDPLLApp app;
    memset(&app, 0, sizeof(QDPLLApp));
    set_default_options(&app);

    qdpll = qdpll_create();

    parse_cmd_line_options(&app, qdpll, argc, argv);
    check_options(&app);
    set_signal_handlers(&app);

    qdpll->seed = app.options.seed;
    qdpll->in_filename = app.options.in_filename;
    qdpll->start = clock();

    if (app.options.print_usage)
    {
        print_usage();
        cleanup(qdpll, &app);
        return result;
    }
    if (app.options.print_version)
    {
        print_version();
        cleanup(qdpll, &app);
        return result;
    }

    if (app.options.max_time)
        alarm(app.options.max_time);

    // ---------- 预处理阶段 ----------
    char* preproc_file = NULL;
    int preproc_ok = 0;
    if (app.options.in_filename != NULL) {
        //if (apply_preprocessing(app.options.in_filename, &preproc_file) == 0) {
        //if (apply_preprocessing_only_HQSPRE(app.options.in_filename, &preproc_file) == 0) {
        //if (apply_preprocessing_bloqqer_hqspre(app.options.in_filename, &preproc_file) == 0) {
        if (apply_preprocessing_BLOQQER(app.options.in_filename, &preproc_file) == 0) {
            fclose(app.options.in);
            app.options.in = fopen(preproc_file, "r");
            if (app.options.in == NULL) {
                fprintf(stderr, "Error: cannot open preprocessed file\n");
                free(preproc_file);
                cleanup(qdpll, &app);
                return -1;
            }
            preproc_ok = 1;
        }
        else {
            fprintf(stderr, "Warning: preprocessing failed, continuing with original input\n");
            preproc_file = NULL;
        }
    }

    int skip_solving = 0;   // 标志：是否跳过后续的 parse 和求解

    // ---------- 规模检查：若变量数或子句数 < 100000 则调用 caqe ----------
    if (preproc_ok && preproc_file != NULL) {
        FILE* f = fopen(preproc_file, "r");
        if (f != NULL) {
            char line[4096];
            int num_var = -1, num_cla = -1;
            while (fgets(line, sizeof(line), f)) {
                if (line[0] == 'c' || line[0] == '\n') continue;
                if (strncmp(line, "p cnf", 5) == 0) {
                    sscanf(line, "p cnf %d %d", &num_var, &num_cla);
                    break;
                }
            }
            fclose(f);

            //printf("num_var=%d  num_cla=%d\n", num_var, num_cla);

            if ((num_var < 100000 || num_cla < 100000)) {
            //if ((num_cla < 100000)) {
            //if ( 1 ) {
                char cmd[1024];
                snprintf(cmd, sizeof(cmd), "./caqe %s 2>&1", preproc_file);
                FILE* pipe = popen(cmd, "r");
                if (pipe == NULL) {
                    fprintf(stderr, "Error: failed to execute caqe\n");
                    // 执行失败，继续原有求解（或视为未知）
                    result = QDPLL_RESULT_UNKNOWN;
                    skip_solving = 1;   // 直接跳到清理，不再 parse
                }
                else {
                    char out_line[4096];
                    int sat_found = 0, unsat_found = 0;
                    while (fgets(out_line, sizeof(out_line), pipe) != NULL) {
                        if (strstr(out_line, "c Satisfiable")) {
                            sat_found = 1;
                            break;
                        }
                        if (strstr(out_line, "c Unsatisfiable")) {
                            unsat_found = 1;
                            break;
                        }
                    }
                    pclose(pipe);

                    if (sat_found)
                        result = QDPLL_RESULT_SAT;
                    else if (unsat_found)
                        result = QDPLL_RESULT_UNSAT;
                    else
                        result = QDPLL_RESULT_UNKNOWN;

                    skip_solving = 1;   // caqe 已经给出结果，跳过 parse 和 qdpll_sat
                }
                //printf("caqe solevd\n");
            }
        }
        else {
            fprintf(stderr, "Warning: cannot reopen preprocessed file for size check\n");
        }
    }

    // ---------- 正常求解流程（未被 caqe 跳过时执行） ----------
    if (!skip_solving) {
        parse(&app, qdpll, app.options.in, app.options.trace);

        if (app.options.pretty_print)
        {
            qdpll_gc(qdpll);
            qdpll_print(qdpll, stdout);
        }
        else if (app.options.deps_only)
        {
            qdpll_init_deps(qdpll);
            if (app.options.print_deps)
                print_deps(&app, qdpll);
            if (app.options.dump_dep_graph)
                qdpll_dump_dep_graph(qdpll);
        }
        else
        {
            result = qdpll_sat(qdpll);
#if (COMPUTE_STATS || COMPUTE_TIMES)
            qdpll_print_stats(qdpll);
#endif
        }

        if (app.options.trace == TRACE_QRP)
            fprintf(stdout, "r ");
        else if (app.options.trace == TRACE_BQRP)
            fprintf(stdout, "%cr ", 0);

        //printf("hybridqbf solevd\n");
    }

    double end = clock();
    end = get_wtime();
    qdpll->run_time = (end - strat1);
    //qdpll->run_time = (end - strat1);

    // ---------- 统一清理和返回 ----------
    print_result_message(&app, qdpll, result, stdout);

    if (preproc_file != NULL)
        free(preproc_file);
    // app.options.in 会在 cleanup 中关闭，这里不需要手动关闭

    cleanup(qdpll, &app);
    return result;
}