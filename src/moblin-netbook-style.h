/*
 * moblin_netbook-style.h
 * This file is part of moblin_netbook-engine
 *
 * Copyright (C) 2006,2007 - OpenedHand Ltd
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#ifndef SATO_STYLE_H
#define SATO_STYLE_H

#include <gtk/gtk.h>
#include <gmodule.h>

#include "moblin-netbook-draw.h"

G_BEGIN_DECLS

/*** Gtk Style Class **********************************************************/

extern GType moblin_netbook_type_style;

#define SATO_TYPE_STYLE              moblin_netbook_type_style
#define SATO_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), POKY_TYPE_STYLE, SatoStyle))
#define SATO_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), POKY_TYPE_STYLE, SatoStyleClass))
#define SATO_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), POKY_TYPE_STYLE))
#define SATO_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), POKY_TYPE_STYLE))
#define SATO_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), POKY_TYPE_STYLE, SatoStyleClass))

typedef struct _SatoStyle SatoStyle;
typedef struct _SatoStyleClass SatoStyleClass;

struct _SatoStyle
{
  GtkStyle parent_instance;
};

struct _SatoStyleClass
{
  GtkStyleClass parent_class;
};

void moblin_netbook_style_register_type (GTypeModule *module);

/******************************************************************************/

/*** Gtk Style RC Class *******************************************************/

extern GType moblin_netbook_type_rc_style;

#define SATO_TYPE_RC_STYLE              moblin_netbook_type_rc_style
#define SATO_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), POKY_TYPE_RC_STYLE, SatoRcStyle))
#define SATO_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), POKY_TYPE_RC_STYLE, SatoRcStyleClass))
#define SATO_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), POKY_TYPE_RC_STYLE))
#define SATO_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), POKY_TYPE_RC_STYLE))
#define SATO_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), POKY_TYPE_RC_STYLE, SatoRcStyleClass))

typedef struct _SatoRcStyle SatoRcStyle;
typedef struct _SatoRcStyleClass SatoRcStyleClass;

struct _SatoRcStyle
{
  GtkRcStyle parent_instance;
};

struct _SatoRcStyleClass
{
  GtkRcStyleClass parent_class;
};

void moblin_netbook_rc_style_register_type (GTypeModule *engine);

/******************************************************************************/

G_END_DECLS

#endif
