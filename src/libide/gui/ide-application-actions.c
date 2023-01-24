/* ide-application-actions.c
 *
 * Copyright 2018-2019 Christian Hergert <chergert@redhat.com>
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

#define G_LOG_DOMAIN "ide-application-addins"
#define DOCS_URI "https://builder.readthedocs.io"

#include "config.h"

#include <glib/gi18n.h>

#include <ide-build-ident.h>

#include <libide-projects.h>

#include "ide-application.h"
#include "ide-application-credits.h"
#include "ide-application-private.h"
#include "ide-application-tweaks.h"
#include "ide-gui-global.h"
#include "ide-primary-workspace.h"
#include "ide-support-private.h"

#include "ide-plugin-section-private.h"

static void
ide_application_actions_tweaks (GSimpleAction *action,
                                GVariant      *parameter,
                                gpointer       user_data)
{
  IdeApplication *self = user_data;
  const char *page = NULL;

  IDE_ENTRY;

  g_assert (IDE_IS_MAIN_THREAD ());
  g_assert (G_IS_SIMPLE_ACTION (action));
  g_assert (IDE_IS_APPLICATION (self));

  if (parameter && g_variant_is_of_type (parameter, G_VARIANT_TYPE_STRING))
    page = g_variant_get_string (parameter, NULL);

  ide_show_tweaks (NULL, page);

  IDE_EXIT;
}

static void
ide_application_actions_quit (GSimpleAction *action,
                              GVariant      *param,
                              gpointer       user_data)
{
  IdeApplication *self = user_data;

  IDE_ENTRY;

  g_assert (IDE_IS_APPLICATION (self));

  /* TODO: Ask all workbenches to cleanup */

  g_application_quit (G_APPLICATION (self));

  IDE_EXIT;
}

static void
ide_application_actions_about (GSimpleAction *action,
                               GVariant      *param,
                               gpointer       user_data)
{
  IdeApplication *self = user_data;
  g_autofree char *support_info = NULL;
  g_autofree char *support_filename = NULL;
  GtkDialog *dialog;
  GtkWindow *parent = NULL;
  GList *iter;
  GList *windows;

  g_assert (IDE_IS_APPLICATION (self));

  windows = gtk_application_get_windows (GTK_APPLICATION (self));

  for (iter = windows; iter; iter = iter->next)
    {
      if (IDE_IS_WORKSPACE (iter->data))
        {
          parent = iter->data;
          break;
        }
    }

  support_info = ide_get_support_log ();
  support_filename = g_strdup_printf ("gnome-builder-%u.log", (int)getpid ());

  dialog = g_object_new (ADW_TYPE_ABOUT_WINDOW,
                         "application-icon", ide_get_application_id (),
                         "application-name", _("Builder"),
                         "copyright", "© 2014–2023 Christian Hergert, et al.",
                         "debug-info", support_info,
                         "debug-info-filename", support_filename,
                         "designers", ide_application_credits_designers,
                         "developer-name", _("Christian Hergert, et al."),
                         "developers", ide_application_credits_developers,
                         "documenters", ide_application_credits_documenters,
                         "issue-url", "https://gitlab.gnome.org/GNOME/gnome-builder/-/issues/new",
                         "license-type", GTK_LICENSE_GPL_3_0,
                         "transient-for", parent,
                         "translator-credits", _("translator-credits"),
                         "version", PACKAGE_VERSION,
                         "website", "https://wiki.gnome.org/Apps/Builder",
                         NULL);
  adw_about_window_add_acknowledgement_section (ADW_ABOUT_WINDOW (dialog),
                                                _("Funded By"),
                                                ide_application_credits_funders);

  ide_gtk_window_present (GTK_WINDOW (dialog));
}

static void
ide_application_actions_help (GSimpleAction *action,
                              GVariant      *param,
                              gpointer       user_data)
{
  IdeApplication *self = user_data;
  gboolean ret = FALSE;

  IDE_ENTRY;

  g_assert (G_IS_SIMPLE_ACTION (action));
  g_assert (IDE_IS_APPLICATION (self));

  g_signal_emit_by_name (self, "show-help", &ret);

  IDE_EXIT;
}

static void
ide_application_actions_load_project (GSimpleAction *action,
                                      GVariant      *args,
                                      gpointer       user_data)
{
  IdeApplication *self = user_data;
  g_autoptr(IdeProjectInfo) project_info = NULL;
  g_autofree gchar *filename = NULL;
  g_autoptr(GFile) file = NULL;
  g_autofree gchar *scheme = NULL;

  g_assert (IDE_IS_APPLICATION (self));

  g_variant_get (args, "s", &filename);

  if ((scheme = g_uri_parse_scheme (filename)))
    file = g_file_new_for_uri (filename);
  else
    file = g_file_new_for_path (filename);

  project_info = ide_project_info_new ();
  ide_project_info_set_file (project_info, file);

  ide_application_open_project_async (self,
                                      project_info,
                                      G_TYPE_INVALID,
                                      NULL, NULL, NULL);
}

static gint
type_compare (gconstpointer a,
              gconstpointer b)
{
  GType *ta = (GType *)a;
  GType *tb = (GType *)b;

  return g_type_get_instance_count (*ta) - g_type_get_instance_count (*tb);
}

static void
ide_application_actions_stats (GSimpleAction *action,
                               GVariant *args,
                               gpointer user_data)
{
  guint n_types = 0;
  g_autofree GType *types = g_type_children (G_TYPE_OBJECT, &n_types);
  GtkScrolledWindow *scroller;
  GtkTextBuffer *buffer;
  GtkTextView *text_view;
  GtkWindow *window;
  gboolean found = FALSE;

  window = g_object_new (GTK_TYPE_WINDOW,
                         "default-width", 1000,
                         "default-height", 600,
                         "title", "about:types",
                         NULL);
  scroller = g_object_new (GTK_TYPE_SCROLLED_WINDOW,
                           "visible", TRUE,
                           NULL);
  gtk_window_set_child (window, GTK_WIDGET (scroller));
  text_view = g_object_new (GTK_TYPE_TEXT_VIEW,
                            "editable", FALSE,
                            "monospace", TRUE,
                            "visible", TRUE,
                            NULL);
  gtk_scrolled_window_set_child(scroller, GTK_WIDGET (text_view));
  buffer = gtk_text_view_get_buffer (text_view);

  gtk_text_buffer_insert_at_cursor (buffer, "Count | Type\n", -1);
  gtk_text_buffer_insert_at_cursor (buffer, "======+======\n", -1);

  qsort (types, n_types, sizeof (GType), type_compare);

  for (guint i = 0; i < n_types; i++)
    {
      gint count = g_type_get_instance_count (types[i]);

      if (count)
        {
          gchar str[12];

          found = TRUE;

          g_snprintf (str, sizeof str, "%6d", count);
          gtk_text_buffer_insert_at_cursor (buffer, str, -1);
          gtk_text_buffer_insert_at_cursor (buffer, " ", -1);
          gtk_text_buffer_insert_at_cursor (buffer, g_type_name (types[i]), -1);
          gtk_text_buffer_insert_at_cursor (buffer, "\n", -1);
        }
    }

  if (!found)
    gtk_text_buffer_insert_at_cursor (buffer, "No stats were found, was GOBJECT_DEBUG=instance-count set?", -1);

  ide_gtk_window_present (window);
}

static void
ide_application_actions_dark (GSimpleAction *action,
                              GVariant      *param,
                              gpointer       user_data)
{
  IdeApplication *self = user_data;
  g_settings_set_string (self->settings, "style-variant", "dark");
}

static void
ide_application_actions_light (GSimpleAction *action,
                               GVariant      *param,
                               gpointer       user_data)
{
  IdeApplication *self = user_data;
  g_settings_set_string (self->settings, "style-variant", "light");
}

static const GActionEntry IdeApplicationActions[] = {
  { "about:types", ide_application_actions_stats },
  { "about", ide_application_actions_about },
  { "load-project", ide_application_actions_load_project, "s"},
  { "preferences", ide_application_actions_tweaks },
  { "preferences-page", ide_application_actions_tweaks, "s" },
  { "quit", ide_application_actions_quit },
  { "help", ide_application_actions_help },
  { "dark", ide_application_actions_dark },
  { "light", ide_application_actions_light },
};

void
_ide_application_init_actions (IdeApplication *self)
{
  g_autoptr(GAction) style_action = NULL;
  g_autoptr(GAction) style_scheme_action = NULL;

  g_assert (IDE_IS_MAIN_THREAD ());
  g_assert (IDE_IS_APPLICATION (self));

  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   IdeApplicationActions,
                                   G_N_ELEMENTS (IdeApplicationActions),
                                   self);

  style_action = g_settings_create_action (self->settings, "style-variant");
  g_action_map_add_action (G_ACTION_MAP (self), style_action);

  style_scheme_action = g_settings_create_action (self->editor_settings, "style-scheme-name");
  g_action_map_add_action (G_ACTION_MAP (self), style_scheme_action);
}

static void
cancellable_weak_notify (gpointer  data,
                         GObject  *where_object_was)
{
  g_autofree char *name = data;
  g_action_map_remove_action (G_ACTION_MAP (IDE_APPLICATION_DEFAULT), name);
}

char *
ide_application_create_cancel_action (IdeApplication *self,
                                      GCancellable   *cancellable)
{
  static guint cancel_count;
  g_autofree char *action_name = NULL;
  g_autofree char *detailed_action_name = NULL;
  g_autoptr(GSimpleAction) action = NULL;
  guint count;

  g_return_val_if_fail (IDE_IS_APPLICATION (self), NULL);
  g_return_val_if_fail (G_IS_CANCELLABLE (cancellable), NULL);

  count = ++cancel_count;
  action_name = g_strdup_printf ("cancel_%u", count);
  detailed_action_name = g_strdup_printf ("app.cancel_%u", count);
  action = g_simple_action_new (action_name, NULL);
  g_signal_connect_object (action,
                           "activate",
                           G_CALLBACK (g_cancellable_cancel),
                           cancellable,
                           G_CONNECT_SWAPPED);
  g_object_weak_ref (G_OBJECT (cancellable),
                     cancellable_weak_notify,
                     g_steal_pointer (&action_name));
  g_action_map_add_action (G_ACTION_MAP (self), G_ACTION (action));

  return g_steal_pointer (&detailed_action_name);
}
