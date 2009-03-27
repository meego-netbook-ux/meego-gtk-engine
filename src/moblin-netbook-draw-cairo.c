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

#include "config.h"

#include "moblin-netbook-draw.h"
#include "moblin-netbook-style.h"
#include "moblin-netbook-utils.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#define DEBUG // g_printf ("%s: detail = '%s'; state = %d; x:%d; y:%d; w:%d; h:%d;\n", __FUNCTION__, detail, state_type, x, y, width, height);
#define DETAIL(foo) (detail && strcmp (foo, detail) == 0)

#define CAIRO_CLIP()  \
  if (area)                                                               \
    cairo_rectangle (cr, area->x, area->y, area->width, area->height);    \
  else                                                                    \
    cairo_rectangle (cr, x, y, width, height);    \
  cairo_clip (cr);

#define LINE_WIDTH 1
#define RADIUS 5

GtkStyleClass *parent_style_class;

static inline void
moblin_netbook_set_border_color (cairo_t *cr, GtkStyle *style)
{
 GdkColor border_color; 
 
 moblin_netbook_shade_colour (&style->bg[GTK_STATE_NORMAL], &border_color, 0.48); 
 gdk_cairo_set_source_color (cr, &border_color);
}


static void
moblin_netbook_rounded_rectangle (cairo_t *cr, gdouble x, gdouble y, gdouble width, gdouble height)
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
moblin_netbook_entry_shadow (cairo_t *cr, int x, int y, int width, int height, GdkColor *border_color)
{
  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_translate (cr, 0.5, 0.5);

  /* draw the inner shadow */
  cairo_set_source_rgba (cr, 0, 0, 0, 0.15);
  moblin_netbook_rounded_rectangle (cr, x + 1, y + 1, width - 1, height - 2);
  cairo_stroke (cr);

  /* draw the border */
  gdk_cairo_set_source_color (cr, border_color);
  moblin_netbook_rounded_rectangle (cr, x, y, width, height - 1);
  cairo_stroke (cr);

  /* draw the outer shadow */
  cairo_set_source_rgba (cr, 0, 0, 0, 0.07);
  cairo_move_to (cr, x + 4, y + height - 1);
  cairo_line_to (cr, x + width - 5, y + height - 1);
  cairo_stroke (cr);
}

static void
moblin_netbook_draw_box (DRAW_ARGS)
{
  cairo_t *cr;
  GdkColor border_color;
  gboolean add_shadow = TRUE;
  
  DEBUG;

  if (DETAIL ("paned"))
    return;
    
  if (DETAIL ("vscrollbar") || DETAIL ("hscrollbar") || DETAIL ("slider")
     || DETAIL ("bar") || DETAIL ("handlebox"))
  {
    add_shadow = FALSE;
  }

  moblin_netbook_shade_colour (&style->bg[GTK_STATE_NORMAL], &border_color, 0.48);

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
    moblin_netbook_rounded_rectangle (cm, 0, 0, width + 1, height + 1);
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
    cr = gdk_cairo_create (window);

    CAIRO_CLIP ();

    /* FIXME: for RTL */
    width += 10;
    x -= 10;

    moblin_netbook_entry_shadow (cr, x, y, width, height, &border_color);
    return;
  }

  /*** combo boxes ***/
  if (DETAIL ("button") && widget && GTK_IS_COMBO_BOX_ENTRY (widget->parent))
  {
    GtkWidget *entry;

    entry = g_object_get_data (G_OBJECT (widget->parent), "moblin_netbook-combo-entry");
    if (GTK_IS_ENTRY (entry))
    {
      gtk_widget_queue_draw_area (entry, entry->allocation.x, entry->allocation.y,
                                  entry->allocation.width,entry->allocation.height);
    }

    g_object_set_data (G_OBJECT (widget->parent), "moblin_netbook-combo-button", widget);

    cr = gdk_cairo_create (window);


    CAIRO_CLIP ();

    /* FIXME: RTL */
    width += 10;
    x -= 10;

    moblin_netbook_entry_shadow (cr, x, y, width, height, &border_color);
    cairo_destroy (cr);
    return;
  }

  if (widget
      && (DETAIL ("trough")
          || DETAIL ("trough-fill-level")
          || DETAIL ("trough-fill-level-full"))
      && GTK_IS_SCALE (widget))
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

  CAIRO_CLIP ();

  cairo_translate (cr, 0.5, 0.5);
  cairo_set_line_width (cr, LINE_WIDTH);


  /* special "fill" indicator */
  if (DETAIL ("trough-fill-level-full")
      || DETAIL ("trough-fill-level"))
    {
      gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_SELECTED]);
      moblin_netbook_rounded_rectangle (cr, x+1, y+1, width-2, height-2);
      cairo_fill (cr);
      cairo_destroy (cr);
      return;
    }

  /* menu and toolbars get just a single line at the bottom of the widget */
  if (DETAIL ("menubar") || DETAIL ("toolbar"))
  {
    cairo_rectangle (cr, x - 0.5, y - 0.5, width, height);
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

    /* background fill */
    moblin_netbook_rounded_rectangle (cr, x, y, width, height);
    gdk_cairo_set_source_color (cr, &style->bg[state_type]);
    cairo_fill (cr);

    if (shadow_type == GTK_SHADOW_IN)
      {
          cairo_move_to (cr, x + 1, y + 1);
          cairo_line_to (cr, x + width - 1, y + 1);
          cairo_set_source_rgba (cr,
                                 border_color.red / 0xffff,
                                 border_color.green / 0xffff,
                                 border_color.blue / 0xffff,
                                 0.1);
          cairo_stroke (cr);

          cairo_move_to (cr, x + 1, y + 2);
          cairo_line_to (cr, x + width - 1, y + 2);
          cairo_set_source_rgba (cr,
                                 border_color.red / 0xffff,
                                 border_color.green / 0xffff,
                                 border_color.blue / 0xffff,
                                 0.05);
          cairo_stroke (cr);
       
          cairo_move_to (cr, x + 1, y + 1);
          cairo_line_to (cr, x + 1, y + height);
          cairo_set_source_rgba (cr,
                                 border_color.red / 0xffff,
                                 border_color.green / 0xffff,
                                 border_color.blue / 0xffff,
                                 0.1);
          cairo_stroke (cr);

          cairo_move_to (cr, x + 2, y + 1);
          cairo_line_to (cr, x + 2, y + height);
          cairo_set_source_rgba (cr,
                                 border_color.red / 0xffff,
                                 border_color.green / 0xffff,
                                 border_color.blue / 0xffff,
                                 0.05);
          cairo_stroke (cr);
      }

  if (shadow_type != GTK_SHADOW_NONE)
    {
      /* border */
      moblin_netbook_rounded_rectangle (cr, x, y, width, height);
      gdk_cairo_set_source_color (cr, &border_color);
      cairo_stroke (cr);
    }

  }

  cairo_destroy (cr);

}

static void
moblin_netbook_draw_shadow (DRAW_ARGS)
{
  cairo_t *cr;
  GdkColor border_color;

  DEBUG;

  if (shadow_type == GTK_SHADOW_NONE)
    return;

  SANITIZE_SIZE;

  moblin_netbook_shade_colour (&style->bg[GTK_STATE_NORMAL], &border_color, 0.48);

  cr = gdk_cairo_create (window);

  CAIRO_CLIP ();

  /* FIXME: for RTL */
  if (widget && DETAIL ("entry") && (GTK_IS_SPIN_BUTTON (widget) || GTK_IS_COMBO_BOX_ENTRY (widget->parent)))
      width += 10;

  if (widget && DETAIL ("entry") && GTK_IS_COMBO_BOX_ENTRY (widget->parent))
  {
    GtkWidget *button;
    g_object_set_data (G_OBJECT (widget->parent), "moblin_netbook-combo-entry", widget);

    button = g_object_get_data (G_OBJECT (widget->parent), "moblin_netbook-combo-button");
    if (GTK_IS_BUTTON (button))
      gtk_widget_queue_draw_area (button, button->allocation.x, button->allocation.y,
                                  button->allocation.width,button->allocation.height);
  }

  moblin_netbook_entry_shadow (cr, x, y, width, height, &border_color);
  cairo_destroy (cr);
}

static void
moblin_netbook_draw_check (GtkStyle * style, GdkWindow * window,
		 GtkStateType state_type, GtkShadowType shadow_type,
		 GdkRectangle * area, GtkWidget * widget,
		 const gchar * detail, gint x, gint y, gint width,
		 gint height)
{
  cairo_t *cr;
  DEBUG;

  cr = gdk_cairo_create (window);
  CAIRO_CLIP();

  cairo_translate (cr, 0.5, 0.5);
  cairo_set_line_width (cr, LINE_WIDTH);

  /* fill the background */
  gdk_cairo_set_source_color (cr, &style->base[state_type]);
  moblin_netbook_rounded_rectangle (cr, x, y, width, height);
  cairo_fill (cr);

  /* inner shadow */
  cairo_set_source_rgba (cr, 0, 0, 0, 0.1);
  moblin_netbook_rounded_rectangle (cr, x + 1, y + 1, width - 1, height - 1);
  cairo_stroke (cr);
  
  cairo_set_source_rgba (cr, 0, 0, 0, 0.05);
  moblin_netbook_rounded_rectangle (cr, x + 2, y + 2, width - 2, height - 2);
  cairo_stroke (cr);


  /* draw the border */
  moblin_netbook_set_border_color (cr, style);
  moblin_netbook_rounded_rectangle (cr, x, y, width, height);
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
moblin_netbook_draw_option (GtkStyle * style, GdkWindow * window,
		  GtkStateType state_type, GtkShadowType shadow_type,
		  GdkRectangle * area, GtkWidget * widget,
		  const gchar * detail, gint x, gint y, gint width,
		  gint height)
{
  cairo_t *cr;
  gint cx, cy, radius;

  DEBUG;

  cr = gdk_cairo_create (window);
  CAIRO_CLIP();

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
  moblin_netbook_set_border_color (cr, style);
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
moblin_netbook_draw_box_gap (GtkStyle * style, GdkWindow * window,
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
  CAIRO_CLIP();

  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_translate (cr, 0.5, 0.5);
  gdk_cairo_set_source_color (cr, &style->fg[state_type]);

  /* start off with a rectangle... */
  cairo_rectangle (cr, x, y, width -1, height -1);
  cairo_stroke (cr);
  cairo_destroy (cr);

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
moblin_netbook_draw_extension (GtkStyle * style, GdkWindow * window,
		     GtkStateType state_type, GtkShadowType shadow_type,
		     GdkRectangle * area, GtkWidget * widget,const gchar * detail,
		     gint x, gint y, gint width, gint height,
		     GtkPositionType gap_side)
{
  cairo_t *cr;

  /* initialise the background */
  gtk_style_apply_default_background (style, window, TRUE, state_type, area,
                                      x, y, width, height);

  cr = gdk_cairo_create (window);
  CAIRO_CLIP();

  /* set up for line drawing */
  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
  cairo_translate (cr , 0.5, 0.5);

  /* reduce with and height since we are using them for co-ordinate values */
  width--; height--;


  /* active tab hilight stripe */
  if (state_type == GTK_STATE_NORMAL)
    {
      double strip_width = RADIUS - 0.5;
      switch (gap_side)
        {
        case GTK_POS_BOTTOM: /* top tab */
          cairo_move_to (cr, x, y + strip_width);
          cairo_arc (cr, x + strip_width, y + strip_width, strip_width, M_PI, M_PI * 1.5);
          cairo_arc (cr, x + width - strip_width, y + strip_width, strip_width, M_PI * 1.5, 0);
          cairo_line_to (cr, x + width, y + strip_width);
          break;
        case GTK_POS_TOP: /* bottom tab */
          cairo_move_to (cr, x, y + height - strip_width);
          cairo_arc_negative (cr, x + strip_width, y + height - strip_width, strip_width, M_PI, M_PI * 0.5);
          cairo_arc_negative (cr, x + width - strip_width, y + height - strip_width, strip_width, M_PI * 0.5, 0);
          cairo_line_to (cr, x + width, y + height - strip_width);
          break;
        case GTK_POS_LEFT: /* right tab */
          cairo_move_to (cr, x + width - strip_width, y);
          cairo_arc (cr, x + width - strip_width, y + strip_width, strip_width, M_PI * 1.5, 0);
          cairo_arc (cr, x + width - strip_width, y + height - strip_width, strip_width, 0, M_PI * 0.5);
          cairo_line_to (cr, x + width - strip_width, y + height);
          break;
        case GTK_POS_RIGHT: /* left tab */
          cairo_move_to (cr, x + strip_width, y);
          cairo_arc_negative (cr, x + strip_width, y + strip_width, strip_width, M_PI * 1.5, M_PI);
          cairo_arc_negative (cr, x + strip_width, y + height - strip_width, strip_width, M_PI, M_PI * 0.5);
          cairo_line_to (cr, x + strip_width, y + height);
          break;
        }
      gdk_cairo_set_source_color (cr, &style->bg[GTK_STATE_SELECTED]);
      cairo_fill (cr);
    }


  /* tab border */
  switch (gap_side)
  {
    case GTK_POS_TOP:     /* bottom tab */
      cairo_move_to (cr, x, y);
      cairo_arc_negative (cr, x + RADIUS, y + height - RADIUS, RADIUS, M_PI, M_PI * 0.5);
      cairo_arc_negative (cr, x + width - RADIUS, y + height - RADIUS, RADIUS, M_PI * 0.5, 0);
      cairo_line_to (cr, x + width, y);
      break;
    case GTK_POS_BOTTOM: /* top tab */
      cairo_move_to (cr, x, y + height);
      cairo_arc (cr, x + RADIUS, y + RADIUS, RADIUS, M_PI, M_PI * 1.5);
      cairo_arc (cr, x + width - RADIUS, y + RADIUS, RADIUS, M_PI * 1.5, 0);
      cairo_line_to (cr, x + width, y + height);
      break;
    case GTK_POS_LEFT:	/* right tab */
      cairo_move_to (cr, x, y);
      cairo_arc (cr, x + width - RADIUS, y + RADIUS, RADIUS, M_PI * 1.5, 0);
      cairo_arc (cr, x + width - RADIUS, y + height - RADIUS, RADIUS, 0, M_PI * 0.5);
      cairo_line_to (cr, x, y + height);
      break;
    case GTK_POS_RIGHT:	/* left tab */
      cairo_move_to (cr, x + width, y);
      cairo_arc_negative (cr, x + RADIUS, y + RADIUS, RADIUS, M_PI * 1.5, M_PI);
      cairo_arc_negative (cr, x + RADIUS, y + height - RADIUS, RADIUS, M_PI, M_PI * 0.5);
      cairo_line_to (cr, x + width, y + height);
      break;
  }

  gdk_cairo_set_source_color (cr, &style->fg[state_type]);
  cairo_stroke (cr);

  cairo_destroy (cr);

}

static void
moblin_netbook_draw_vline (GtkStyle *style, GdkWindow *window, GtkStateType state_type,
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
moblin_netbook_draw_hline (GtkStyle *style, GdkWindow *window,  GtkStateType state_type,
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
moblin_netbook_draw_arrow (GtkStyle *style,
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

  DEBUG;


  cr = gdk_cairo_create (window);
  CAIRO_CLIP();

  cairo_set_line_width (cr, 2);

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  if (DETAIL ("menuitem") && state_type == GTK_STATE_PRELIGHT)
    gdk_cairo_set_source_color (cr, &style->fg[state_type]);
  else
    gdk_cairo_set_source_color (cr, &style->bg[GTK_STATE_SELECTED]);

  if (DETAIL ("vscrollbar"))
    {
      x += (width - 6) / 2;
      width = 6;
    }
  else if (DETAIL ("hscrollbar"))
    {
      y += (height - 6) / 2;
      height = 6;
    }
  
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
moblin_netbook_draw_style_class_init (GtkStyleClass * style_class)
{

  parent_style_class = g_type_class_peek_parent (style_class);

  style_class->draw_shadow = moblin_netbook_draw_shadow;
  style_class->draw_box = moblin_netbook_draw_box;
  style_class->draw_check = moblin_netbook_draw_check;
  style_class->draw_option = moblin_netbook_draw_option;
  style_class->draw_box_gap = moblin_netbook_draw_box_gap;
  style_class->draw_shadow_gap = moblin_netbook_draw_box_gap;
  style_class->draw_extension = moblin_netbook_draw_extension;
  style_class->draw_hline = moblin_netbook_draw_hline;
  style_class->draw_vline = moblin_netbook_draw_vline;
  /* style_class->draw_focus = moblin_netbook_draw_focus; */
  style_class->draw_arrow = moblin_netbook_draw_arrow;

}


