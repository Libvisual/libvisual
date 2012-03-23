


#include <stdlib.h>
#include <getopt.h>
#include <libvisual/libvisual.h>


#define DEFAULT_ACTOR "lv_analyzer"
#define DEFAULT_INPUT "dummy"
#define DEFAULT_MORPH "slide"


static char actor_name[128];
static char input_name[128];
static char morph_name[128];




/** print commandline help */
static void _print_help(char *name)
{
	printf("libvisual commandline utility - %s\n"
	       "Usage: %s [options]\n\n"
	       "Valid options:\n"
	       "\t--help\t\t\t-h\t\tThis help text\n"
               "\t--plugin-help\t\t-p\t\tList of installed plugins + information\n"
               "\t--input <input>\t\t-i <input>\tUse this input plugin [%s]\n"
	       "\t--actor <actor>\t\t-a <actor>\tUse this actor plugin [%s]\n"               
               "\t--morph <morph>\t\t-m <morph>\tUse this morph plugin [%s]\n\n",               
	       "http://github.com/StarVisuals/libvisual", 
               name, input_name, actor_name, morph_name);
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
				/* save name for later */
				strncpy(input_name, optarg, sizeof(input_name)-1);
				break;
			}

                        /* --actor */
			case 'a':
			{
				/* save name for later */
				strncpy(actor_name, optarg, sizeof(actor_name)-1);
				break;
			}

                        /* --morph */
			case 'm':
			{
				/* save filename for later */
				strncpy(morph_name, optarg, sizeof(morph_name)-1);
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
        /* set defaults */
        strncpy(actor_name, DEFAULT_ACTOR, sizeof(actor_name)-1);
        strncpy(input_name, DEFAULT_INPUT, sizeof(input_name)-1);
        strncpy(morph_name, DEFAULT_MORPH, sizeof(morph_name)-1);
        
        /* parse commandline arguments */
        if(_parse_args(argc, argv) != EXIT_SUCCESS)
                return EXIT_FAILURE;
        
        /**
         * initialize libvisual once (this is meant to be called only once,
         * visual_init() after visual_quit() results in undefined state) 
         */
        visual_init (&argc, &argv);
        visual_log_set_verboseness(VISUAL_LOG_VERBOSENESS_HIGH);


        /* create new VisBin for video output */
        VisBin *bin;
        bin = visual_bin_new();
        visual_bin_set_supported_depth(bin, VISUAL_VIDEO_DEPTH_ALL);
        visual_bin_switch_set_style(bin, VISUAL_SWITCH_STYLE_MORPH);

        /* initialize actor plugin */
        VisActor *actor;
        if(!(actor = visual_actor_new(actor_name)))
        {
                fprintf(stderr, "Failed to load actor \"%s\"\n", actor_name);
                goto _m_exit;
        }
        
        /* initialize input plugin */
        VisInput *input;
        if(!(input = visual_input_new(input_name)))
        {
                fprintf(stderr, "Failed to load input \"%s\"\n", input_name);
                goto _m_exit;
        }

        
        /* handle depth? */
        int depthflag, depth;
        if((depthflag = visual_actor_get_supported_depth(actor)) 
                == VISUAL_VIDEO_DEPTH_GL)
        {
                visual_bin_set_depth(bin, VISUAL_VIDEO_DEPTH_GL);
        }
        else
        {
                depth = visual_video_depth_get_highest(depthflag);
                if((bin->depthflag & depth) > 0)
                {
                        visual_bin_set_depth(bin, depth);
                }
                else
                {
                        visual_bin_set_depth(bin, 
                                visual_video_depth_get_highest_nogl(
                                                        bin->depthflag));
                }
        }

        /* what is this for? */
        bin->depthforcedmain = bin->depth;

        //depth = visual_video_depth_get_highest_nogl(depthflag);
        VisVideoAttributeOptions *vidoptions;
        vidoptions = visual_actor_get_video_attribute_options (actor);




_m_exit:

        
        /* cleanup resources allocated by visual_init() */
        visual_quit ();
    
	//printf ("Total frames: %d, average fps: %f\n", display_fps_total (display), display_fps_average (display));
        return EXIT_SUCCESS;
}
