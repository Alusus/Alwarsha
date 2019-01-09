/* ide-layout-grid-column.h
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

#pragma once

#include <dazzle.h>

#include "ide-version-macros.h"

#include "layout/ide-layout-stack.h"

G_BEGIN_DECLS

#define IDE_TYPE_LAYOUT_GRID_COLUMN (ide_layout_grid_column_get_type())

IDE_AVAILABLE_IN_3_32
G_DECLARE_FINAL_TYPE (IdeLayoutGridColumn, ide_layout_grid_column, IDE, LAYOUT_GRID_COLUMN, DzlMultiPaned)

IDE_AVAILABLE_IN_3_32
GtkWidget      *ide_layout_grid_column_new               (void);
IDE_AVAILABLE_IN_3_32
IdeLayoutStack *ide_layout_grid_column_get_current_stack (IdeLayoutGridColumn *self);
IDE_AVAILABLE_IN_3_32
void            ide_layout_grid_column_set_current_stack (IdeLayoutGridColumn *self,
                                                          IdeLayoutStack      *stack);

G_END_DECLS
