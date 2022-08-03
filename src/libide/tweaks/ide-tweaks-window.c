/* ide-tweaks-window.c
 *
 * Copyright 2022 Christian Hergert <chergert@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define G_LOG_DOMAIN "ide-tweaks-window"

#include "config.h"

#include "ide-tweaks-panel-private.h"
#include "ide-tweaks-panel-list-private.h"
#include "ide-tweaks-window.h"

struct _IdeTweaksWindow
{
  AdwWindow           parent_instance;

  IdeTweaks          *tweaks;

  GtkStack           *panel_stack;
  GtkStack           *panel_list_stack;
};

enum {
  PROP_0,
  PROP_TWEAKS,
  N_PROPS
};

G_DEFINE_FINAL_TYPE (IdeTweaksWindow, ide_tweaks_window, ADW_TYPE_WINDOW)

static GParamSpec *properties [N_PROPS];

static void
ide_tweaks_window_clear (IdeTweaksWindow *self)
{
  GtkWidget *child;

  g_assert (IDE_IS_TWEAKS_WINDOW (self));
  g_assert (IDE_IS_TWEAKS (self->tweaks));

  while ((child = gtk_widget_get_first_child (GTK_WIDGET (self->panel_list_stack))))
    gtk_stack_remove (self->panel_list_stack, child);

  while ((child = gtk_widget_get_first_child (GTK_WIDGET (self->panel_stack))))
    gtk_stack_remove (self->panel_stack, child);
}

static void
ide_tweaks_window_rebuild (IdeTweaksWindow *self)
{
  GtkWidget *list;

  g_assert (IDE_IS_TWEAKS_WINDOW (self));
  g_assert (IDE_IS_TWEAKS (self->tweaks));

  list = ide_tweaks_panel_list_new ();
  ide_tweaks_panel_list_set_item (IDE_TWEAKS_PANEL_LIST (list),
                                  IDE_TWEAKS_ITEM (self->tweaks));
  gtk_stack_add_named (self->panel_list_stack,
                       list,
                       ide_tweaks_item_get_id (IDE_TWEAKS_ITEM (self->tweaks)));
}

static void
ide_tweaks_window_dispose (GObject *object)
{
  IdeTweaksWindow *self = (IdeTweaksWindow *)object;

  if (self->tweaks)
    {
      ide_tweaks_window_clear (self);
      g_clear_object (&self->tweaks);
    }

  G_OBJECT_CLASS (ide_tweaks_window_parent_class)->dispose (object);
}

static void
ide_tweaks_window_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  IdeTweaksWindow *self = IDE_TWEAKS_WINDOW (object);

  switch (prop_id)
    {
    case PROP_TWEAKS:
      g_value_set_object (value, ide_tweaks_window_get_tweaks (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
ide_tweaks_window_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  IdeTweaksWindow *self = IDE_TWEAKS_WINDOW (object);

  switch (prop_id)
    {
    case PROP_TWEAKS:
      ide_tweaks_window_set_tweaks (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
ide_tweaks_window_class_init (IdeTweaksWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = ide_tweaks_window_dispose;
  object_class->get_property = ide_tweaks_window_get_property;
  object_class->set_property = ide_tweaks_window_set_property;

  properties [PROP_TWEAKS] =
    g_param_spec_object ("tweaks", NULL, NULL,
                         IDE_TYPE_TWEAKS,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/libide-tweaks/ide-tweaks-window.ui");
  gtk_widget_class_bind_template_child (widget_class, IdeTweaksWindow, panel_stack);
  gtk_widget_class_bind_template_child (widget_class, IdeTweaksWindow, panel_list_stack);

  g_type_ensure (IDE_TYPE_TWEAKS_PANEL);
  g_type_ensure (IDE_TYPE_TWEAKS_PANEL_LIST);
}

static void
ide_tweaks_window_init (IdeTweaksWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
ide_tweaks_window_new (void)
{
  return g_object_new (IDE_TYPE_TWEAKS_WINDOW, NULL);
}

/**
 * ide_tweaks_window_get_tweaks:
 * @self: a #IdeTweaksWindow
 *
 * Gets the tweaks property of the window.
 *
 * Returns: (transfer none) (nullable): an #IdeTweaks or %NULL
 */
IdeTweaks *
ide_tweaks_window_get_tweaks (IdeTweaksWindow *self)
{
  g_return_val_if_fail (IDE_IS_TWEAKS_WINDOW (self), NULL);

  return self->tweaks;
}

/**
 * ide_tweaks_window_set_tweaks:
 * @self: a #IdeTweaksWindow
 * @tweaks: (nullable): an #IdeTweaks
 *
 * Sets the tweaks to be displayed in the window.
 */
void
ide_tweaks_window_set_tweaks (IdeTweaksWindow *self,
                              IdeTweaks       *tweaks)
{
  g_return_if_fail (IDE_IS_TWEAKS_WINDOW (self));
  g_return_if_fail (!tweaks || IDE_IS_TWEAKS (tweaks));

  if (self->tweaks == tweaks)
    return;

  if (self->tweaks != NULL)
    {
      ide_tweaks_window_clear (self);
      g_clear_object (&self->tweaks);
    }

  if (tweaks != NULL)
    {
      g_set_object (&self->tweaks, tweaks);
      ide_tweaks_window_rebuild (self);
    }

  g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TWEAKS]);
}

/**
 * ide_tweaks_window_navigate_to:
 * @self: a #IdeTweaksWindow
 * @item: (nullable): an #IdeTweaksItem or %NULL
 *
 * Navigates to @item.
 *
 * If @item is %NULL and #IdeTweaksWindow:tweaks is set, then navigates
 * to the topmost item.
 */
void
ide_tweaks_window_navigate_to (IdeTweaksWindow *self,
                               IdeTweaksItem   *item)
{
  g_return_if_fail (IDE_IS_TWEAKS_WINDOW (self));
  g_return_if_fail (!item || IDE_IS_TWEAKS_ITEM (item));

  if (item == NULL)
    item = IDE_TWEAKS_ITEM (self->tweaks);

  if (item == NULL)
    return;

}