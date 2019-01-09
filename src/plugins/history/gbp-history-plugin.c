/* gbp-history-plugin.c
 *
 * Copyright 2017-2019 Christian Hergert <chergert@redhat.com>
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

#include <ide.h>
#include <libpeas/peas.h>

#include "gbp-history-editor-view-addin.h"
#include "gbp-history-layout-stack-addin.h"

void
gbp_history_register_types (PeasObjectModule *module)
{
  peas_object_module_register_extension_type (module,
                                              IDE_TYPE_EDITOR_VIEW_ADDIN,
                                              GBP_TYPE_HISTORY_EDITOR_VIEW_ADDIN);
  peas_object_module_register_extension_type (module,
                                              IDE_TYPE_LAYOUT_STACK_ADDIN,
                                              GBP_TYPE_HISTORY_LAYOUT_STACK_ADDIN);
}
