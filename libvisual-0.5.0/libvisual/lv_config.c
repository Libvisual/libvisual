/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_config.c,v 1.10 2006/01/22 13:23:37 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <gettext.h>

#include "lvconfig.h"

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "lv_common.h"
#include "lv_mem.h"
#include "lv_config.h"

static int config_registry_dtor (VisObject *object);
static int config_registry_section_dtor (VisObject *object);

static int config_registry_dtor (VisObject *object)
{
	VisConfigRegistry *registry = VISUAL_CONFIG_REGISTRY (object);

	if (registry->filename != NULL)
		visual_mem_free (registry->filename);

	visual_collection_destroy (VISUAL_COLLECTION (&registry->sections));

	registry->filename = NULL;

	return VISUAL_OK;
}

static int config_registry_section_dtor (VisObject *object)
{
	VisConfigRegistrySection *rsection = VISUAL_CONFIG_REGISTRYSECTION (object);

	if (rsection->name != NULL)
		visual_mem_free (rsection->name);

	if (rsection->data != NULL)
		visual_mem_free (rsection->data);

	rsection->name = NULL;
	rsection->data = NULL;

	return VISUAL_OK;
}


/**
 * @defgroup VisConfig VisConfig
 * @{
 */

VisConfigRegistry *visual_config_registry_new ()
{
	VisConfigRegistry *registry;

	registry = visual_mem_new0 (VisConfigRegistry, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (registry), TRUE, config_registry_dtor);

	visual_collection_set_destroyer (VISUAL_COLLECTION (&registry->sections), visual_object_collection_destroyer);

	return registry;
}

VisConfigRegistry *visual_config_registry_open (const char *configfile)
{
	VisConfigRegistry *registry;
	VisConfigRegistrySection *rsection;
	int fd;
	int length;
	uint32_t datalength;
	char namebuf[64];
	char sectionname[128];

	visual_log_return_val_if_fail (configfile != NULL, NULL);

	registry = visual_config_registry_new ();

	registry->filename = strdup (configfile);

	/* Opening file */
	fd = open (configfile, O_RDONLY);

	if (fd < 0)
		goto out;

	length = lseek (fd, 0, SEEK_END);

	lseek (fd, 0, SEEK_SET);

	/* Empty config registry */
	if (length == 0)
		goto out;

	/* Checking version */
	if (read (fd, namebuf, 19) != 19)
		goto broken;

	/* Different config registry version, won't load */
	if (strncmp (namebuf, VISUAL_CONFIG_VERSION, 19) != 0) {
		visual_log (VISUAL_LOG_WARNING, _("The config registry file format is of an obsolete file format, won't load it"));

		goto out;
	}

	/* Loading sections */
	while (lseek (fd, 1, SEEK_CUR) > 0) {
		lseek (fd, -1, SEEK_CUR);

		if (read (fd, &datalength, sizeof (uint32_t)) != 4)
			goto broken;

		if (read (fd, sectionname, datalength > 128 ? 128 : datalength) != (datalength > 128 ? 128 : datalength))
			goto broken;

		sectionname[127] = '\0';

		rsection = visual_config_registry_section_new ();

		rsection->name = strdup (sectionname);

		lseek (fd, (datalength > 128 ? -128 : -datalength) + strlen (sectionname), SEEK_CUR);

		rsection->datalength = datalength;
		rsection->data = visual_mem_malloc0 (datalength);

		if (read (fd, rsection->data, datalength) != datalength) {
			visual_object_unref (VISUAL_OBJECT (rsection));

			goto broken;
		}

		visual_config_registry_add (registry, rsection);
	}

broken:
	visual_log (VISUAL_LOG_CRITICAL, _("Broken config registry, won't load"));

	/* Unload all sections, some might have been partially parsed, which can be dangerious, we don't
	 * want that. */
	visual_collection_destroy (VISUAL_COLLECTION (&registry->sections));

out:
	close (fd);

	return registry;
}

VisConfigRegistrySection *visual_config_registry_find (VisConfigRegistry *registry, const char *name)
{
	VisConfigRegistrySection *rsection;
	VisListEntry *le = NULL;

	visual_log_return_val_if_fail (registry != NULL, NULL);

	while ((rsection = visual_list_next (&registry->sections, &le)) != NULL) {
		if (strcmp (rsection->name, name) == 0)
			return rsection;
	}

	return NULL;
}

int visual_config_registry_remove (VisConfigRegistry *registry, const char *name)
{
	VisConfigRegistrySection *rsection;
	VisListEntry *le = NULL;

	visual_log_return_val_if_fail (registry != NULL, -VISUAL_ERROR_CONFIG_REGISTRY_NULL);

	while ((rsection = visual_list_next (&registry->sections, &le)) != NULL) {
		if (strcmp (rsection->name, name) == 0) {
			visual_list_delete (&registry->sections, &le);

			visual_object_unref (VISUAL_OBJECT (rsection));

			return VISUAL_OK;
		}
	}

	return VISUAL_OK;
}

int visual_config_registry_add (VisConfigRegistry *registry, VisConfigRegistrySection *rsection)
{
	visual_log_return_val_if_fail (registry != NULL, -VISUAL_ERROR_CONFIG_REGISTRY_NULL);
	visual_log_return_val_if_fail (rsection != NULL, -VISUAL_ERROR_CONFIG_REGISTRY_SECTION_NULL);

	visual_list_add (&registry->sections, rsection);

	return VISUAL_OK;
}

VisConfigRegistrySection *visual_config_registry_section_new ()
{
	VisConfigRegistrySection *rsection;

	rsection = visual_mem_new0 (VisConfigRegistrySection, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (rsection), TRUE, config_registry_section_dtor);

	return rsection;
}

VisConfigRegistrySection *visual_config_registry_section_open (const char *name, const char *configfile)
{
	VisConfigRegistry *registry;
	VisConfigRegistrySection *rsection;

	registry = visual_config_registry_open (configfile);

	visual_log_return_val_if_fail (registry != NULL, NULL);

	rsection = visual_config_registry_find (registry, name);

	return rsection;
}

int visual_config_registry_write_by_data (VisConfigRegistry *registry, const char *name, const char *data, int datalength)
{
	visual_log_return_val_if_fail (registry != NULL, -VISUAL_ERROR_CONFIG_REGISTRY_NULL);
	visual_log_return_val_if_fail (name != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (data != NULL, -VISUAL_ERROR_NULL);

	return VISUAL_OK;
}

int visual_config_registry_write (VisConfigRegistry *registry, VisConfigRegistrySection *rsection)
{
	visual_log_return_val_if_fail (registry != NULL, -VISUAL_ERROR_CONFIG_REGISTRY_NULL);
	visual_log_return_val_if_fail (rsection != NULL, -VISUAL_ERROR_CONFIG_REGISTRY_SECTION_NULL);

	visual_config_registry_write_by_data (registry, rsection->name, rsection->data, rsection->datalength);

	return VISUAL_OK;
}

int visual_config_registry_sync (VisConfigRegistry *registry)
{
	visual_log_return_val_if_fail (registry != NULL, -VISUAL_ERROR_CONFIG_REGISTRY_NULL);

	return VISUAL_OK;
}

/**
 * @}
 */

