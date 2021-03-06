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


G_DEFINE_DYNAMIC_TYPE (MoblinNetbookRcStyle, moblin_netbook_rc_style,
                       GTK_TYPE_RC_STYLE)


enum
{
  TOKEN_BORDER_COLOR = G_TOKEN_LAST + 1,
  TOKEN_RADIUS,
  TOKEN_SHADOW,
};

static struct
{
  gchar *name;
  guint token;
}
moblin_rc_symbols[] =
{
  { "border", TOKEN_BORDER_COLOR },
  { "radius", TOKEN_RADIUS },
  { "shadow", TOKEN_SHADOW },
  { NULL, 0 }
};

static GtkStyle *
moblin_netbook_rc_style_create_style (GtkRcStyle *rc_style)
{
  return GTK_STYLE (g_object_new (MOBLIN_NETBOOK_TYPE_STYLE, NULL));
}

static guint
moblin_get_token (GScanner *scanner,
                  guint     expected)
{
  guint token;

  token = g_scanner_peek_next_token (scanner);
  if (token != expected)
    {
      return expected;
    }
  else
    {
      g_scanner_get_next_token (scanner);
      return G_TOKEN_NONE;
    }
}

static guint
moblin_parse_border_color (GScanner             *scanner,
                           MoblinNetbookRcStyle *rc_style)
{
  guint token;
  GtkStateType state_type;
  GdkColor color;

  /* border-color */
  g_scanner_get_next_token (scanner);


  /* [state] */
  token = gtk_rc_parse_state (scanner, &state_type);
  if (token != G_TOKEN_NONE)
    return token;

  /* = */
  token = moblin_get_token (scanner, G_TOKEN_EQUAL_SIGN);
  if (token != G_TOKEN_NONE)
    return token;

  token = gtk_rc_parse_color_full (scanner, (GtkRcStyle *) rc_style, &color);
  if (token != G_TOKEN_NONE)
    return token;

  rc_style->border_color[state_type] = color;
  rc_style->border_color_set[state_type] = TRUE;

  return G_TOKEN_NONE;
}

static guint
moblin_netbook_rc_style_parse (GtkRcStyle  *rc_style,
                               GtkSettings *settings,
                               GScanner    *scanner)
{
  GTokenType token;
  static GQuark scope_id;
  guint old_scope;
  int i;
  MoblinNetbookRcStyle *mb_style = MOBLIN_NETBOOK_RC_STYLE (rc_style);

  if (!scope_id)
    scope_id = g_quark_from_string ("moblin-gtk-engine");

  /* set the new scope and store the old scope id */
  old_scope = g_scanner_set_scope (scanner, scope_id);

  /* register the symbols if we haven't already done so */
  if (!g_scanner_lookup_symbol (scanner, moblin_rc_symbols[0].name))
    {
      for (i = 0; moblin_rc_symbols[i].name; i++)
        {
          g_scanner_scope_add_symbol (scanner,
                                      scope_id,
                                      moblin_rc_symbols[i].name,
                                      GINT_TO_POINTER (moblin_rc_symbols[i].
                                                       token));
        }
    }

  token = g_scanner_peek_next_token (scanner);
  while (token != G_TOKEN_RIGHT_CURLY)
    {
      switch (token)
        {
        case TOKEN_BORDER_COLOR:
          token = moblin_parse_border_color (scanner, mb_style);
          break;

        case TOKEN_RADIUS:
          g_scanner_get_next_token (scanner);

          token = moblin_get_token (scanner, G_TOKEN_EQUAL_SIGN);
          if (token != G_TOKEN_NONE)
            break;

          token = moblin_get_token (scanner, G_TOKEN_INT);
          if (token != G_TOKEN_NONE)
            break;

          mb_style->radius = scanner->value.v_int;
          break;

        case TOKEN_SHADOW:
          g_scanner_get_next_token (scanner);

          token = moblin_get_token (scanner, G_TOKEN_EQUAL_SIGN);
          if (token != G_TOKEN_NONE)
            break;

          token = moblin_get_token (scanner, G_TOKEN_FLOAT);
          if (token != G_TOKEN_NONE)
            break;

          mb_style->shadow = scanner->value.v_float;
          mb_style->shadow_set = TRUE;
          break;

        default:
          g_scanner_get_next_token (scanner);
          token = G_TOKEN_RIGHT_CURLY;
          break;

        }

      if (token != G_TOKEN_NONE)
        {
          return token;
        }

      token = g_scanner_peek_next_token (scanner);
    }

  g_scanner_get_next_token (scanner);
  g_scanner_set_scope (scanner, old_scope);
  return G_TOKEN_NONE;
}

static void
moblin_netbook_rc_style_merge (GtkRcStyle *adest,
                               GtkRcStyle *asrc)
{
  MoblinNetbookRcStyle *dest;
  MoblinNetbookRcStyle *src;
  gint state;

  /* chain up */
  GTK_RC_STYLE_CLASS (moblin_netbook_rc_style_parent_class)->merge (adest,
                                                                    asrc);

  /* we can't handle merge unless both src and dest are our class */
  if (!(MOBLIN_NETBOOK_IS_RC_STYLE (adest) &&
        MOBLIN_NETBOOK_IS_RC_STYLE (asrc)))
    {
      return;
    }

  dest = (MoblinNetbookRcStyle *) adest;
  src = (MoblinNetbookRcStyle *) asrc;

  if (!dest->radius_set)
    {
      dest->radius = src->radius;
      dest->radius_set = 1;
    }

  for (state = 0; state < 5; state++)
    {
      if (!dest->border_color_set[state] && src->border_color_set[state])
        {
          dest->border_color_set[state] = TRUE;
          dest->border_color[state] = src->border_color[state];
        }
    }

  if (!dest->shadow_set && src->shadow_set)
    {
      dest->shadow = src->shadow;
      dest->shadow_set = TRUE;
    }
}

static void
moblin_netbook_rc_style_class_init (MoblinNetbookRcStyleClass *klass)
{
  GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS (klass);

  rc_style_class->create_style = moblin_netbook_rc_style_create_style;
  rc_style_class->parse = moblin_netbook_rc_style_parse;
  rc_style_class->merge = moblin_netbook_rc_style_merge;
}

static void
moblin_netbook_rc_style_class_finalize (MoblinNetbookRcStyleClass *rc_style)
{
}

static void
moblin_netbook_rc_style_init (MoblinNetbookRcStyle *rc_style)
{
  /* set up defaults */
  rc_style->radius = 0;
  rc_style->shadow = 0;

  rc_style->radius_set = 0;
}

void
_moblin_netbook_rc_style_register_type (GTypeModule *module)
{
  moblin_netbook_rc_style_register_type (module);
}
