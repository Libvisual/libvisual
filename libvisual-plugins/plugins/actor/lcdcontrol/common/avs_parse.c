/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: avs_parse.c,v 1.8 2006-09-19 19:05:47 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libvisual/libvisual.h>

#include "avs_parse.h"

/* Prototypes */
static int avs_tree_dtor (VisObject *object);
static int avs_element_dtor (VisObject *object);
static int avs_main_dtor (VisObject *object);
static int avs_render_superscope_dtor (VisObject *object);

static void show_options (AVSElement *element);

/* Object destructors */
static int avs_tree_dtor (VisObject *object)
{
        AVSTree *avstree = AVS_TREE (object);

        if (avstree->data != NULL)
                visual_mem_free (avstree->data);

        if (avstree->main != NULL)
                visual_object_unref (VISUAL_OBJECT (avstree->main));

        avstree->origfile = NULL;
        avstree->data = NULL;
        avstree->cur = NULL;
        avstree->main = NULL;

        return TRUE;
}

/* Every object that derives from AVSElement needs to pass on to this dtor when setting a custom dtor */
static int avs_element_dtor (VisObject *object)
{
        AVSElement *element = AVS_ELEMENT (object);

        if (element->serialize != NULL)
                visual_object_unref (VISUAL_OBJECT (element->serialize));

        if (element->pcont != NULL)
                visual_object_unref (VISUAL_OBJECT (element->pcont));

        element->serialize = NULL;
        element->pcont = NULL;

        return TRUE;
}

static int avs_container_dtor (VisObject *object)
{
        AVSContainer *container = AVS_CONTAINER (object);

        if (container->members != NULL)
                visual_object_unref (VISUAL_OBJECT (container->members));

        container->members = NULL;

        avs_element_dtor (object);

        return TRUE;
}



static void show_options (AVSElement *element)
{
        return;
        VisParamEntry *param;
        VisHashmapChainEntry *mentry;
        VisCollectionIter *iter;

        if (element == NULL)
                return;

        printf ("Element options of element type: %d\n", element->type);

        iter = visual_collection_get_iter (VISUAL_COLLECTION (&element->pcont->entries));

        while (visual_collection_iter_has_more (iter)) {
            printf("Bleh\n");
                mentry = visual_collection_iter_get_data (iter);
                param = mentry->data;

                switch (param->type) {
                        case VISUAL_PARAM_ENTRY_TYPE_NULL:
                                printf ("\t%s: Type NULL\n",
                                                visual_param_entry_get_name (param));

                                break;

                        case VISUAL_PARAM_ENTRY_TYPE_STRING:
                                printf ("\t%s: Type STRING: %s\n",
                                                visual_param_entry_get_name (param),
                                                visual_param_entry_get_string (param));

                                break;

                        case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
                                printf ("\t%s: Type INTEGER: %d\n",
                                                visual_param_entry_get_name (param),
                                                visual_param_entry_get_integer (param));

                                break;

                        case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
                                printf ("\t%s: Type FLOAT: %f\n",
                                                visual_param_entry_get_name (param),
                                                visual_param_entry_get_float (param));

                                break;

                        case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
                                printf ("\t%s: Type DOUBLE: %f\n",
                                                visual_param_entry_get_name (param),
                                                visual_param_entry_get_double (param));

                                break;

                        case VISUAL_PARAM_ENTRY_TYPE_COLOR:
                                printf ("\t%s: Type COLOR: %d %d %d\n",
                                                visual_param_entry_get_name (param),
                                                param->color.r, param->color.g, param->color.b);

                                break;

                        case VISUAL_PARAM_ENTRY_TYPE_PALETTE:
                                {
                                        int i;

                                        printf ("\t%s: Type PALETTE:\n",
                                                        visual_param_entry_get_name (param));

                                        for (i = 0; i < param->pal.ncolors; i++) {
                                                printf ("\t\tcolor[%d] %d %d %d\n", i,
                                                                param->pal.colors[i].r,
                                                                param->pal.colors[i].g,
                                                                param->pal.colors[i].b);
                                        }
                                }
                                break;

            default:
                break;
                }
        }

        printf ("\n");
}



/* AVS parser */
AVSTree *avs_tree_new_from_preset (char *filename)
{
        AVSTree *avstree;

        avstree = visual_mem_new0 (AVSTree, 1);

        /* Do the VisObject initialization */
        visual_object_initialize (VISUAL_OBJECT (avstree), TRUE, avs_tree_dtor);

        avs_parse_data (avstree, filename);

        return avstree;
}

int avs_check_version (AVSTree *avstree)
{
        if (avstree->datasize <= strlen ("Nullsoft AVS Preset 0.2")) {
                avstree->version = AVS_VERSION_UNKNOWN;

                printf ("VERSION PARSE: unknown\n");

                return -1;
        }

        if (!strncmp(avstree->data, "Nullsoft AVS Preset 0.2", strlen ("Nullsoft AVS Preset 0.2"))) {
                avstree->version = AVS_VERSION_2;

                printf ("VERSION PARSE: Nullsoft AVS Preset 0.2\n");

                avstree->cur = avstree->data + strlen ("Nullsoft AVS Preset 0.2");

                return 0;
        }

        avstree->version = AVS_VERSION_UNKNOWN;

        printf ("VERSION PARSE: unknown\n");

        return -1;
}

int avs_parse_tree (AVSTree *avstree, AVSContainer *curcontainer)
{
        AVSElement *element = NULL;
        char namedelem[30];
        int isnamed;
        char *next_section = NULL;
        int marker;

        if (curcontainer == NULL)
                return -1;

        while (avstree->cur < (avstree->data + avstree->datasize)) {

                element = NULL;

                marker = AVS_SERIALIZE_GET_INT (AVS_TREE_GET_CURRENT_POINTER (avstree));
                AVS_SERIALIZE_SKIP_INT (AVS_TREE_GET_CURRENT_POINTER (avstree));

                /* Named preset section */
                isnamed = FALSE;
                if (marker > 0xff) {
                        printf ("Marker > 0xff, named APE %x\n", marker);

                        strncpy (namedelem, AVS_TREE_GET_CURRENT_POINTER (avstree), 30);

                        AVS_SERIALIZE_SKIP_LENGTH (avstree->cur, 32);
                        isnamed = TRUE;
                        marker = AVS_ELEMENT_TYPE_APE;

                        printf("avstree->cur %s\n", avstree->cur);
                }
                // FIXME: In some files next_section doesn't point to the correct location.
                // Check the location of the section's size data. For now I've commented the
                // line at the bottom of this function where avstree->cur is assigned next_section.
                // This may not work for every file, but it works with the few I've checked.
                next_section = AVS_SERIALIZE_GET_NEXT_SECTION (AVS_TREE_GET_CURRENT_POINTER (avstree));
                avstree->cur_section_length = next_section - avstree->cur;

                AVS_SERIALIZE_SKIP_INT (AVS_TREE_GET_CURRENT_POINTER (avstree));

                printf (":: 0x%x marker! %d\n", marker, marker);

                /* FIXME: Use a table lookup here instead of giant function */
                switch (marker) {
                    case AVS_ELEMENT_TYPE_RENDER_SIMPLESPECTRUM:

                    break;

                    case AVS_ELEMENT_TYPE_MISC_COMMENT:
                            element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_MISC_COMMENT,
                                                "text", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                                                NULL);

                    break;

                    case AVS_ELEMENT_TYPE_MISC_RENDERSTATE:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_MISC_RENDERSTATE,
                            "newmode", AVS_SERIALIZE_ENTRY_TYPE_INT,
                            NULL);
                    break;
            
                    case AVS_ELEMENT_TYPE_MISC_BPM:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_MISC_BPM,
                            "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                            "arbitrary", AVS_SERIALIZE_ENTRY_TYPE_INT,
                            "skip", AVS_SERIALIZE_ENTRY_TYPE_INT,
                            "invert", AVS_SERIALIZE_ENTRY_TYPE_INT,
                            "arbVal", AVS_SERIALIZE_ENTRY_TYPE_INT,
                            "skipVal", AVS_SERIALIZE_ENTRY_TYPE_INT,
                            "skipfirst", AVS_SERIALIZE_ENTRY_TYPE_INT,
                            NULL);
                    break;

            
                case AVS_ELEMENT_TYPE_MISC_STACK:
                    element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_MISC_STACK,
                        "dir", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "which", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "blend", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "adjblend_val", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        NULL);
                break;
            
                case AVS_ELEMENT_TYPE_RENDER_RING:
                    element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_RENDER_RING,
                        "source and place", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "palette", AVS_SERIALIZE_ENTRY_TYPE_PALETTE,
                        "size", AVS_SERIALIZE_ENTRY_TYPE_BYTE,
                        "type", AVS_SERIALIZE_ENTRY_TYPE_BYTE,
                        NULL);

                break;

                case AVS_ELEMENT_TYPE_RENDER_SUPERSCOPE:
                    element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_RENDER_SUPERSCOPE,
                        "empty", AVS_SERIALIZE_ENTRY_TYPE_BYTE,
                        "point", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                        "frame", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                        "beat", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                        "init", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                        "channel source", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "palette", AVS_SERIALIZE_ENTRY_TYPE_PALETTE,
                        "draw type", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        NULL);

                break;

                case AVS_ELEMENT_TYPE_TRANS_DMOVE:
                    element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_DMOVE,
                        "empty", AVS_SERIALIZE_ENTRY_TYPE_BYTE,
                        "point", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                        "frame", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                        "beat", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                        "init", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                        "subpixel", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "rectcoords", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "xres", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "yres", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "blend", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "wrap", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "buffern", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "nomove", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        NULL);
                break;

                case AVS_ELEMENT_TYPE_RENDER_CLEARSCREEN:
                    element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_RENDER_CLEARSCREEN,
                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "color", AVS_SERIALIZE_ENTRY_TYPE_COLOR,
                        "clear type", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "clear 5050", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "first", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        NULL);

                break;

                case AVS_ELEMENT_TYPE_RENDER_BASSSPIN:
                    element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_RENDER_BASSSPIN,
                        "chan enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        "left color", AVS_SERIALIZE_ENTRY_TYPE_COLOR,
                        "right color", AVS_SERIALIZE_ENTRY_TYPE_COLOR,
                        "draw type", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        NULL);
                break;

                case AVS_ELEMENT_TYPE_TRANS_FASTBRIGHT:
                    element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_FASTBRIGHT,
                            "brightness type", AVS_SERIALIZE_ENTRY_TYPE_INT,
                            NULL);

                break;

                case AVS_ELEMENT_TYPE_TRANS_INVERT:
                    element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_INVERT,
                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                        NULL);

                break;

                case AVS_ELEMENT_TYPE_TRANS_BLUR:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_BLUR,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "roundmode",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_MOVEMENT:
                        element = AVS_ELEMENT (avs_parse_trans_movement (avstree));

                        break;

                case AVS_ELEMENT_TYPE_TRANS_ONETONE:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_ONETONE,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "color",  AVS_SERIALIZE_ENTRY_TYPE_COLOR,
                                        "blend",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blendavg",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "invert",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_GRAIN:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_GRAIN,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blend",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blendavg",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "smax",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "staticgrain",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_MIRROR:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_MIRROR,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "mode",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "onbeat",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "smooth",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "slower",  AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_SCATTER:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_SCATTER,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_FADEOUT:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_FADEOUT,
                                        "fadelen", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "color", AVS_SERIALIZE_ENTRY_TYPE_COLOR,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_MOSAIC:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_MOSAIC,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "quality", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "quality2", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blend", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blendavg", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "onbeat", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "frame duration", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_WATER:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_BRIGHTNESS,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_WATERBUMP:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_WATERBUMP,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "density", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "depth", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "random prop", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "drop position x", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "drop position y", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "drop radius", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "method", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_BRIGHTNESS:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_BRIGHTNESS,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blend", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blendavg", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "redp", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "greenp", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "bleup", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "dissoc", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "color", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "exclude", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "distance", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_BUMPMAP:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_BUMPMAP,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "onbeat", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "frame duration", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "depth", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "depth 2", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blend", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blendavg", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "code1", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                                        "code2", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                                        "code3", AVS_SERIALIZE_ENTRY_TYPE_STRING,
                                        "showlight", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "invert", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "oldstyle", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "buffern", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_TRANS_NFRAMECLEAR:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_NFRAMECLEAR,
                                        "color", AVS_SERIALIZE_ENTRY_TYPE_COLOR,
                                        "blend", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "n frame", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_RENDER_PARTICLE:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_RENDER_PARTICLE,
                                        "enabled", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "color", AVS_SERIALIZE_ENTRY_TYPE_COLOR,
                                        "maxdist", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "size", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "size2", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "blend", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_RENDER_OSCSTARS:
                        element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_RENDER_OSCSTARS,
                                        "effect", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "palette", AVS_SERIALIZE_ENTRY_TYPE_PALETTE,
                                        "size", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        "rotate", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                        NULL);

                        break;

                case AVS_ELEMENT_TYPE_APE:

                        printf ("APE NAME: %s\n", namedelem);

                        element = NULL;
                        if (strcmp (namedelem, "Multiplier") == 0) {
                                element = avs_parse_element_non_complex (avstree, AVS_ELEMENT_TYPE_TRANS_MULTIPLIER,
                                                "multiply", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                                NULL);
                        } else if (strcmp (namedelem, "Channel Shift") == 0) {
                                element = avs_parse_element_non_complex (avstree,
                                                AVS_ELEMENT_TYPE_TRANS_CHANNELSHIFT,
                                                "shift", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                                "onbeat", AVS_SERIALIZE_ENTRY_TYPE_INT,
                                                NULL);
                        } else {
                                printf ("Unhandled named entry: %s position: %x\n",
                                                namedelem, avstree->cur - avstree->data);
                        }

                        break;

                default:
                        printf ("Unhandled type: %x position: %x\n", *avstree->cur, avstree->cur - avstree->data);

                        return -1;

                        break;
                }

                if (element != NULL) {
                        visual_list_add (curcontainer->members, element);

                        show_options (element);
                }


                avstree->cur = next_section;
        }

        return 0;
}

int avs_element_connect_serialize_container (AVSElement *element, AVSSerializeContainer *scont)
{
        element->serialize = scont;

        return 0;
}

int avs_element_deserialize (AVSElement *element, AVSTree *avstree)
{
        AVS_TREE_GET_CURRENT_POINTER (avstree) =
                avs_serialize_container_deserialize (element->serialize, AVS_TREE_GET_CURRENT_POINTER (avstree));

        return 0;
}

int avs_element_deserialize_many_new_params (AVSElement *element, AVSTree *avstree, ...)
{
        AVSSerializeContainer *scont;

        VisParamContainer *pcont;
        va_list ap;
        char *pname;
        AVSSerializeEntryType stype;

        pcont = visual_param_container_new ();
        scont = avs_serialize_container_new ();

        va_start (ap, avstree);

        while ((pname = va_arg (ap, char *)) != NULL) {
                VisParamEntry *param = visual_param_entry_new (pname);
                AVSSerializeEntry *sentry;

                sentry = avs_serialize_entry_new (param);
                sentry->type = va_arg (ap, AVSSerializeEntryType);

                avs_serialize_container_add (scont, sentry);
                visual_param_container_add (pcont, param);
        }

        va_end (ap);

        element->pcont = pcont;
        avs_element_connect_serialize_container (element, scont);

        return avs_element_deserialize (element, avstree);
}

AVSContainer *avs_parse_main (AVSTree *avstree)
{
        AVSContainer *avsmain;
        AVSSerializeContainer *scont;
        VisParamContainer *pcont;

        static VisParamEntry params[] = {
                VISUAL_PARAM_LIST_ENTRY ("clearscreen"),
                VISUAL_PARAM_LIST_END
        };

        pcont = visual_param_container_new ();

        visual_param_container_add_many (pcont, params);

        avsmain = visual_mem_new0 (AVSContainer, 1);

        /* Do the VisObject initialization */
        visual_object_initialize (VISUAL_OBJECT (avsmain), TRUE, avs_container_dtor);

        AVS_ELEMENT (avsmain)->pcont = pcont;
        AVS_ELEMENT (avsmain)->type = AVS_ELEMENT_TYPE_MAIN;

        AVS_CONTAINER (avsmain)->members = visual_list_new (visual_object_collection_destroyer);

        scont = avs_serialize_container_new ();
        avs_serialize_container_add_byte (scont, visual_param_container_get (pcont, "clearscreen"));

        avs_element_connect_serialize_container (AVS_ELEMENT (avsmain), scont);

        avs_element_deserialize (AVS_ELEMENT (avsmain), avstree);

        avstree->main = avsmain;

        return avsmain;
}

AVSElement *avs_parse_trans_movement (AVSTree *avstree)
{
        AVSElement *movement;
        AVSSerializeContainer *scont;
        int len = avstree->cur_section_length;
        int pos=0;

        int effect;
        int rectangular;
        int effect_exp;
        int blend;
        int sourcemapped;
        int subpixel;
        int wrap;
        int REFFECT_MAX = 23;
        int effect_exp_ch;

        char buf[257];

        VisParamContainer *pcont;

        static VisParamEntry params[] = {
                VISUAL_PARAM_LIST_ENTRY_INTEGER ("effect", 0),
                VISUAL_PARAM_LIST_ENTRY_INTEGER ("rectangular", 0),
                VISUAL_PARAM_LIST_ENTRY_INTEGER ("blend", 0),
                VISUAL_PARAM_LIST_ENTRY_INTEGER ("sourcemapped", 0),
                VISUAL_PARAM_LIST_ENTRY_INTEGER ("subpixel", 0),
                VISUAL_PARAM_LIST_ENTRY_INTEGER ("wrap", 0),
                VISUAL_PARAM_LIST_ENTRY_STRING ("code", ""),
                VISUAL_PARAM_LIST_END
        };

        visual_mem_set (buf, 0, sizeof (buf));

        pcont = visual_param_container_new ();

        visual_param_container_add_many(pcont, params);

        movement = visual_mem_new0 (AVSElement, 1);

        /* Do the VisObject initialization */
        visual_object_initialize (VISUAL_OBJECT (movement), TRUE, avs_element_dtor);

        AVS_ELEMENT (movement)->pcont = pcont;
        AVS_ELEMENT (movement)->type = AVS_ELEMENT_TYPE_TRANS_MOVEMENT;

        /* Deserialize without using the container, too complex (borked) serialization */
        if (len - pos >= 4) {
                effect=AVS_SERIALIZE_GET_INT (avstree->cur);
                AVS_SERIALIZE_SKIP_INT (avstree->cur);
                pos += 4;
        }
        if (effect == 32767)
        {
                if (!memcmp(avstree->cur,"!rect ",6))
                {
                        AVS_SERIALIZE_SKIP_LENGTH (avstree->cur, 6);
                        rectangular=1;
                }
                if (AVS_SERIALIZE_GET_BYTE (avstree->cur) == 1)
                {
                        AVS_SERIALIZE_SKIP_BYTE (avstree->cur);
                        pos++;

                        int l=AVS_SERIALIZE_GET_INT(avstree->cur); AVS_SERIALIZE_SKIP_INT (avstree->cur); pos += 4;
                        if (l > 0 && len-pos >= l)
                        {
//                              effect_exp.resize(l);
                                memcpy(buf, avstree->cur, l);
                                buf[l] = 0;

//                              memcpy(effect_exp.get(), data+pos, l);
                                AVS_SERIALIZE_SKIP_LENGTH (avstree->cur, l);
                                pos+=l;
                        }
                        else
                        {
//                              effect_exp.resize(1);
//                              effect_exp.get()[0]=0;
                        }
                }
                else if (len-pos >= 256)
                {
                        int l = 256 - (rectangular ? 6 : 0);
                        memcpy(buf,avstree->cur,l);
                        buf[l]=0;
//                      effect_exp.assign(buf);
                        AVS_SERIALIZE_SKIP_LENGTH (avstree->cur, l);
                        pos+=l;
                        printf ("trans_movement buf %s\n", buf);
                }
        }
        if (len-pos >= 4) { blend=AVS_SERIALIZE_GET_INT(avstree->cur); AVS_SERIALIZE_SKIP_INT (avstree->cur);pos+=4; }
        if (len-pos >= 4) { sourcemapped=AVS_SERIALIZE_GET_INT(avstree->cur); AVS_SERIALIZE_SKIP_INT (avstree->cur);pos+=4; }
        if (len-pos >= 4) { rectangular=AVS_SERIALIZE_GET_INT(avstree->cur); AVS_SERIALIZE_SKIP_INT (avstree->cur);pos+=4; }
        if (len-pos >= 4) { subpixel=AVS_SERIALIZE_GET_INT(avstree->cur); AVS_SERIALIZE_SKIP_INT (avstree->cur);pos+=4; }
        else subpixel=0;
        if (len-pos >= 4) { wrap=AVS_SERIALIZE_GET_INT(avstree->cur); AVS_SERIALIZE_SKIP_INT (avstree->cur);pos+=4; }
        else wrap=0;
        if (!effect && len-pos >= 4)
        {
                effect=AVS_SERIALIZE_GET_INT(avstree->cur); AVS_SERIALIZE_SKIP_INT (avstree->cur);pos+=4;
        }

        if (effect != 32767 && (effect > REFFECT_MAX || effect < 0))
                effect=0;
        effect_exp_ch=1;

        visual_param_entry_set_integer (visual_param_container_get (pcont, "effect"), effect);
        visual_param_entry_set_integer (visual_param_container_get (pcont, "rectangular"), rectangular);
        visual_param_entry_set_integer (visual_param_container_get (pcont, "blend"), blend);
        visual_param_entry_set_integer (visual_param_container_get (pcont, "sourcemapped"), sourcemapped);
        visual_param_entry_set_integer (visual_param_container_get (pcont, "subpixel"), subpixel);
        visual_param_entry_set_integer (visual_param_container_get (pcont, "wrap"), wrap);
        visual_param_entry_set_string (visual_param_container_get (pcont, "code"), buf);

        printf ("effect: %d, rectangular: %d, blend %d, sourcemapped %d, subpixel %d, wrap %d\n",
                        effect, rectangular, blend, sourcemapped, subpixel, wrap);

        return movement;
}

AVSElement *avs_parse_element_non_complex (AVSTree *avstree, AVSElementType type, ...)
{
        AVSElement *element;
        AVSSerializeContainer *scont;
        VisParamContainer *pcont;
        va_list ap;
        char *pname;
        AVSSerializeEntryType stype;

        element = visual_mem_new0 (AVSElement, 1);

        /* Do the VisObject initialization */
        visual_object_initialize (VISUAL_OBJECT (element), TRUE, avs_element_dtor);

        element->type = type;

        pcont = visual_param_container_new ();
        scont = avs_serialize_container_new ();

        va_start (ap, type);

        while ((pname = va_arg (ap, char *)) != NULL) {
                VisParamEntry *param = visual_param_entry_new (pname);
                AVSSerializeEntry *sentry;

                sentry = avs_serialize_entry_new (param);
                sentry->type = va_arg (ap, AVSSerializeEntryType);

                avs_serialize_container_add (scont, sentry);
                visual_param_container_add (pcont, param);
        }

        va_end (ap);

        element->pcont = pcont;
        avs_element_connect_serialize_container (element, scont);

        avs_element_deserialize (element, avstree);

        return element;
}

int avs_parse_data (AVSTree *avstree, char *filename)
{
        int fd;
        int size;

        char *buf;

        if (avstree == NULL || filename == NULL)
                return -1;

        /* Open file, and read it into a memory buffer, a bit rough edged, but who cares! */
        fd = open (filename, O_RDONLY);

        if (fd < 0) {
                printf ("FILE NOT FOUND BLAH: %s\n", filename);

                exit (-1);
        }

        /* So we know how big this file is, nice for allocating our buffer */
        size = lseek (fd, 0, SEEK_END);
        lseek (fd, 0, SEEK_SET);

        /* Read the data into the buffer */
        avstree->data = malloc (size);
        read (fd, avstree->data , size);
        avstree->datasize = size;

        /* Close the file since it's no longer needed */
        close (fd);

        avstree->cur = avstree->data;
        avs_check_version (avstree);

        if (avstree->version != AVS_VERSION_2) {
                printf ("Not supported AVS version, or wrong filetype\n");

                return -1;
        }

        /* Skip over the main section marker, on to it's flags */
        AVS_SERIALIZE_SKIP_BYTE (AVS_TREE_GET_CURRENT_POINTER (avstree));

        avstree->main = avs_parse_main (avstree);

        avs_parse_tree (avstree, AVS_CONTAINER (avstree->main));

        return 0;
}

