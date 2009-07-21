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

#include "moblin-netbook-style.h"
#include "moblin-netbook-utils.h"
#include "moblin-netbook-rc-style.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#define DEBUG // g_printf ("%s: detail = '%s'; state = %d; x:%d; y:%d; w:%d; h:%d;\n", __FUNCTION__, detail, state_type, x, y, width, height);
#define DETAIL(foo) (detail && strcmp (foo, detail) == 0)

#define LINE_WIDTH 1

G_DEFINE_DYNAMIC_TYPE (MoblinNetbookStyle, moblin_netbook_style,
                       GTK_TYPE_STYLE)

void
_moblin_netbook_style_register_type (GTypeModule *module)
{
  moblin_netbook_style_register_type (module);
}

static cairo_t*
moblin_netbook_cairo_create (GdkWindow    *window,
                             GdkRectangle *area)
{
  cairo_t *cr;

  cr = gdk_cairo_create (window);

  if (area)
    {
      cairo_rectangle (cr, area->x, area->y, area->width, area->height);
      cairo_clip (cr);
    }

  return cr;
}

static inline void
moblin_netbook_set_border_color (cairo_t     *cr,
                                 GtkStyle    *style,
                                 GtkStateType state)
{
  if (MOBLIN_NETBOOK_STYLE (style)->border_color)
    gdk_cairo_set_source_color (cr,
                                &(MOBLIN_NETBOOK_STYLE (style)->border_color[
                                    state]));
}


static void
moblin_netbook_rounded_rectangle (cairo_t *cr,
                                  gdouble  x,
                                  gdouble  y,
                                  gdouble  width,
                                  gdouble  height,
                                  gdouble  radius)
{
  if (width < 1 || height < 1)
    return;

  if (radius == 0)
    {
      cairo_rectangle (cr, x, y, width, height);
      return;
    }

  if (width < radius * 2)
    {
      radius = width / 2;
    }
  else if (height < radius * 2)
    {
      radius = height / 2;
    }

  cairo_move_to (cr, x, y + height - radius);
  cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
  cairo_arc (cr, x + width - radius, y + radius, radius, M_PI * 1.5, 0);
  cairo_arc (cr, x + width - radius, y + height - radius, radius, 0,
             M_PI * 0.5);
  cairo_arc (cr, x + radius, y + height - radius, radius, M_PI * 0.5, M_PI);
}

static void
moblin_netbook_draw_grip (cairo_t *cr,
                          gboolean vertical,
                          gdouble  x,
                          gdouble  y,
                          gdouble  width,
                          gdouble  height)
{
  gdouble cx, cy;
  gint n_strips = 3;
  gint strip_w = 3;
  gint strip_h = 8 - strip_w;
  gint strip_padding = 2;
  gint grip_w = (n_strips * strip_w) + ((n_strips - 1) * strip_padding);
  gint i;

  cairo_save (cr);

  cairo_set_line_width (cr, strip_w);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  if (!vertical)
    {
      cx = x + (width / 2.0) - grip_w / 2.0 + strip_w / 2.0;
      cy = y + (height / 2.0) - (strip_h / 2.0);

      for (i = 0; i < n_strips; i++)
        {
          gdouble position;

          position = cx + (i * (strip_w + strip_padding));

          cairo_move_to (cr, position, cy);
          cairo_line_to (cr, position, cy + strip_h);

          cairo_stroke (cr);
        }
    }
  else
    {
      cy = y + (height / 2.0) - grip_w / 2.0 + strip_w / 2.0;
      cx = x + (width / 2.0) - (strip_h / 2.0);

      for (i = 0; i < n_strips; i++)
        {
          gdouble position;

          position = cy + (i * (strip_w + strip_padding));

          cairo_move_to (cr, cx, position);
          cairo_line_to (cr, cx + strip_h, position);

          cairo_stroke (cr);
        }
    }

  cairo_restore (cr);
}

static void
moblin_netbook_draw_box (GtkStyle     *style,
                         GdkWindow    *window,
                         GtkStateType  state_type,
                         GtkShadowType shadow_type,
                         GdkRectangle *area,
                         GtkWidget    *widget,
                         const gchar  *detail,
                         gint          x,
                         gint          y,
                         gint          width,
                         gint          height)
{
  cairo_t *cr;
  MoblinNetbookStyle *mb_style = MOBLIN_NETBOOK_STYLE (style);
  gint radius = mb_style->radius;

  DEBUG;

  if (DETAIL ("paned") || DETAIL ("buttondefault"))
    return;


  /* we want hover and focused widgets to look the same */
  if (widget && GTK_WIDGET_HAS_FOCUS (widget))
    state_type = GTK_STATE_PRELIGHT;


  /* scrollbar troughs are a plain rectangle */
  if (widget && GTK_IS_SCROLLBAR (widget) && DETAIL ("trough"))
    {
      cr = moblin_netbook_cairo_create (window, area);

      cairo_rectangle (cr, x, y, width, height);
      gdk_cairo_set_source_color (cr, &style->bg[state_type]);
      cairo_fill (cr);

      cairo_destroy (cr);

      return;
    }

  SANITIZE_SIZE;

  /*** treeview headers ***/
  if (widget && GTK_IS_TREE_VIEW (widget->parent))
    {
      cr = moblin_netbook_cairo_create (window, area);

      cairo_rectangle (cr, x, y, width, height);
      gdk_cairo_set_source_color (cr, &style->bg[state_type]);
      cairo_fill (cr);
      cairo_destroy (cr);

      gtk_paint_vline (style, window, state_type, area, widget, detail,
                       y + 5, y + height - 5, x + width - 1);
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
    }

  /*** combo boxes ***/
  if (DETAIL ("button") && widget && GTK_IS_COMBO_BOX_ENTRY (widget->parent))
    {
      GtkWidget *entry;

      entry = g_object_get_data (G_OBJECT (widget->parent),
                                 "moblin-netbook-combo-entry");
      g_object_set_data (G_OBJECT (widget->parent),
                         "moblin-netbook-combo-button", widget);
      g_object_set_data (G_OBJECT (widget->parent),
                         "moblin-netbook-combo-button-shadow",
                         GINT_TO_POINTER (shadow_type));


      if (GTK_IS_ENTRY (entry))
        {
          gtk_widget_queue_draw (entry);
        }

      /* FIXME: RTL */
      width += 10;
      x -= 10;
    }

  if (widget
      && (DETAIL ("trough")
          || DETAIL ("trough-fill-level")
          || DETAIL ("trough-fill-level-full"))
      && GTK_IS_SCALE (widget))
    {
      if (mb_style->shadow)
        {
          width--;
          height--;
        }
      if (width > height)
        {
          y = y + (height / 2.0 - 2);
          height = 4;
        }
      else
        {
          x = x + (width / 2.0 - 2);
          width = 4;
        }
    }


  /* scrollbar buttons */
  if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar"))
    {
      x += 2;
      y += 2;
      width -= 4;
      height -= 4;
    }

  /* scrollbar slider */
  if (DETAIL ("slider"))
    {
      if (width > height)
        {
          y += 2;
          height -= 4;
        }
      else
        {
          x += 2;
          width -= 4;
        }
    }

  cr = moblin_netbook_cairo_create (window, area);

  cairo_set_line_width (cr, LINE_WIDTH);


  /* special "fill" indicator */
  if (DETAIL ("trough-fill-level-full")
      || DETAIL ("trough-fill-level"))
    {
      gdk_cairo_set_source_color (cr, &style->base[GTK_STATE_SELECTED]);
      moblin_netbook_rounded_rectangle (cr, x+1, y+1, width-2, height-2,
                                        radius);
      cairo_fill (cr);
      cairo_destroy (cr);
      return;
    }

  if (mb_style->shadow)
    {
      if (shadow_type == GTK_SHADOW_OUT)
        {
          /* outer shadow */
          moblin_netbook_rounded_rectangle (cr, x, y, width, height,
                                            radius + 1);
          cairo_set_source_rgba (cr, 0, 0, 0, mb_style->shadow);
          cairo_fill (cr);

          /* reduce size for outer shadow */
          height--;
          width--;
        }
      else if (shadow_type == GTK_SHADOW_IN)
        {
          x++;
          y++;
          width--;
          height--;
        }
    }


  /* fill */
  moblin_netbook_rounded_rectangle (cr, x, y, width, height, radius);
  gdk_cairo_set_source_color (cr, &style->bg[state_type]);
  cairo_fill (cr);

  if (shadow_type != GTK_SHADOW_NONE)
    {
      /* border */
      moblin_netbook_rounded_rectangle (cr, x + 0.5, y + 0.5,
                                        width - 1, height - 1, radius);
      moblin_netbook_set_border_color (cr, style, state_type);
      cairo_stroke (cr);
    }

  /* add a grip to handles */
  if (DETAIL ("light-switch-handle") || DETAIL ("hscale"))
    {
      gdk_cairo_set_source_color (cr, &style->mid[state_type]);
      moblin_netbook_draw_grip (cr, FALSE, x, y, width, height);
    }
  else if (DETAIL ("vscale"))
    {
      gdk_cairo_set_source_color (cr, &style->mid[state_type]);
      moblin_netbook_draw_grip (cr, TRUE, x, y, width, height);
    }
  cairo_destroy (cr);

}

static void
moblin_netbook_draw_shadow (GtkStyle     *style,
                            GdkWindow    *window,
                            GtkStateType  state_type,
                            GtkShadowType shadow_type,
                            GdkRectangle *area,
                            GtkWidget    *widget,
                            const gchar  *detail,
                            gint          x,
                            gint          y,
                            gint          width,
                            gint          height)
{
  cairo_t *cr;
  MoblinNetbookStyle *mb_style = MOBLIN_NETBOOK_STYLE (style);
  gdouble radius = mb_style->radius;

  DEBUG;

  if (shadow_type == GTK_SHADOW_NONE)
    return;

  SANITIZE_SIZE;

  cr = moblin_netbook_cairo_create (window, area);

  /* FIXME: for RTL */
  if (widget && DETAIL ("entry")
      && (GTK_IS_SPIN_BUTTON (widget)
          || GTK_IS_COMBO_BOX_ENTRY (widget->parent)))
    width += 10;

  if (widget && DETAIL ("entry") && GTK_IS_COMBO_BOX_ENTRY (widget->parent))
    {
      GtkWidget *button;
      gpointer pointer;

      g_object_set_data (G_OBJECT (widget->parent),
                         "moblin-netbook-combo-entry", widget);

      button = g_object_get_data (G_OBJECT (widget->parent),
                                  "moblin-netbook-combo-button");

      pointer = g_object_get_data (G_OBJECT (widget->parent),
                                   "moblin-netbook-combo-button-shadow");
      shadow_type = GPOINTER_TO_INT (pointer);

      width += 10;
    }

  cairo_translate (cr, 0.5, 0.5);
  width--;
  height--;

  cairo_set_line_width (cr, 1.0);
  if (mb_style->shadow != 0.0)
    {
      if (shadow_type == GTK_SHADOW_OUT)
        {
          /* outer shadow */
          moblin_netbook_rounded_rectangle (cr, x, y, width, height,
                                            radius + 1.0);
          cairo_set_source_rgba (cr, 0, 0, 0, mb_style->shadow);
          cairo_stroke (cr);
        }
      else if (shadow_type == GTK_SHADOW_IN)
        {
          x++;
          y++;
        }

      /* reduce size for outer shadow */
      height--;
      width--;
    }

  /* border */
  moblin_netbook_rounded_rectangle (cr, x, y, width, height, radius);
  moblin_netbook_set_border_color (cr, style, state_type);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
moblin_netbook_draw_check (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GtkShadowType shadow_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           gint          x,
                           gint          y,
                           gint          width,
                           gint          height)
{
  cairo_t *cr;
  gint radius = MOBLIN_NETBOOK_STYLE (style)->radius;

  DEBUG;

  cr = moblin_netbook_cairo_create (window, area);

  cairo_set_line_width (cr, 1.0);

  if (shadow_type == GTK_SHADOW_IN)
    {
      state_type = GTK_STATE_SELECTED;
    }

  /* we don't support anything other than 15x15 */
  width = 15;
  height = 15;

  moblin_netbook_rounded_rectangle (cr, x + 0.5, y + 0.5,
                                    width - 1, height - 1, radius);

  /* fill the background */
  gdk_cairo_set_source_color (cr, &style->base[state_type]);
  cairo_fill_preserve (cr);

  /* draw the border */
  moblin_netbook_set_border_color (cr, style, state_type);
  cairo_stroke (cr);

  gdk_cairo_set_source_color (cr, &style->text[state_type]);

  /* draw a tick when checked */
  if (shadow_type == GTK_SHADOW_IN)
    {
      cairo_translate (cr, x, y);

      cairo_move_to (cr, 3, 6);
      cairo_line_to (cr, 6, 9);
      cairo_line_to (cr, 12, 3);
      cairo_line_to (cr, 12, 6);
      cairo_line_to (cr, 6, 12);
      cairo_line_to (cr, 3, 9);
      cairo_line_to (cr, 3, 6);

      cairo_fill (cr);
    }
  cairo_destroy (cr);

}


static void
moblin_netbook_draw_option (GtkStyle     *style,
                            GdkWindow    *window,
                            GtkStateType  state_type,
                            GtkShadowType shadow_type,
                            GdkRectangle *area,
                            GtkWidget    *widget,
                            const gchar  *detail,
                            gint          x,
                            gint          y,
                            gint          width,
                            gint          height)
{
  cairo_t *cr;
  gint cx, cy, radius;

  DEBUG;

  cr = moblin_netbook_cairo_create (window, area);

  cairo_set_line_width (cr, 1);
  cairo_translate (cr, 0.5, 0.5);
  width--;
  height--;

  if (shadow_type == GTK_SHADOW_IN)
    {
      state_type = GTK_STATE_SELECTED;
    }

  /* define radius and centre coordinates */
  if (width % 2) width--;
  radius = width / 2;
  cx = x + radius;
  cy = y + radius;

  /* fill the background */
  gdk_cairo_set_source_color (cr, &style->base[state_type]);
  cairo_arc (cr, cx, cy, radius, 0, M_PI * 2);
  cairo_fill (cr);

  /* draw the border */
  cairo_arc (cr, cx, cy, radius, 0, M_PI * 2);
  moblin_netbook_set_border_color (cr, style, state_type);
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
moblin_netbook_draw_box_gap (GtkStyle       *style,
                             GdkWindow      *window,
                             GtkStateType    state_type,
                             GtkShadowType   shadow_type,
                             GdkRectangle   *area,
                             GtkWidget      *widget,
                             const gchar    *detail,
                             gint            x,
                             gint            y,
                             gint            width,
                             gint            height,
                             GtkPositionType gap_side,
                             gint            gap_x,
                             gint            gap_width)
{
  cairo_t *cr;
  GdkRectangle rect;

  if (shadow_type == GTK_SHADOW_NONE)
    return;

  cr = moblin_netbook_cairo_create (window, area);

  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_translate (cr, 0.5, 0.5);

  moblin_netbook_set_border_color (cr, style, state_type);

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
moblin_netbook_draw_extension (GtkStyle       *style,
                               GdkWindow      *window,
                               GtkStateType    state_type,
                               GtkShadowType   shadow_type,
                               GdkRectangle   *area,
                               GtkWidget      *widget,
                               const gchar    *detail,
                               gint            x,
                               gint            y,
                               gint            width,
                               gint            height,
                               GtkPositionType gap_side)
{
  cairo_t *cr;
  gint radius = MOBLIN_NETBOOK_STYLE (style)->radius;

  /* initialise the background */
  gtk_style_apply_default_background (style, window, TRUE, state_type, area,
                                      x, y, width, height);

  cr = moblin_netbook_cairo_create (window, area);

  /* set up for line drawing */
  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);
  cairo_translate (cr, 0.5, 0.5);

  /* reduce with and height since we are using them for co-ordinate values */
  width--; height--;

  /* tab border */
  switch (gap_side)
    {
    case GTK_POS_TOP:     /* bottom tab */
      cairo_move_to (cr, x, y);
      cairo_arc_negative (cr, x + radius, y + height - radius, radius, M_PI,
                          M_PI * 0.5);
      cairo_arc_negative (cr, x + width - radius, y + height - radius, radius,
                          M_PI * 0.5, 0);
      cairo_line_to (cr, x + width, y);
      break;
    case GTK_POS_BOTTOM: /* top tab */
      cairo_move_to (cr, x, y + height);
      cairo_arc (cr, x + radius, y + radius, radius, M_PI, M_PI * 1.5);
      cairo_arc (cr, x + width - radius, y + radius, radius, M_PI * 1.5, 0);
      cairo_line_to (cr, x + width, y + height);
      break;
    case GTK_POS_LEFT:  /* right tab */
      cairo_move_to (cr, x, y);
      cairo_arc (cr, x + width - radius, y + radius, radius, M_PI * 1.5, 0);
      cairo_arc (cr, x + width - radius, y + height - radius, radius, 0,
                 M_PI * 0.5);
      cairo_line_to (cr, x, y + height);
      break;
    case GTK_POS_RIGHT: /* left tab */
      cairo_move_to (cr, x + width, y);
      cairo_arc_negative (cr, x + radius, y + radius, radius, M_PI * 1.5, M_PI);
      cairo_arc_negative (cr, x + radius, y + height - radius, radius, M_PI,
                          M_PI * 0.5);
      cairo_line_to (cr, x + width, y + height);
      break;
    }

  moblin_netbook_set_border_color (cr, style, state_type);
  cairo_stroke (cr);

  cairo_destroy (cr);

}

static void
moblin_netbook_draw_vline (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           gint          y1,
                           gint          y2,
                           gint          x)
{
  cairo_t *cr;

  if (DETAIL ("vscale") || DETAIL ("hscale"))
    return;

  if (DETAIL ("vseparator"))
    return;

  cr = gdk_cairo_create (window);

  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  cairo_move_to (cr, x + LINE_WIDTH / 2.0, y1);
  cairo_line_to (cr, x + LINE_WIDTH / 2.0, y2);

  moblin_netbook_set_border_color (cr, style, state_type);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
moblin_netbook_draw_hline (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           gint          x1,
                           gint          x2,
                           gint          y)
{
  cairo_t *cr;

  if (DETAIL ("vscale") || DETAIL ("hscale"))
    return;

  cr = gdk_cairo_create (window);

  cairo_set_line_width (cr, LINE_WIDTH);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  moblin_netbook_set_border_color (cr, style, state_type);
  cairo_move_to (cr, x1, y + LINE_WIDTH / 2.0);
  cairo_line_to (cr, x2, y + LINE_WIDTH / 2.0);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
moblin_netbook_draw_focus (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           gint          x,
                           gint          y,
                           gint          width,
                           gint          height)
{
  cairo_t *cr;
  gint line_width;
  MoblinNetbookStyle *mb_style = MOBLIN_NETBOOK_STYLE (style);

  cr = gdk_cairo_create (window);

  if (widget)
    gtk_widget_style_get (widget, "focus-line-width", &line_width, NULL);
  else
    line_width = 1;

  cairo_translate (cr, line_width / 2.0, line_width / 2.0);
  width -= line_width;
  height -= line_width;

  if (mb_style->shadow)
    {
      width -= 1;
      height -= 1;
    }

  moblin_netbook_rounded_rectangle (cr, x, y, width, height, line_width);
  cairo_set_line_width (cr, line_width);
  gdk_cairo_set_source_color (cr, &style->bg[GTK_STATE_SELECTED]);
  cairo_stroke (cr);

  cairo_destroy (cr);
}

static void
moblin_netbook_draw_arrow (GtkStyle     *style,
                           GdkWindow    *window,
                           GtkStateType  state_type,
                           GtkShadowType shadow_type,
                           GdkRectangle *area,
                           GtkWidget    *widget,
                           const gchar  *detail,
                           GtkArrowType  arrow_type,
                           gboolean      fill,
                           gint          x,
                           gint          y,
                           gint          width,
                           gint          height)
{
  cairo_t *cr;

  DEBUG;

  cr = moblin_netbook_cairo_create (window, area);

  cairo_set_line_width (cr, 2);

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

  gdk_cairo_set_source_color (cr, &style->fg[state_type]);

  /* add padding around scrollbar buttons */
  if (DETAIL ("vscrollbar") || DETAIL ("hscrollbar"))
    {
      x += 3;
      width -= 4;
      y += 3;
      height -= 4;
    }

  /* ensure we have odd number of pixels for width or height to allow for
   * correct centering
   */
  if (width % 2) width--;
  if (height % 2) height--;


  switch (arrow_type)
    {
    case GTK_ARROW_UP:
      y = y + height / 2 - (width * 0.12) - 1;
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
      x = x + width / 2 - (height * 0.12) - 1;
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

static void
moblin_netbook_init_from_rc (GtkStyle   *style,
                             GtkRcStyle *rc_style)
{
  int i;
  MoblinNetbookStyle *mb_style;
  MoblinNetbookRcStyle *mb_rc_style;

  GTK_STYLE_CLASS (moblin_netbook_style_parent_class)->init_from_rc (style,
                                                                     rc_style);

  mb_rc_style = MOBLIN_NETBOOK_RC_STYLE (rc_style);
  mb_style = MOBLIN_NETBOOK_STYLE (style);

  mb_style->radius = mb_rc_style->radius;

  for (i = 0; i < 5; i++)
    mb_style->border_color[i] = mb_rc_style->border_color[i];

  mb_style->shadow = mb_rc_style->shadow;
}

static void
moblin_netbook_style_copy (GtkStyle *dest,
                           GtkStyle *src)
{
  int i;
  MoblinNetbookStyle *mb_dest = MOBLIN_NETBOOK_STYLE (dest);
  MoblinNetbookStyle *mb_src = MOBLIN_NETBOOK_STYLE (src);

  mb_dest->radius = mb_src->radius;

  for (i = 0; i < 5; i++)
    mb_dest->border_color[i] = mb_src->border_color[i];

  mb_dest->shadow = mb_src->shadow;

  GTK_STYLE_CLASS (moblin_netbook_style_parent_class)->copy (dest, src);
}

static void
moblin_netbook_style_class_init (MoblinNetbookStyleClass *klass)
{
  GtkStyleClass *style_class = GTK_STYLE_CLASS (klass);

  style_class->init_from_rc = moblin_netbook_init_from_rc;
  style_class->copy = moblin_netbook_style_copy;

  style_class->draw_shadow = moblin_netbook_draw_shadow;
  style_class->draw_box = moblin_netbook_draw_box;
  style_class->draw_check = moblin_netbook_draw_check;
  style_class->draw_option = moblin_netbook_draw_option;
  style_class->draw_box_gap = moblin_netbook_draw_box_gap;
  style_class->draw_shadow_gap = moblin_netbook_draw_box_gap;
  style_class->draw_extension = moblin_netbook_draw_extension;
  style_class->draw_hline = moblin_netbook_draw_hline;
  style_class->draw_vline = moblin_netbook_draw_vline;
  style_class->draw_focus = moblin_netbook_draw_focus;
  style_class->draw_arrow = moblin_netbook_draw_arrow;
}

static void
moblin_netbook_style_class_finalize (MoblinNetbookStyleClass *klass)
{
}

static void
moblin_netbook_style_init (MoblinNetbookStyle *style)
{
}
