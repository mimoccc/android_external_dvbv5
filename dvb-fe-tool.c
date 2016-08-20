/*
 * Copyright (c) 2011-2012 - Mauro Carvalho Chehab
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 */

#include "libdvbv5/dvb-file.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>


#ifdef ENABLE_NLS
# define _(string) gettext(string)
# include "gettext.h"
# include <locale.h>
# include <langinfo.h>
# include <iconv.h>
#else
# define _(string) string
#endif


const char *argp_program_bug_address = "Mauro Carvalho Chehab <m.chehab@samsung.com>";

static struct option long_options[] =
{
    /* These options don’t set a flag.
    We distinguish them by their indices. */
    {"dvbv3",     no_argument,       0, '3'},
    {"adapter",  required_argument,       0, 'a'},
    {"acoustical", no_argument, 0, 'A'},
    {"set-delsys",  required_argument, 0, 'd'},
    {"frontend",  required_argument, 0, 'f'},
    {"get",     no_argument,       0, 'g'},
    {"femon",   no_argument, 0, 'm'},
    {"verbose", no_argument, 1, 'v'},
    {"help", no_argument, 0,'h'},
    {"version", no_argument, 0, 'V'},
    {"usage", no_argument, 0, 'u'},
    {0, 0, 0, 0}
};

static int adapter = 0;
static int frontend = 0;
static unsigned get = 0;
static char *set_params = NULL;
static int verbose = 0;
static int dvbv3 = 0;
static int delsys = 0;
static int femon = 0;
static int acoustical = 0;
static int timeout_flag = 0;

static void do_timeout(int x)
{
    (void)x;

    if (timeout_flag == 0) {
        timeout_flag = 1;
        alarm(2);
        signal(SIGALRM, do_timeout);
    } else {
        /* something has gone wrong ... exit */
        exit(1);
    }
}

#define PERROR(x...)                                                    \
    do {                                                            \
        fprintf(stderr, _("ERROR: "));                          \
        fprintf(stderr, x);                                     \
        fprintf(stderr, " (%s)\n", strerror(errno));        \
    } while (0)

#define HELP_MESSAGE "Usage: dvb-fe-tool [OPTION...] \n\
\n\
A DVB frontend tool using API version 5 \n\
\n\
On the options below, the arguments are: \n\
  ADAPTER      - the dvb adapter to control \n\
  FRONTEND     - the dvb frontend to control \n\
\n\
  -3, --dvbv3                Use DVBv3 only \n\
  -a, --adapter=ADAPTER      dvb adapter \n\
  -A, --acoustical           bips if signal quality is good. Also enables femon \n\
                             mode. Please notice that console bip should be \n\
                             enabled on your wm. \n\
  -d, --set-delsys=PARAMS    set delivery system \n\
  -f, --frontend=FRONTEND    dvb frontend \n\
  -g, --get                  get frontend \n\
  -m, --femon                monitors frontend stats on an streaming frontend \n\
  -v, --verbose              enables debug messages \n\
  -?, --help                 Give this help list \n\
      --usage                Give a short usage message \n\
  -V, --version              Print program version\n"


#define USAGE_MESSAGE "Usage: dvb-fe-tool [-3Agmv?V] [-a ADAPTER] [-d PARAMS] [-f FRONTEND] [--dvbv3]\n \
            [--adapter=ADAPTER] [--acoustical] [--set-delsys=PARAMS]\n \
            [--frontend=FRONTEND] [--get] [--femon] [--verbose] [--help]\n \
            [--usage] [--version]\n"

static int parse_opt(int argc, char* argv[])
{

    int c;
    int digit_optind = 0;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;

        c = getopt_long(argc, argv, "3a:Ad:f:gmvhV",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'a':
            adapter = atoi(optarg);
            break;
        case 'f':
            frontend = atoi(optarg);
            break;
        case 'd':
            delsys = dvb_parse_delsys(optarg);
            if (delsys < 0)
                return -1;
            break;
        case 'm':
            femon++;
            break;
        case 'A':
            femon++;
            acoustical++;
            break;
    #if 0
        case 's':
            set_params = optarg;
            break;
    #endif
        case 'g':
            get++;
            break;
        case '3':
            dvbv3++;
            break;
        case 'v':
            verbose ++;
            break;
        case 'h':
            fprintf(stdout,HELP_MESSAGE);
            //fprintf(state->out_stream, _("\nReport bugs to %s.\n"), argp_program_bug_address);
            exit(0);
        case 'u':
            fprintf(stdout,USAGE_MESSAGE);
            //fprintf(state->out_stream, _("\nReport bugs to %s.\n"), argp_program_bug_address);
            exit(0);
        case 'V':
            fprintf(stdout,"dvb-fe-tool for Android version 1.10.0\n");
            //fprintf (state->out_stream, "%s\n", argp_program_version);
            exit(0);
        default:
            return -1;
        }
    }
        return 0;
}

static int print_frontend_stats(FILE *fd,
                struct dvb_v5_fe_parms *parms)
{
    char buf[512], *p;
    int rc, i, len, show, n_status_lines = 0;

    rc = dvb_fe_get_stats(parms);
    if (rc) {
        PERROR(_("dvb_fe_get_stats failed"));
        return -1;
    }

    p = buf;
    len = sizeof(buf);
    dvb_fe_snprintf_stat(parms,  DTV_STATUS, NULL, 0, &p, &len, &show);

    for (i = 0; i < MAX_DTV_STATS; i++) {
        show = 1;

        dvb_fe_snprintf_stat(parms, DTV_QUALITY, _("Quality"),
                     i, &p, &len, &show);

        dvb_fe_snprintf_stat(parms, DTV_STAT_SIGNAL_STRENGTH, _("Signal"),
                     i, &p, &len, &show);

        dvb_fe_snprintf_stat(parms, DTV_STAT_CNR, _("C/N"),
                     i, &p, &len, &show);

        dvb_fe_snprintf_stat(parms, DTV_STAT_ERROR_BLOCK_COUNT, _("UCB"),
                     i,  &p, &len, &show);

        dvb_fe_snprintf_stat(parms, DTV_BER, _("postBER"),
                     i,  &p, &len, &show);

        dvb_fe_snprintf_stat(parms, DTV_PRE_BER, _("preBER"),
                     i,  &p, &len, &show);

        dvb_fe_snprintf_stat(parms, DTV_PER, _("PER"),
                     i,  &p, &len, &show);
        if (p != buf) {
            if (isatty(fileno(fd))) {
                enum dvb_quality qual;
                int color;

                qual = dvb_fe_retrieve_quality(parms, 0);

                switch (qual) {
                case DVB_QUAL_POOR:
                    color = 31;
                    break;
                case DVB_QUAL_OK:
                    color = 36;
                    break;
                case DVB_QUAL_GOOD:
                    color = 32;
                    break;
                case DVB_QUAL_UNKNOWN:
                default:
                    color = 0;
                    break;
                }
                fprintf(fd, "\033[%dm", color);
                /*
                 * It would be great to change the BELL
                 * tone depending on the quality. The legacy
                 * femon used to to that, but this doesn't
                 * work anymore with modern Linux distros.
                 *
                 * So, just print a bell if quality is good.
                 *
                 * The console audio should be enabled
                 * at the window manater for this to
                 * work.
                 */
                if (acoustical) {
                    if (qual == DVB_QUAL_GOOD)
                        fprintf(fd, "\a");
                }
            }

            if (n_status_lines)
                fprintf(fd, "\t%s\n", buf);
            else
                fprintf(fd, "%s\n", buf);

            n_status_lines++;

            p = buf;
            len = sizeof(buf);
        }
    }

    fflush(fd);

    return 0;
}

static void get_show_stats(struct dvb_v5_fe_parms *parms)
{
    int rc;

    signal(SIGTERM, do_timeout);
    signal(SIGINT, do_timeout);

    do {
        rc = dvb_fe_get_stats(parms);
        if (!rc)
            print_frontend_stats(stderr, parms);
        if (!timeout_flag)
            usleep(1000000);
    } while (!timeout_flag);
}

int main(int argc, char *argv[])
{
    struct dvb_v5_fe_parms *parms;
    int fe_flags = O_RDWR;

#ifdef ENABLE_NLS
    setlocale (LC_ALL, "");
    bindtextdomain (PACKAGE, LOCALEDIR);
    textdomain (PACKAGE);
#endif

    parse_opt(argc, argv);

    /*
     * If called without any option, be verbose, to print the
     * DVB frontend information.
     */
    if (!get && !delsys && !set_params && !femon)
        verbose++;

    if (!delsys && !set_params)
        fe_flags = O_RDONLY;

    parms = dvb_fe_open_flags(adapter, frontend, verbose, dvbv3,
                  NULL, fe_flags);
    if (!parms)
        return -1;

    if (delsys) {
        printf(_("Changing delivery system to: %s\n"),
            delivery_system_name[delsys]);
        dvb_set_sys(parms, delsys);
        goto ret;
    }

#if 0
    if (set_params)
        do_something();
#endif
    if (get) {
        dvb_fe_get_parms(parms);
        dvb_fe_prt_parms(parms);
    }

    if (femon)
        get_show_stats(parms);

ret:
    dvb_fe_close(parms);

    return 0;
}
