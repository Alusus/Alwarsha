/* ide-completion-types.h
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

#pragma once

#include "ide-types.h"

G_BEGIN_DECLS

typedef enum
{
  IDE_COMPLETION_INTERACTIVE,
  IDE_COMPLETION_USER_REQUESTED,
  IDE_COMPLETION_TRIGGERED,
} IdeCompletionActivation;

typedef enum
{
  IDE_COMPLETION_COLUMN_ICON,
  IDE_COMPLETION_COLUMN_LEFT_OF,
  IDE_COMPLETION_COLUMN_TYPED_TEXT,
  IDE_COMPLETION_COLUMN_RIGHT_OF,
} IdeCompletionColumn;

G_END_DECLS
