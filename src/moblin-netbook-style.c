/*
 * moblin-gtk-engine - A GTK+ theme engine for Moblin
 *
 * Copyright (c) 2008, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <gtk/gtk.h>
#include <stdio.h>

#include "moblin-netbook-style.h"

/*** Gtk Style Class **********************************************************/

GType moblin_netbook_type_style = 0;

static void moblin_netbook_style_class_init (SatoStyleClass *klass);

void
moblin_netbook_style_register_type (GTypeModule *module)
{
  static const GTypeInfo object_info =
  {
    sizeof (SatoStyleClass),
    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,
    (GClassInitFunc) moblin_netbook_style_class_init,
    NULL,           /* class_finalize */
    NULL,           /* class_data */
    sizeof (SatoStyle),
    0,              /* n_preallocs */
    (GInstanceInitFunc) NULL,
  };

  moblin_netbook_type_style = g_type_module_register_type (module,
						 GTK_TYPE_STYLE,
						 "SatoStyle",
						 &object_info, 0);
}

static void
moblin_netbook_style_class_init (SatoStyleClass *klass)
{
  GtkStyleClass *style_class = GTK_STYLE_CLASS (klass);
  moblin_netbook_draw_style_class_init (style_class);
}

/******************************************************************************/

/*** Gtk Style RC Class *******************************************************/

GType moblin_netbook_type_rc_style = 0;

static GtkStyle *moblin_netbook_rc_style_create_style (GtkRcStyle *rc_style);

static void
moblin_netbook_rc_style_class_init (SatoRcStyleClass *klass)
{
	GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS (klass);
	rc_style_class->create_style = moblin_netbook_rc_style_create_style;
}

void
moblin_netbook_rc_style_register_type (GTypeModule *module)
{
  static const GTypeInfo object_info =
  {
    sizeof (SatoRcStyleClass),
    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,
    (GClassInitFunc) moblin_netbook_rc_style_class_init,
    NULL,           /* class_finalize */
    NULL,           /* class_data */
    sizeof (SatoRcStyle),
    0,              /* n_preallocs */
    (GInstanceInitFunc) NULL,
  };

  moblin_netbook_type_rc_style = g_type_module_register_type (module,
                                                      GTK_TYPE_RC_STYLE,
                                                      "SatoRcStyle",
                                                      &object_info, 0);
}

static GtkStyle *
moblin_netbook_rc_style_create_style (GtkRcStyle *rc_style)
{
  return GTK_STYLE (g_object_new (SATO_TYPE_STYLE, NULL));
}

/******************************************************************************/
