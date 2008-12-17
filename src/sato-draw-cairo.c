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

#include "sato-draw.h"
#include "sato-style.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#define DEBUG(func) // g_printf ("%s: detail = '%s'; state = %d; x:%d; y:%d; w:%d; h:%d;\n", func, detail, state_type, x, y, width, height);
#define DETAIL(foo) (detail && strcmp (foo, detail) == 0)

#define LINE_WIDTH 1
#define RADIUS 4

GtkStyleClass *parent_style_class;

static void sato_draw_shadow (DRAW_ARGS);

static inline void
sato_set_border_color (cairo_t *cr, GtkStyle *style)
{
 GdkColor border_color; 
 
 sato_shade_colour (&style->bg[GTK_STATE_NORMAL], &border_color, 0.48); 
 gdk_cairo_set_source_color (cr, &border_color);
}


static void
sato_rounded_rectangle (cairo_t *cr, gdouble x, gdouble y, gdouble width, gdouble height)
{
  int radius = RADIUS;

  if (width < 1 || height < 1)
    return;

  /* this allows for calculating end co-ordinates */
  width--;
  height--;

  if (width < RADIUS * 2)
  {
    radius = width / 2;
  }
  else if (height < RADIUS * 2)
  {
    radius = height / 2;
  }

  cairo_move_to (cr, x, y + height - radius);
  cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
  cairo_arc (cr, x + width - radius, y + radius, radius, M_PI * 1.5, 0);
  cairo_arc (cr, x + width - radius, y + height - radius, radius, 0, M_PI * 0.5);
  cairo_arc (cr, x + radius, y + height - radius, radius, M_PI * 0.5, M_PI);
}

static void
sato_draw_box (DRAW_ARGS)
{
  cairo_t *cr;
  GdkColor border_color;
  gboolean add_shadow = TRUE;
  
  DEBUG ("draw_box");

  if (DETAIL ("paned"))
    return;
    
  if (DETAIL ("vscrollbar") || DETAIL ("hscrollbar") || DETAIL ("slider")
     || DETAIL ("bar"))
  {
    add_shadow = FALSE;
  }

  sato_shade_colour (&style->bg[GTK_STATE_NORMAL], &border_color, 0.48);

  if (width <= 0 && DETAIL ("menu"))
  {
    /* we're allowed to set a mask on the window here */
    GdkBitmap *bm;
    cairo_t *cm;
    GtkWidget *toplevel;

    SANITIZE_SIZE; /* get the correct sizes */

    bm = (GdkBitmap *) gdk_pixmap_new (NULL, width, height, 1);
    cm = gdk_cairo_create (bm);

    /* fill the pixmap black */
    cairo_save (cm);
    cairo_rectangle (cm, 0, 0, width, height);
    cairo_set_operator (cm, CAIRO_OPERATOR_CLEAR);
    cairo_fill (cm);
    cairo_restore (cm);

    /* create the mask as a white region */
    cairo_set_source_rgb (cm, 1.0, 1.0, 1.0);
    sato_rounded_rectangle (cm, 0, 0, width + 1, height + 1);
    cairo_fill (cm);
    cairo_destroy (cm);

    /* set the mask on the top level widget... */
    toplevel = gtk_widget_get_toplevel (widget);

    gtk_widget_shape_combine_mask (toplevel, bm, 0, 0);
    g_object_unref (bm);

    add_shadow = FALSE;
  }
  else
  {
    SANITIZE_SIZE;
  }


  /*** treeview headers ***/
  if (widget && GTK_IS_TREE_VIEW (widget->parent))
  {
    return;
  }

  /*** spin buttons ***/
  if (DETAIL ("spinbutton_down") || DETAIL ("spinbutton_up"))
    return;

  if (DETAIL ("spinbutton"))
  {
    /* FIXME: for RTL */
    width += 10;
    x -= 10;
    sato_draw_shadow (style, window, GTK_SHADOW_IN, shadow_type, area, widget, detail, x, y, width, height);
    return;
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
    }

    g_object_set_data (G_OBJECT (widget->parent), "sato-combo-button", widget);

    /* FIXME: RTL */
    width += 10;
    x -= 10;

    sato_draw_shadow (style, window, GTK_SHADOW_IN, shadow_type, area, widget, detail, x, y, width, height);
    return;
  }

  if (widget && DETAIL ("trough") && GTK_IS_SCALE (widget))
    {
      if (width > height)
        {
          height = 6;
          y += 4;
        }
      else
        {
          width = 6;
          x += 4;
        }
    }


  cr = gdk_cairo_create (window);
  cairo_translate (cr, 0.5, 0.5);
  cairo_set_line_width (cr, LINE_WIDTH);


  /* menu and toolbars get just a single line at the bottom of the widget */
  if (DETAIL ("menubar") || DETAIL ("toolbar"))
  {
    cairo_rectangle (cr, x, y, width, height);
    gdk_cairo_set_source_color (cr, &style->bg[state_type]);
    cairo_fill (cr);

    gdk_cairo_set_source_color (cr, &border_color);
    cairo_move_to (cr, x, y + height - 1);
    cairo_line_to (cr, x + width, y + height - 1);
    cairo_stroke (cr);
  }
  else
  {
    if (shadow_type == GTK_SHADOW_OUT && add_shadow)
      {
        /* outer shadow */
        cairo_move_to (cr, x + RADIUS, y + height - 1);
        cairo_line_to (cr, x + width - RADIUS - 1, y + height - 1);
        cairo_set_source_rgba (cr, 0, 0, 0, 0.07);
        cairo_stroke (cr);

        /* reduce height for outer shadow */
        height--;
      }

    sato_rounded_rectangle (cr, x, y, width, height);
    gdk_cairo_set_source_color (cr, &style->bg[state_type]);
    cairo_fill_preserve (cr);
    gdk_cairo_set_source_color (cr, &border_color);
    cairo_stroke (cr);

    if (shadow_type == GTK_SHADOW_IN)
      {
        cairo_move_to (cr, x + RADIUS / 2, y + 1);
        cairo_line_to (cr, x + width - RADIUS / 2, y + 1);
        cairo_set_source_rgba (cr,
                               border_color.red / 0xffff,
                               border_color.green / 0xffff,
                               border_color.blue / 0xffff,
                               0.2);
        cairo_stroke (cr);

        cairo_move_to (cr, x + RADIUS / 2, y + 2);
        cairo_line_to (cr, x + width - RADIUS / 2, y + 2);
        cairo_set_source_rgba (cr,
                               border_color.red / 0xffff,
                               border_color.green / 0xffff,
                               border_color.blue / 0xffff,
                               0.1);
        cairo_stroke (cr);
      }
  }

  cairo_destroy (cr);

}

static void
sato_draw_shadow (DRAW_ARGS)
{
  cairo_t *cr;
  GdkColor border_color;

  DEBUG ("draw_shadow");

  if (shadow_type == GTK_SHADOW_NONE)
    return;

  SANITIZE_SIZE;

  sato_shade_colour (&style->bg[GTK_STATE_NORMAL], &border_color, 0.48);

  /* FIXME: for RTL */
  if (widget && DETAIL ("entry") && (GTK_IS_SPIN_BUTTON (widget) || GTK_IS_COMBO_BOX_ENTRY (widget->parent)))
      width += 10;

  if (widget && DETAIL ("entry") && GTK_IS_COMBO_BOX_ENTRY (widget->parent))
  {
    GtkWidget *button;
    g_object_set_data (G_OBJECT (widget->parent), "sato-combo-entry", widget);

    button = g_object_get_data (G_OBJECT (widget->parent), "sato-combo-button");
    if (GTK_IS_BUTTON (button))
      gtk_widget_queue_draw_area (button, button->allocation.x, button->allocation.y,
                                  button->allocation.width,button->allocation.height);
  }

  cr = gdk_cairo_create (window);
  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_translate (cr, 0.5, 0.5);

  /* draw the inner shadow */
  cairo_set_source_rgba (cr, 0, 0, 0, 0.15);
  sato_rounded_rectangle (cr, x + 1, y + 1, width - 1, height - 2);
  cairo_stroke (cr);

  /* draw the border */
  gdk_cairo_set_source_color (cr, &border_color);
  sato_rounded_rectangle (cr, x, y, width, height - 1);
  cairo_stroke (cr);

  /* draw the outer shadow */
  cairo_set_source_rgba (cr, 0, 0, 0, 0.07);
  cairo_move_to (cr, x + 4, y + height - 1);
  cairo_line_to (cr, x + width - 5, y + height - 1);
  cairo_stroke (cr);

  cairo_destroy (cr);
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
  cairo_t *cr;
  DEBUG ("draw_check");

  cr = gdk_cairo_create (window);
  cairo_translate (cr, 0.5, 0.5);
  cairo_set_line_width (cr, LINE_WIDTH);

  /* fill the background */
  gdk_cairo_set_source_color (cr, &style->base[state_type]);
  sato_rounded_rectangle (cr, x, y, width, height);
  cairo_fill (cr);

  /* inner shadow */
  cairo_set_source_rgba (cr, 0, 0, 0, 0.1);
  sato_rounded_rectangle (cr, x + 1, y + 1, width - 1, height - 1);
  cairo_stroke (cr);
  
  cairo_set_source_rgba (cr, 0, 0, 0, 0.05);
  sato_rounded_rectangle (cr, x + 2, y + 2, width - 2, height - 2);
  cairo_stroke (cr);


  /* draw the border */
  sato_set_border_color (cr, style);
  sato_rounded_rectangle (cr, x, y, width, height);
  cairo_stroke (cr);
  
  gdk_cairo_set_source_color (cr, &style->text[state_type]);

  /*** draw check mark ***/
  if (shadow_type == GTK_SHADOW_IN)
  {
    x += 2;
    y += 3;
    width -= 5;
    height -= 6;
    
    cairo_set_line_width (cr, 2);

    cairo_rectangle (cr, x, y, width, height);
    cairo_clip (cr);
    y += 1;
    height -= 2;

    cairo_move_to (cr, x, y + height * 0.53);
    cairo_line_to (cr, x + width * 0.3, y + height);
    cairo_line_to (cr, x + width, y);

    cairo_stroke (cr);
  }
  cairo_destroy (cr);

}


static void
sato_draw_option (GtkStyle * style, GdkWindow * window,
		  GtkStateType state_type, GtkShadowType shadow_type,
		  GdkRectangle * area, GtkWidget * widget,
		  const gchar * detail, gint x, gint y, gint width,
		  gint height)
{
  cairo_t *cr;
  gint cx, cy, radius;

  DEBUG ("draw_option");

  cr = gdk_cairo_create (window);
  cairo_set_line_width (cr, 1);
  cairo_translate (cr, 0.5, 0.5);

  /* define radius and centre coordinates */
  if (width % 2) width--;
  radius = width / 2;
  cx = x + radius;
  cy = y + radius;

  /* fill the background */
  gdk_cairo_set_source_color (cr, &style->base[state_type]);
  cairo_arc (cr, cx, cy, radius, 0, M_PI * 2);
  cairo_fill (cr);
  
  /* inner shadow */
  cairo_arc (cr, cx, cy, radius - 1, M_PI * 0.75, M_PI * 1.75);
  cairo_set_source_rgba (cr, 0, 0, 0, 0.2);
  cairo_stroke (cr);

  /* draw the border */
  cairo_arc (cr, cx, cy, radius, 0, M_PI * 2);
  sato_set_border_color (cr, style);
  cairo_stroke (cr);

  /*** draw check mark ***/
  if (shadow_type == GTK_SHADOW_IN)
  {
    cairo_arc (cr, cx, cy, radius - 4,  0, M_PI * 2);
    gdk_cairo_set_source_color (cr, &style->text[state_type]);
    cairo_fill (cr);
  }
  cairo_destroy (cr);
}

static void
sato_draw_box_gap (GtkStyle * style, GdkWindow * window,
		   GtkStateType state_type, GtkShadowType shadow_type,
		   GdkRectangle * area, GtkWidget * widget, const gchar * detail,
		   gint x, gint y, gint width, gint height,
		   GtkPositionType gap_side, gint gap_x, gint gap_width)
{
  cairo_t *cr;
  GdkRectangle rect;

  if (shadow_type == GTK_SHADOW_NONE)
    return;


  cr = gdk_cairo_create (window);
  cairo_set_line_width (cr, LINE_WIDTH);
  gdk_cairo_set_source_color (cr, &style->fg[state_type]);

  /* start off with a rectangle... */
  cairo_rectangle (cr, x + LINE_WIDTH / 2.0, y + LINE_WIDTH / 2.0, width - LINE_WIDTH, height - LINE_WIDTH);
  cairo_stroke (cr);

  switch (gap_side)
  {
    case GTK_POS_TOP:
      rect.x = x + gap_x + LINE_WIDTH;
      rect.y = y;
      rect.width = gap_width - LINE_WIDTH * 2;
      rect.height = LINE_WIDTH;
      break;
    case GTK_POS_BOTTOM:
      rect.x = x + gap_x + LINE_WIDTH;
      rect.y = y + height - 2;
      rect.width = gap_width - LINE_WIDTH * 2;
      rect.height = 2;
      break;
    case GTK_POS_LEFT:
      rect.x = x;
      rect.y = y + gap_x + LINE_WIDTH;
      rect.width = 2;
      rect.height = gap_width - LINE_WIDTH * 2;
      break;
    case GTK_POS_RIGHT:
      rect.x = x + width - 2;
      rect.y = y + gap_x + LINE_WIDTH;
      rect.width = 2;
      rect.height = gap_width - LINE_WIDTH * 2;
      break;
  }

  /* and finally blank out the gap */
  gtk_style_apply_default_background (style, window, TRUE, state_type, area,
				      rect.x, rect.y, rect.width,
				      rect.height);


}


static void
sato_draw_extension (GtkStyle * style, GdkWindow * window,
		     GtkStateType state_type, GtkShadowType shadow_type,
		     GdkRectangle * area, GtkWidget * widget,const gchar * detail,
		     gint x, gint y, gint width, gint height,
		     GtkPositionType gap_side)
{
  cairo_t *cr;
  gdouble cx, cy;

  cr = gdk_cairo_create (window);
  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);


  cx = x + LINE_WIDTH / 2.0;
  cy = y + LINE_WIDTH / 2.0;
  width -= LINE_WIDTH;
  height -= LINE_WIDTH;

  switch (gap_side)
  {
    case GTK_POS_TOP:     /* bottom tab */
      /* this allows for some overlap */
      if (state_type == GTK_STATE_NORMAL)
        cy -= LINE_WIDTH;

      cairo_move_to (cr, cx, cy);
      cairo_arc_negative (cr, cx + RADIUS, cy + height - RADIUS, RADIUS, M_PI, M_PI * 0.5);
      cairo_arc_negative (cr, cx + width - RADIUS, cy + height - RADIUS, RADIUS, M_PI * 0.5, 0);
      cairo_line_to (cr, cx + width, cy);
      break;
    case GTK_POS_BOTTOM: /* top tab */
      if (state_type == GTK_STATE_NORMAL)
        height += LINE_WIDTH;

      cairo_move_to (cr, cx, cy + height);
      cairo_arc (cr, cx + RADIUS, cy + RADIUS, RADIUS, M_PI, M_PI * 1.5);
      cairo_arc (cr, cx + width - RADIUS, cy + RADIUS, RADIUS, M_PI * 1.5, 0);
      cairo_line_to (cr, cx + width, cy + height);
      break;
    case GTK_POS_LEFT:	/* right tab */
      if (state_type == GTK_STATE_NORMAL)
        cx -= LINE_WIDTH;
      cairo_move_to (cr, cx, cy);
      cairo_arc (cr, cx + width - RADIUS, cy + RADIUS, RADIUS, M_PI * 1.5, 0);
      cairo_arc (cr, cx + width - RADIUS, cy + height - RADIUS, RADIUS, 0, M_PI * 0.5);
      cairo_line_to (cr, cx, cy + height);
      break;
    case GTK_POS_RIGHT:	/* left tab */
      if (state_type == GTK_STATE_NORMAL)
        width += LINE_WIDTH;
      cairo_move_to (cr, cx + width, cy);
      cairo_arc_negative (cr, cx + RADIUS, cy + RADIUS, RADIUS, M_PI * 1.5, M_PI);
      cairo_arc_negative (cr, cx + RADIUS, cy + height - RADIUS, RADIUS, M_PI, M_PI * 0.5);
      cairo_line_to (cr, cx + width, cy + height);
      break;
  }

  gdk_cairo_set_source_color (cr, &style->bg[state_type]);
  cairo_fill_preserve (cr);

  gdk_cairo_set_source_color (cr, &style->fg[state_type]);
  cairo_stroke (cr);

  cairo_destroy (cr);

}

static void
sato_draw_vline (GtkStyle *style, GdkWindow *window, GtkStateType state_type,
                 GdkRectangle *area, GtkWidget *widget, const gchar *detail,
                 gint y1, gint y2, gint x)
{
  cairo_t *cr;

  if (DETAIL ("vscale") || DETAIL ("hscale"))
    return;

  if (DETAIL ("vseparator"))
    return;

  cr = gdk_cairo_create (window);
  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  gdk_cairo_set_source_color (cr, &style->dark[state_type]);
  cairo_move_to (cr, x + LINE_WIDTH / 2.0, y1);
  cairo_line_to (cr, x + LINE_WIDTH / 2.0, y2);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
sato_draw_hline (GtkStyle *style, GdkWindow *window,  GtkStateType state_type,
                 GdkRectangle *area,GtkWidget *widget, const gchar *detail,
                 gint x1, gint x2, gint y)
{
  cairo_t *cr;

  if (DETAIL ("vscale") || DETAIL ("hscale"))
    return;

  cr = gdk_cairo_create (window);
  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  gdk_cairo_set_source_color (cr, &style->dark[state_type]);
  cairo_move_to (cr, x1, y + LINE_WIDTH / 2.0);
  cairo_line_to (cr, x2, y + LINE_WIDTH / 2.0);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
sato_draw_arrow (GtkStyle *style,
                 GdkWindow *window,
                 GtkStateType state_type,
                 GtkShadowType shadow_type,
                 GdkRectangle *area,
                 GtkWidget *widget,
                 const gchar *detail,
                 GtkArrowType arrow_type,
                 gboolean fill,
                 gint x,
                 gint y,
                 gint width,
                 gint height)
{
  cairo_t *cr;
  
  cr = gdk_cairo_create (window);
  cairo_set_line_width (cr, 2);

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
  gdk_cairo_set_source_color (cr, &style->bg[GTK_STATE_SELECTED]);
  
  /* ensure we have odd number of pixels for width or height to allow for
   * correct centering
   */
  if (width % 2) width--;
  if (height % 2) height--;

  switch (arrow_type)
  {
    case GTK_ARROW_UP:
      y = y + height / 2 - (width * 0.12);
      height = width * 0.6;
      cairo_move_to (cr, x, y + height);
      cairo_line_to (cr, x + width / 2, y);
      cairo_line_to (cr, x + width, y + height);
      break;
    case GTK_ARROW_DOWN:
      y = y + height / 2 - (width * 0.12);
      height = width * 0.6;
      cairo_move_to (cr, x, y);
      cairo_line_to (cr, x + width / 2, y + height);
      cairo_line_to (cr, x + width, y);
      break;
    case GTK_ARROW_LEFT:
      x = x + width / 2 - (height * 0.12);
      width = height * 0.6;
      cairo_move_to (cr, x + width, y);
      cairo_line_to (cr, x, y + height / 2);
      cairo_line_to (cr, x + width, y + height);
      break;
    case GTK_ARROW_RIGHT:
       x = x + width / 2 - (height * 0.12);
      width = height * 0.6;
      cairo_move_to (cr, x, y);
      cairo_line_to (cr, x + width, y + height / 2);
      cairo_line_to (cr, x, y + height);
      break;
    case GTK_ARROW_NONE:
    break;
  }
  cairo_stroke (cr);
  cairo_destroy (cr);
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
  style_class->draw_arrow = sato_draw_arrow;

}


