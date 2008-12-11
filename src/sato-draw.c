/* vi: set expandtab sw=2 sts=2:
 * sato-draw.c
 * This file is part of sato-engine
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

#include "config.h"

#ifndef ENABLE_CAIRO

#include "sato-draw.h"
#include "sato-style.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#define DEBUG(func) // g_printf ("%s: detail = '%s'; state = %d; x:%d; y:%d; w:%d; h:%d;\n", func, detail, state_type, x, y, width, height);
#define DETAIL(foo) (detail && strcmp (foo, detail) == 0)

GtkStyleClass *parent_style_class;

typedef enum
{
  SATO_CORNER_TOP_LEFT,
  SATO_CORNER_TOP_RIGHT,
  SATO_CORNER_BOTTOM_LEFT,
  SATO_CORNER_BOTTOM_RIGHT
} SatoCorner;


/**
 * Prepare a new GC with the additional Sato style values
 */
static GdkGC*
sato_gc_new (GdkGC* old_gc, GdkDrawable *d)
{
  GdkGC *new_gc;
  new_gc = gdk_gc_new (d);
  gdk_gc_copy (new_gc, old_gc);
  gdk_gc_set_line_attributes (new_gc, 2, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
  return new_gc;
}

static void
sato_corner (GdkWindow *window, GdkGC *gc, gint x, gint y, SatoCorner corner)
{

  GdkColor aa;
  GdkGCValues gc_values;

  gdk_gc_get_values (gc, &gc_values);

  gdk_colormap_query_color (gdk_drawable_get_colormap (window),
			    gc_values.foreground.pixel,
			    &gc_values.foreground);

  /* Brighten the colour for fake AA. Need a better way to do this */
  aa.red = MIN (65535, gc_values.foreground.red * 1.8);
  aa.blue = MIN (65535, gc_values.foreground.blue * 1.8);
  aa.green = MIN (65535, gc_values.foreground.green * 1.8);

  switch (corner)
  {
    case SATO_CORNER_TOP_LEFT:
      gdk_draw_point (window, gc, x + 1, y + 1);
      gdk_draw_point (window, gc, x + 1, y + 2);
      gdk_draw_point (window, gc, x + 2, y + 1);

      gdk_draw_point (window, gc, x + 2, y + 2);
      gdk_gc_set_rgb_fg_color (gc, &aa);
      gdk_draw_point (window, gc, x + 2, y);

      gdk_draw_point (window, gc, x, y + 2);
      break;
    case SATO_CORNER_BOTTOM_LEFT:
      gdk_gc_set_rgb_fg_color (gc, &gc_values.foreground);
      gdk_draw_point (window, gc, x + 1, y - 3);
      gdk_draw_point (window, gc, x + 1, y  - 2);
      gdk_draw_point (window, gc, x + 2, y - 2);

      gdk_draw_point (window, gc, x + 2, y - 3);
      gdk_gc_set_rgb_fg_color (gc, &aa);
      gdk_draw_point (window, gc, x + 2, y - 1);

      gdk_draw_point (window, gc, x, y - 3);
      break;
    case SATO_CORNER_TOP_RIGHT:
      gdk_gc_set_rgb_fg_color (gc, &gc_values.foreground);
      gdk_draw_point (window, gc, x - 3, y + 1);
      gdk_draw_point (window, gc, x - 2, y + 1);
      gdk_draw_point (window, gc, x - 2, y + 2);

      gdk_draw_point (window, gc, x - 3, y + 2);
      gdk_gc_set_rgb_fg_color (gc, &aa);
      gdk_draw_point (window, gc, x - 3, y);

      gdk_draw_point (window, gc, x - 1, y + 2);
      break;
    case SATO_CORNER_BOTTOM_RIGHT:
      gdk_gc_set_rgb_fg_color (gc, &gc_values.foreground);
      gdk_draw_point (window, gc, x - 2, y - 3);
      gdk_draw_point (window, gc, x  - 2, y - 2);
      gdk_draw_point (window, gc, x - 3, y - 2);

      gdk_draw_point (window, gc, x - 3, y - 3);
      gdk_gc_set_rgb_fg_color (gc, &aa);
      gdk_draw_point (window, gc, x - 3, y - 1);

      gdk_draw_point (window, gc, x- 1, y - 3);
      break;
  }
  /* reset colour */
  gdk_gc_set_rgb_fg_color (gc, &gc_values.foreground);
}

static void
sato_rounded_rectangle (GdkWindow * window, GdkGC * gc, gint x, gint y,
			gint width, gint height)
{


  /** draw corners **/
  sato_corner (window, gc, x, y, SATO_CORNER_TOP_LEFT);
  sato_corner (window, gc, x, y + height, SATO_CORNER_BOTTOM_LEFT);
  sato_corner (window, gc, x + width, y, SATO_CORNER_TOP_RIGHT);
  sato_corner (window, gc, x+ width, y + height, SATO_CORNER_BOTTOM_RIGHT);

  /** draw lines **/

  /* top and bottom */
  y++;
  height++;
  gdk_draw_line (window, gc, x + 3, y, x + width - 3, y);
  gdk_draw_line (window, gc, x + 3, y + height - 3, x + width - 3, y + height - 3);
  y--;
  height--;

  /* left and right */
  x++;
  width++;
  gdk_draw_line (window, gc, x, y + 3, x, y + height - 3);
  gdk_draw_line (window, gc, x + width - 3, y + 3, x + width - 3, y + height - 3);


}

static void
sato_draw_box (DRAW_ARGS)
{
  gboolean has_focus;
  GdkGC *gc;
  GdkGC *border_color = style->fg_gc[state_type];

  DEBUG ("draw_box");

  if (DETAIL ("paned") || DETAIL ("vscrollbar") || DETAIL ("hscrollbar"))
    return;

  /* hack to remove PRELIGHT state */
  if (!DETAIL ("menuitem") && !DETAIL ("bar") && state_type == GTK_STATE_PRELIGHT)
  {
    if (widget && GTK_IS_TOGGLE_BUTTON (widget) && shadow_type == GTK_SHADOW_IN)
    {
      state_type = GTK_STATE_ACTIVE;
    }
    else
    {
      if (widget && GTK_IS_BUTTON (widget)
          && (gtk_button_get_relief (GTK_BUTTON (widget)) == GTK_RELIEF_NONE))
      {
        /* none relief buttons shouldn't draw anything for "normal" state */
        return;
      }
      else
      {
        state_type == GTK_STATE_NORMAL;
      }
    }
  }

  SANITIZE_SIZE;


  /*** treeview headers ***/
  if (widget && GTK_IS_TREE_VIEW (widget->parent))
  {
    return;
  }

  has_focus = (widget && GTK_WIDGET_HAS_FOCUS (widget));
  if (has_focus && state_type != GTK_STATE_ACTIVE)
      border_color = style->bg_gc[GTK_STATE_SELECTED];

  /*** spin buttons ***/
  if (DETAIL ("spinbutton_down") || DETAIL ("spinbutton_up"))
    return;

  if (DETAIL ("spinbutton"))
  {
    /* FIXME: for RTL */
    width += 10;
    x -= 10;
  }

  /*** combo boxes ***/
  if (DETAIL ("button") && widget && GTK_IS_COMBO_BOX_ENTRY (widget->parent))
  {
    GtkWidget *entry;

    entry = g_object_get_data (G_OBJECT (widget->parent), "sato-combo-entry");
    if (GTK_IS_ENTRY (entry))
    {
      gtk_widget_queue_draw_area (entry, entry->allocation.x, entry->allocation.y,
                                  entry->allocation.width,entry->allocation.height);

      has_focus = GTK_WIDGET_HAS_FOCUS (entry);
    }

    g_object_set_data (G_OBJECT (widget->parent), "sato-combo-button", widget);
    if (has_focus)
      border_color = style->bg_gc[GTK_STATE_SELECTED];
    else
      border_color = style->fg_gc[state_type];

    /* FIXME: RTL */
    width += 10;
    x -= 10;

  }


  gc = sato_gc_new (border_color, window);

  /* menu and toolbars get just a single line at the bottom of the widget */
  if (DETAIL ("menubar") || DETAIL ("toolbar"))
  {
    gdk_draw_line (window, gc, x, y + height - 1, x + width, y + height - 1);
  }
  else
  {
    gdk_draw_rectangle (window, style->bg_gc[state_type], TRUE, x + 1, y + 1, width - 2, height - 2);
    sato_rounded_rectangle (window, gc, x, y, width, height);
  }

  g_object_unref (gc);

}

static void
sato_draw_shadow (DRAW_ARGS)
{
 GdkGC *gc;

  DEBUG ("draw_shadow");

  if (shadow_type == GTK_SHADOW_NONE)
    return;

  SANITIZE_SIZE;

  /* FIXME: for RTL */
  if (widget && (GTK_IS_SPIN_BUTTON (widget) || GTK_IS_COMBO_BOX_ENTRY (widget->parent)))
      width += 10;

  if (widget && GTK_IS_COMBO_BOX_ENTRY (widget->parent))
  {
    GtkWidget *button;
    g_object_set_data (G_OBJECT (widget->parent), "sato-combo-entry", widget);

    button = g_object_get_data (G_OBJECT (widget->parent), "sato-combo-button");
    if (GTK_IS_BUTTON (button))
      gtk_widget_queue_draw_area (button, button->allocation.x, button->allocation.y,
                                  button->allocation.width,button->allocation.height);
  }

  /*** draw the border ***/

  /* draw a hilight shadow on focused widgets (i.e. entry widgets) */
  if (widget && GTK_WIDGET_HAS_FOCUS (widget))
    gc = sato_gc_new (style->bg_gc[GTK_STATE_SELECTED], window);
  else
    gc = sato_gc_new (style->fg_gc[state_type], window);

  sato_rounded_rectangle (window, gc, x, y, width, height);

  g_object_unref (gc);
}

static void
sato_draw_focus (GtkStyle *style, GdkWindow *window, GtkStateType state_type,
                 GdkRectangle *area, GtkWidget *widget, const gchar *detail,
                 gint x, gint y, gint width, gint height)
{
  DEBUG ("draw_focus");
  /* TODO */
}

static void
sato_draw_check (GtkStyle * style, GdkWindow * window,
		 GtkStateType state_type, GtkShadowType shadow_type,
		 GdkRectangle * area, GtkWidget * widget,
		 const gchar * detail, gint x, gint y, gint width,
		 gint height)
{
  GdkGC *gc;

  DEBUG ("draw_check");


  /* fill the background */
  if (widget && GTK_WIDGET_HAS_FOCUS (widget) && !GTK_IS_TREE_VIEW (widget))
    gdk_draw_rectangle (window, style->base_gc[GTK_STATE_SELECTED], TRUE, x, y, width, height);
  else
    gdk_draw_rectangle (window, style->base_gc[state_type], TRUE, x, y, width, height);

  /* draw the border */
  gc = sato_gc_new (style->fg_gc[state_type], window);
  gdk_draw_rectangle (window, gc, FALSE, x, y, width-1, height-1);

  /*** draw check mark ***/
  if (shadow_type == GTK_SHADOW_IN)
  {
    gdk_draw_rectangle (window, style->text_gc[state_type], TRUE, x + 3, y + 3, width - 7, height - 7);
  }

  g_object_unref (gc);
}

static void
sato_draw_option (GtkStyle * style, GdkWindow * window,
		  GtkStateType state_type, GtkShadowType shadow_type,
		  GdkRectangle * area, GtkWidget * widget,
		  const gchar * detail, gint x, gint y, gint width,
		  gint height)
{
  GdkGC *gc;

  DEBUG ("draw_option");

  /* fill the background */
  if (widget && GTK_WIDGET_HAS_FOCUS (widget) && !GTK_IS_TREE_VIEW (widget))
    gdk_draw_arc (window, style->base_gc[GTK_STATE_SELECTED], TRUE, x+1, y+1, width-2, height-2, 0, 360 * 64);
  else
    gdk_draw_arc (window, style->base_gc[state_type], TRUE, x+1, y+1, width-2, height-2, 0, 360 * 64);

  /* draw the border */
  gc = sato_gc_new (style->fg_gc[state_type], window);
  gdk_draw_arc (window, gc, FALSE, x, y, width - 1, height - 1, 0, 360 * 64);

  /*** draw check mark ***/
  if (shadow_type == GTK_SHADOW_IN)
  {
    gdk_draw_arc (window, style->text_gc[state_type], TRUE, x + 3, y + 3, width - 7, height - 7, 0, 360 * 64);
  }
  g_object_unref (gc);
}

static void
sato_draw_box_gap (GtkStyle * style, GdkWindow * window,
		   GtkStateType state_type, GtkShadowType shadow_type,
		   GdkRectangle * area, GtkWidget * widget, const gchar * detail,
		   gint x, gint y, gint width, gint height,
		   GtkPositionType gap_side, gint gap_x, gint gap_width)
{
  GdkGC *gc;
  GdkRectangle rect;

  if (shadow_type == GTK_SHADOW_NONE)
    return;

  gc = sato_gc_new (style->fg_gc [state_type], window);

  /* start off with a rectangle... */
  gdk_draw_rectangle (window, gc, FALSE, x + 1, y + 1, width - 2, height - 2);


  /* this stuff draws the corners as long as they don't overlap with a gap */

  if (!((gap_side == GTK_POS_TOP || gap_side == GTK_POS_LEFT)  && gap_x < 4))
  {
    gtk_style_apply_default_background (style, window, TRUE, state_type, area, x, y, 3, 3);
    sato_corner (window, gc, x, y, SATO_CORNER_TOP_LEFT);
  }

  if (!((gap_side == GTK_POS_TOP && gap_x > width - 4) || (gap_side == GTK_POS_RIGHT && gap_x < 4)))
  {
    gtk_style_apply_default_background (style, window, TRUE, state_type, area, x + width - 3, y, 3, 3);
    sato_corner (window, gc, x + width, y, SATO_CORNER_TOP_RIGHT);
  }

  if (!((gap_side == GTK_POS_BOTTOM && gap_x > width - 4) || (gap_side == GTK_POS_RIGHT && gap_x > height - 4)))  
  {
    gtk_style_apply_default_background (style, window, TRUE, state_type, area, x + width - 3, y + height - 3, 3, 3);
    sato_corner (window, gc, x + width, y + height, SATO_CORNER_BOTTOM_RIGHT);
  }

  if (!((gap_side == GTK_POS_BOTTOM && gap_x < 4) || (gap_side == GTK_POS_LEFT && gap_x > height - 4)))  
  {
    gtk_style_apply_default_background (style, window, TRUE, state_type, area, x, y + height - 3, 3, 3);
    sato_corner (window, gc, x, y + height, SATO_CORNER_BOTTOM_LEFT);
  }



  switch (gap_side)
  {
    case GTK_POS_TOP:

      rect.x = x + gap_x;
      rect.y = y;
      rect.width = gap_width;
      rect.height = 2;
      break;
    case GTK_POS_BOTTOM:
      rect.x = x + gap_x;
      rect.y = y + height - 2;
      rect.width = gap_width;
      rect.height = 2;
      break;
    case GTK_POS_LEFT:
      rect.x = x;
      rect.y = y + gap_x;
      rect.width = 2;
      rect.height = gap_width;
      break;
    case GTK_POS_RIGHT:
      rect.x = x + width - 2;
      rect.y = y + gap_x;
      rect.width = 2;
      rect.height = gap_width;
      break;
  }

  /* and finally blank out the gap */
  gtk_style_apply_default_background (style, window, TRUE, state_type, area,
				      rect.x, rect.y, rect.width,
				      rect.height);


  g_object_unref (gc);
}


static void
sato_draw_extension (GtkStyle * style, GdkWindow * window,
		     GtkStateType state_type, GtkShadowType shadow_type,
		     GdkRectangle * area, GtkWidget * widget,const gchar * detail,
		     gint x, gint y, gint width, gint height,
		     GtkPositionType gap_side)
{

  GdkGC *gc;
  gc = sato_gc_new (style->text_gc[state_type], window);

  switch (gap_side)
  {
    case GTK_POS_TOP:     /* bottom tab */
      gdk_draw_rectangle (window, style->bg_gc[state_type], TRUE, x + 1, y, width - 2, height - 2);
      sato_corner (window, gc, x, y + height, SATO_CORNER_BOTTOM_LEFT);
      sato_corner (window, gc, x + width, y + height, SATO_CORNER_BOTTOM_RIGHT);
      /* adjust for line width */
      x += 1; width -= 2; height -= 1;
      gdk_draw_line (window, gc, x, y - 2, x, y + height - 2);
      gdk_draw_line (window, gc, x + width, y - 2, x + width, y + height - 2);
      gdk_draw_line (window, gc, x + 2, y + height, x + width - 2, y + height);
      break;
    case GTK_POS_BOTTOM: 	/* top tab */
      gdk_draw_rectangle (window, style->bg_gc[state_type], TRUE, x + 2, y + 2, width - 4, height - 2);
      sato_corner (window, gc, x, y, SATO_CORNER_TOP_LEFT);
      sato_corner (window, gc, x + width, y, SATO_CORNER_TOP_RIGHT);
      /* adjust for line width */
      x += 1; y += 1; width -= 2; height += 1;
      gdk_draw_line (window, gc, x, y + 2, x, y + height);
      gdk_draw_line (window, gc, x + width, y + 2, x + width, y + height);
      gdk_draw_line (window, gc, x + 2, y, x + width - 2, y);
      break;
    case GTK_POS_LEFT:	/* right tab */
      gdk_draw_rectangle (window, style->bg_gc[state_type], TRUE, x, y + 2, width - 2, height - 4);
      sato_corner (window, gc, x + width, y, SATO_CORNER_TOP_RIGHT);
      sato_corner (window, gc, x + width, y + height, SATO_CORNER_BOTTOM_RIGHT);
      /* adjust for line width */
      x -= 2; y += 1; width += 1; height -= 2;
      gdk_draw_line (window, gc, x, y, x + width - 2, y);
      gdk_draw_line (window, gc, x, y + height, x + width - 2, y + height);
      gdk_draw_line (window, gc, x + width, y + 2, x + width, y + height - 2);
      break;
    case GTK_POS_RIGHT:	/* left tab */
      gdk_draw_rectangle (window, style->bg_gc[state_type], TRUE, x + 2, y + 1, width, height - 2);
      sato_corner (window, gc, x, y, SATO_CORNER_TOP_LEFT);
      sato_corner (window, gc, x, y + height, SATO_CORNER_BOTTOM_LEFT);
      /* adjust for line width */
      x += 1; y += 1; width += 1; height -= 2;
      gdk_draw_line (window, gc, x + 2, y, x + width, y);
      gdk_draw_line (window, gc, x + 2, y + height, x + width, y + height);
      gdk_draw_line (window, gc, x, y + 2, x, y + height - 2);
      break;
  }


  g_object_unref (gc);

}

static void
sato_draw_vline (GtkStyle *style, GdkWindow *window, GtkStateType state_type,
                 GdkRectangle *area, GtkWidget *widget, const gchar *detail,
                 gint y1, gint y2, gint x)
{
  GdkGC *line_gc;

  if (DETAIL ("vscale") || DETAIL ("hscale"))
    return;

  line_gc = sato_gc_new (style->text_gc[state_type], window);


  gdk_draw_line (window, line_gc, x, y1, x, y2);

  g_object_unref (line_gc);
}

static void
sato_draw_hline (GtkStyle *style, GdkWindow *window,  GtkStateType state_type,
                 GdkRectangle *area,GtkWidget *widget, const gchar *detail,
                 gint x1, gint x2, gint y)
{
  GdkGC *line_gc;

  if (DETAIL ("vscale") || DETAIL ("hscale"))
    return;

  line_gc = sato_gc_new (style->dark_gc[state_type], window);

  gdk_draw_line (window, line_gc, x1, y, x2, y);

  g_object_unref (line_gc);
}


void
sato_draw_style_class_init (GtkStyleClass * style_class)
{

  parent_style_class = g_type_class_peek_parent (style_class);

  style_class->draw_shadow = sato_draw_shadow;
  style_class->draw_box = sato_draw_box;
  style_class->draw_check = sato_draw_check;
  style_class->draw_option = sato_draw_option;
  style_class->draw_box_gap = sato_draw_box_gap;
  style_class->draw_shadow_gap = sato_draw_box_gap;
  style_class->draw_extension = sato_draw_extension;
  style_class->draw_hline = sato_draw_hline;
  style_class->draw_vline = sato_draw_vline;
  style_class->draw_focus = sato_draw_focus;

}


#endif /* not ENABLE_CAIRO */
