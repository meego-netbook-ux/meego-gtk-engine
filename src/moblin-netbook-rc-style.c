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

#include "moblin-netbook-rc-style.h"
#include "moblin-netbook-style.h"


G_DEFINE_DYNAMIC_TYPE (MoblinNetbookRcStyle, moblin_netbook_rc_style, GTK_TYPE_RC_STYLE)


static GtkStyle *
moblin_netbook_rc_style_create_style (GtkRcStyle *rc_style)
{
  return GTK_STYLE (g_object_new (MOBLIN_NETBOOK_TYPE_STYLE, NULL));
}

static void
moblin_netbook_rc_style_class_init (MoblinNetbookRcStyleClass *klass)
{
  GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS (klass);
  rc_style_class->create_style = moblin_netbook_rc_style_create_style;
}

static void
moblin_netbook_rc_style_class_finalize (MoblinNetbookRcStyleClass *rc_style)
{
}

static void
moblin_netbook_rc_style_init (MoblinNetbookRcStyle *rc_style)
{
}

void
_moblin_netbook_rc_style_register_type (GTypeModule *module)
{
  moblin_netbook_rc_style_register_type (module);
}
