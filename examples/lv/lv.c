


#include <stdlib.h>
#include <getopt.h>
#include <libvisual/libvisual.h>





/** print commandline help */
static void _print_help(char *name)
{
	printf("libvisual commandline utility - %s\n"
	       "Usage: %s [options]\n\n"
	       "Valid options:\n"
	       "\t--help\t\t\t-h\t\tThis help text\n"
               "\t--plugin-help\t\t-p\t\tList of installed plugins + information\n"
               "\t--input <input>\t-i <input>\tUse this input plugin [dummy]\n"
	       "\t--actor <actor>\t\t-a <actor>\tUse this actor plugin [lv_analyer]\n"               
               "\t--morph <morph>\t\t-m <morph>\t\tUse this morph plugin [slide]\n\n",               
	       "http://github.com/StarVisuals/libvisual", name);
}


/** parse commandline arguments */
static int _parse_args(int argc, char *argv[])
{
	int index, argument;

	static struct option loptions[] =
	{
		{"help",        no_argument,       0, 'h'},
                {"plugin-help", no_argument,       0, 'p'},
		{"input",       required_argument, 0, 'i'},
		{"actor",       required_argument, 0, 'a'},
                {"morph",       required_argument, 0, 'm'},                
		{0,             0,                 0,  0 }
	};

	while((argument = getopt_long(argc, argv, "hpi:a:m:", loptions, &index)) >= 0)
	{
		
		switch(argument)
		{			
			/* --help */
			case 'h':
			{
				_print_help(argv[0]);
				return EXIT_FAILURE;
			}

                        /* --plugin-help */
                        case 'p':
                        {
                                //_print_plugin_help();
                                return EXIT_FAILURE;
                        }

			/* --input */
			case 'i':
			{
				/* save filename for later */
				//strncpy(_c.settingsfile, optarg, sizeof(_c.settingsfile));
				break;
			}

			/* invalid argument */
			case '?':
			{
				fprintf(stderr, "argument %d is invalid", index);
				_print_help(argv[0]);
				return EXIT_FAILURE;
			}

			/* unhandled arguments */
			default:
			{
				fprintf(stderr, "argument %d is invalid", index);
				break;
			}
		}
	}

		
	return EXIT_SUCCESS;
}


/******************************************************************************
 ******************************************************************************
 ******************************************************************************/
int main (int argc, char **argv)
{
        /**
         * initialize libvisual once (this is meant to be called only once,
         * visual_init() after visual_quit() results in undefined state) 
         */
        visual_init (&argc, &argv);
        visual_log_set_verboseness(VISUAL_LOG_VERBOSENESS_HIGH);



        /* cleanup resources allocated by visual_init() */
        visual_quit ();
    
	//printf ("Total frames: %d, average fps: %f\n", display_fps_total (display), display_fps_average (display));
        return EXIT_SUCCESS;
}
