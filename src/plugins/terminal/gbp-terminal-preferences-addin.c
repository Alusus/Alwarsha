/* gbp-terminal-preferences-addin.c
 *
 * Copyright 2021 Christian Hergert <chergert@redhat.com>
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

#define G_LOG_DOMAIN "gbp-terminal-preferences-addin"

#include "config.h"

#include <glib/gi18n.h>

#include "gbp-terminal-preferences-addin.h"

struct _GbpTerminalPreferencesAddin
{
  GObject parent_instance;
  guint limit_id;
  guint lines_id;
  guint scroll_on_output_id;
  guint scroll_on_keystroke_id;
  guint font_id;
  guint allow_bold_id;
  guint allow_hyperlink_id;
};

static void
gbp_terminal_preferences_addin_load (IdePreferencesAddin *addin,
                                     DzlPreferences      *preferences)
{
  GbpTerminalPreferencesAddin *self = (GbpTerminalPreferencesAddin *)addin;

  g_assert (GBP_IS_TERMINAL_PREFERENCES_ADDIN (self));
  g_assert (DZL_IS_PREFERENCES (preferences));

  dzl_preferences_add_page (preferences, "terminal", _("Terminal"), 100);
  dzl_preferences_add_list_group (preferences, "terminal", "scrollback", _("Scrollback"), GTK_SELECTION_NONE, 10);
  dzl_preferences_add_list_group (preferences, "terminal", "general", _("General"), GTK_SELECTION_NONE, 0);

  self->font_id = dzl_preferences_add_font_button (preferences,
                                                   "terminal",
                                                   "general",
                                                   "org.alusus.alwarsha.terminal",
                                                   "font-name",
                                                   _("Terminal Font"),
                                                   C_("Keywords", "terminal font monospace"),
                                                   1);

  self->allow_bold_id = dzl_preferences_add_switch (preferences,
                                                    "terminal",
                                                    "general",
                                                    "org.alusus.alwarsha.terminal",
                                                    "allow-bold",
                                                    NULL,
                                                    NULL,
                                                    _("Bold text in terminals"),
                                                    _("If terminals are allowed to display bold text"),
                                                    C_("Keywords", "terminal allow bold"),
                                                    2);
  self->allow_hyperlink_id = dzl_preferences_add_switch (preferences,
                                                         "terminal",
                                                         "general",
                                                         "org.alusus.alwarsha.terminal",
                                                         "allow-hyperlink",
                                                         NULL,
                                                         NULL,
                                                         _("Show hyperlinks"),
                                                         _("When enabled hyperlinks (OSC 8 escape sequences) are recognized and displayed"),
                                                         C_("Keywords", "terminal show hyperlinks links urls"),
                                                         3);

  self->scroll_on_output_id = dzl_preferences_add_switch (preferences,
                                                          "terminal",
                                                          "scrollback",
                                                          "org.alusus.alwarsha.terminal",
                                                          "scroll-on-output",
                                                          NULL,
                                                          NULL,
                                                          _("Scroll on output"),
                                                          _("When enabled the terminal will scroll to the bottom when new output is displayed"),
                                                          C_("Keywords", "scroll on output"),
                                                          0);
  self->scroll_on_keystroke_id = dzl_preferences_add_switch (preferences,
                                                             "terminal",
                                                             "scrollback",
                                                             "org.alusus.alwarsha.terminal",
                                                             "scroll-on-keystroke",
                                                             NULL,
                                                             NULL,
                                                             _("Scroll on keystroke"),
                                                             _("When enabled the terminal will scroll to the bottom when typing"),
                                                             C_("Keywords", "scroll on keystroke"),
                                                             10);
  self->limit_id = dzl_preferences_add_switch (preferences,
                                               "terminal",
                                               "scrollback",
                                               "org.alusus.alwarsha.terminal",
                                               "limit-scrollback",
                                               NULL,
                                               NULL,
                                               _("Limit Scrollback"),
                                               _("When enabled terminal scrollback will be limited to the number of lines specified below"),
                                               C_("Keywords", "scrollback limit"),
                                               20);
  self->lines_id = dzl_preferences_add_spin_button (preferences,
                                                    "terminal",
                                                    "scrollback",
                                                    "org.alusus.alwarsha.terminal",
                                                    "scrollback-lines",
                                                    NULL,
                                                    _("Scrollback Lines"),
                                                    _("The number of lines to keep available for scrolling"),
                                                    C_("Keywords", "scrollback lines"),
                                                    30);
}

static void
gbp_terminal_preferences_addin_unload (IdePreferencesAddin *addin,
                                       DzlPreferences      *preferences)
{
  GbpTerminalPreferencesAddin *self = (GbpTerminalPreferencesAddin *)addin;

  g_assert (GBP_IS_TERMINAL_PREFERENCES_ADDIN (self));
  g_assert (DZL_IS_PREFERENCES (preferences));

  dzl_preferences_remove_id (preferences, self->limit_id);
  dzl_preferences_remove_id (preferences, self->lines_id);
  dzl_preferences_remove_id (preferences, self->scroll_on_keystroke_id);
  dzl_preferences_remove_id (preferences, self->scroll_on_output_id);
  dzl_preferences_remove_id (preferences, self->allow_bold_id);
  dzl_preferences_remove_id (preferences, self->font_id);
}

static void
preferences_addin_iface_Init (IdePreferencesAddinInterface *iface)
{
  iface->load = gbp_terminal_preferences_addin_load;
  iface->unload = gbp_terminal_preferences_addin_unload;
}

G_DEFINE_FINAL_TYPE_WITH_CODE (GbpTerminalPreferencesAddin, gbp_terminal_preferences_addin, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (IDE_TYPE_PREFERENCES_ADDIN,
                                                preferences_addin_iface_Init))

static void
gbp_terminal_preferences_addin_class_init (GbpTerminalPreferencesAddinClass *klass)
{
}

static void
gbp_terminal_preferences_addin_init (GbpTerminalPreferencesAddin *self)
{
}
