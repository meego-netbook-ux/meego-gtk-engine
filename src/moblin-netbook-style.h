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

#ifndef _MOBLIN_NETBOOK_STYLE_H
#define _MOBLIN_NETBOOK_STYLE_H

#include <gtk/gtk.h>

#define DRAW_ARGS    GtkStyle       *style, \
  GdkWindow      *window, \
  GtkStateType state_type, \
  GtkShadowType shadow_type, \
  GdkRectangle   *area, \
  GtkWidget      *widget, \
  const gchar    *detail, \
  gint x, \
  gint y, \
  gint width, \
  gint height

#define SANITIZE_SIZE                                   \
  g_return_if_fail (width  >= -1);                      \
  g_return_if_fail (height >= -1);                      \
                                                        \
  if ((width == -1) && (height == -1)) \
    gdk_drawable_get_size (window, &width, &height);\
  else if (width == -1) \
    gdk_drawable_get_size (window, &width, NULL);\
  else if (height == -1) \
    gdk_drawable_get_size (window, NULL, &height);

#define MOBLIN_NETBOOK_TYPE_STYLE              moblin_netbook_style_get_type()
#define MOBLIN_NETBOOK_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST (( \
                                                                              object), \
                                                                            MOBLIN_NETBOOK_TYPE_STYLE, \
                                                                            MoblinNetbookStyle))
#define MOBLIN_NETBOOK_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST (( \
                                                                           klass), \
                                                                         MOBLIN_NETBOOK_TYPE_STYLE, \
                                                                         MoblinNetbookStyleClass))
#define MOBLIN_NETBOOK_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE (( \
                                                                              object), \
                                                                            MOBLIN_NETBOOK_TYPE_STYLE))
#define MOBLIN_NETBOOK_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE (( \
                                                                           klass), \
                                                                         MOBLIN_NETBOOK_TYPE_STYLE))
#define MOBLIN_NETBOOK_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS (( \
                                                                             obj), \
                                                                           MOBLIN_NETBOOK_TYPE_STYLE, \
                                                                           MoblinNetbookStyleClass))

typedef struct _MoblinNetbookStyle MoblinNetbookStyle;
typedef struct _MoblinNetbookStyleClass MoblinNetbookStyleClass;

struct _MoblinNetbookStyle
{
  GtkStyle parent_instance;

  gint radius;
  GdkColor border_color[5];
  gdouble shadow;

};

struct _MoblinNetbookStyleClass
{
  GtkStyleClass parent_class;
};

GType moblin_netbook_style_get_type (void);
void _moblin_netbook_style_register_type (GTypeModule *module);

#endif /* _MOBLIN_NETBOOK_STYLE_H_ */
