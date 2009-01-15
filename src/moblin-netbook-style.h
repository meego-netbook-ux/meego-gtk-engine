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

#ifndef MOBLIN_NETBOOK_STYLE_H
#define MOBLIN_NETBOOK_STYLE_H

#include <gtk/gtk.h>
#include <gmodule.h>

#include "moblin-netbook-draw.h"

G_BEGIN_DECLS

/*** Gtk Style Class **********************************************************/

extern GType moblin_netbook_type_style;

#define MOBLIN_NETBOOK_TYPE_STYLE              moblin_netbook_type_style
#define MOBLIN_NETBOOK_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), MOBLIN_NETBOOK_TYPE_STYLE, MoblinNetbookStyle))
#define MOBLIN_NETBOOK_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOBLIN_NETBOOK_TYPE_STYLE, MoblinNetbookStyleClass))
#define MOBLIN_NETBOOK_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MOBLIN_NETBOOK_TYPE_STYLE))
#define MOBLIN_NETBOOK_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MOBLIN_NETBOOK_TYPE_STYLE))
#define MOBLIN_NETBOOK_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MOBLIN_NETBOOK_TYPE_STYLE, MoblinNetbookStyleClass))

typedef struct _MoblinNetbookStyle MoblinNetbookStyle;
typedef struct _MoblinNetbookStyleClass MoblinNetbookStyleClass;

struct _MoblinNetbookStyle
{
  GtkStyle parent_instance;
};

struct _MoblinNetbookStyleClass
{
  GtkStyleClass parent_class;
};

void moblin_netbook_style_register_type (GTypeModule *module);

/******************************************************************************/

/*** Gtk Style RC Class *******************************************************/

extern GType moblin_netbook_type_rc_style;

#define MOBLIN_NETBOOK_TYPE_RC_STYLE              moblin_netbook_type_rc_style
#define MOBLIN_NETBOOK_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), MOBLIN_NETBOOK_TYPE_RC_STYLE, MoblinNetbookRcStyle))
#define MOBLIN_NETBOOK_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOBLIN_NETBOOK_TYPE_RC_STYLE, MoblinNetbookRcStyleClass))
#define MOBLIN_NETBOOK_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MOBLIN_NETBOOK_TYPE_RC_STYLE))
#define MOBLIN_NETBOOK_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MOBLIN_NETBOOK_TYPE_RC_STYLE))
#define MOBLIN_NETBOOK_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MOBLIN_NETBOOK_TYPE_RC_STYLE, MoblinNetbookRcStyleClass))

typedef struct _MoblinNetbookRcStyle MoblinNetbookRcStyle;
typedef struct _MoblinNetbookRcStyleClass MoblinNetbookRcStyleClass;

struct _MoblinNetbookRcStyle
{
  GtkRcStyle parent_instance;
};

struct _MoblinNetbookRcStyleClass
{
  GtkRcStyleClass parent_class;
};

void moblin_netbook_rc_style_register_type (GTypeModule *engine);

/******************************************************************************/

G_END_DECLS

#endif
