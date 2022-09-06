/* gbp-rust-analyzer-service.c
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

#define G_LOG_DOMAIN "gbp-rust-analyzer-service"

#include "config.h"

#include <jsonrpc-glib.h>

#include <libide-threading.h>

#include "gbp-rust-analyzer-service.h"

struct _GbpRustAnalyzerService
{
  IdeLspService parent_instance;
};

G_DEFINE_FINAL_TYPE (GbpRustAnalyzerService, gbp_rust_analyzer_service, IDE_TYPE_LSP_SERVICE)

static void
gbp_rust_analyzer_service_configure_client (IdeLspService *service,
                                            IdeLspClient  *client)
{
  IDE_ENTRY;

  g_assert (GBP_IS_RUST_ANALYZER_SERVICE (service));
  g_assert (IDE_IS_LSP_CLIENT (client));

  ide_lsp_client_add_language (client, "rust");

  IDE_EXIT;
}

static void
gbp_rust_analyzer_service_configure_launcher (IdeLspService         *service,
                                              IdePipeline           *pipeline,
                                              IdeSubprocessLauncher *launcher)
{
  IDE_ENTRY;

  g_assert (GBP_IS_RUST_ANALYZER_SERVICE (service));
  g_assert (IDE_IS_PIPELINE (pipeline));
  g_assert (IDE_IS_SUBPROCESS_LAUNCHER (launcher));

  IDE_EXIT;
}

static void
gbp_rust_analyzer_service_class_init (GbpRustAnalyzerServiceClass *klass)
{
  IdeLspServiceClass *lsp_service_class = IDE_LSP_SERVICE_CLASS (klass);

  lsp_service_class->configure_client = gbp_rust_analyzer_service_configure_client;
  lsp_service_class->configure_launcher = gbp_rust_analyzer_service_configure_launcher;
}

static void
gbp_rust_analyzer_service_init (GbpRustAnalyzerService *self)
{
  ide_lsp_service_set_program (IDE_LSP_SERVICE (self), "rust-analyzer");
}
