#include <libvisual/libvisual.h>

#include <rfb/rfb.h>

VisActor *actor;
VisVideo *video;
VisPalette *pal;
VisInput *input;

int main(int argc,char** argv)
{
	int i;
	uint8_t bytes[256*3];
	int width = 640;
	int height = 400;

	visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_HIGH);
	visual_init (&argc, &argv);

	if (argc > 1)
		actor = visual_actor_new (argv[1]);
	else
		actor = visual_actor_new ("oinksie");

	visual_actor_realize (actor);

	video = visual_video_new ();


	input = visual_input_new ("alsa");
	visual_input_realize (input);

	rfbScreenInfoPtr server=rfbGetScreen(&argc,argv,width,height,8,1,4);
#if 0
	server->serverFormat.trueColour=FALSE;
	server->colourMap.count=256;
	server->colourMap.is16=FALSE;
	for(i=0;i<256;i++) {
		bytes[i*3+0]=255-i; /* red */
		bytes[i*3+1]=0; /* green */
		bytes[i*3+2]=i; /* blue */
	}
	bytes[128*3+0]=0xff;
	bytes[128*3+1]=0;
	bytes[128*3+2]=0;
	server->colourMap.data.bytes=bytes;
#endif
	
	server->frameBuffer=(char*)malloc(width * height * 4);

	rfbInitServer(server);

	visual_actor_set_video (actor, video);
	visual_video_set_depth (video, VISUAL_VIDEO_DEPTH_32BIT);
	visual_video_set_dimension (video, width, height);
	visual_video_set_buffer (video, server->frameBuffer);

	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);
	
	while (1) {
		visual_input_run (input);
		visual_actor_run (actor, input->audio);
		rfbProcessEvents(server, 10);
		rfbDoCopyRect(server, 0, 0, width, height, 0, height);
		printf ("jaja\n");
	}

	return(0);
}
