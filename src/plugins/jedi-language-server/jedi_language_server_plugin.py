#!/usr/bin/env python3

# jedi_language_server_plugin.py
#
# Copyright 2021 Günther Wagner <info@gunibert.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# SPDX-License-Identifier: GPL-3.0-or-later

import gi

from gi.repository import GLib
from gi.repository import Gio
from gi.repository import GObject
from gi.repository import Ide

class JediService(Ide.LspService):
    def do_constructed(self):
        self.set_inherit_stderr(True)

    def do_configure_launcher(self, launcher):
        launcher.push_argv("jedi-language-server")

    def do_configure_client(self, client):
        client.add_language('python')
        client.add_language('python3')

class JediDiagnosticProvider(Ide.LspDiagnosticProvider, Ide.DiagnosticProvider):
    def do_load(self):
        JediService.bind_client(self)

class JediCompletionProvider(Ide.LspCompletionProvider, Ide.CompletionProvider):
    def do_load(self, context):
        Ide.debug("load completion provider")
        JediService.bind_client(self)

    def do_get_priority(self, context):
        # This provider only activates when it is very likely that we
        # want the results. So use high priority (negative is better).
        return -1000

class JediSymbolResolver(Ide.LspSymbolResolver, Ide.SymbolResolver):
    def do_load(self):
        JediService.bind_client(self)

class JediHighlighter(Ide.LspHighlighter, Ide.Highlighter):
    def do_load(self):
        JediService.bind_client(self)

class JediFormatter(Ide.LspFormatter, Ide.Formatter):
    def do_load(self):
        JediService.bind_client(self)

class JediHoverProvider(Ide.LspHoverProvider, Ide.HoverProvider):
    def do_prepare(self):
        self.props.category = 'Python'
        self.props.priority = 200
        JediService.bind_client(self)

class JediRenameProvider(Ide.LspRenameProvider, Ide.RenameProvider):
    def do_load(self):
        JediService.bind_client(self)

class JediCodeActionProvider(Ide.LspCodeActionProvider, Ide.CodeActionProvider):
    def do_load(self):
        JediService.bind_client(self)

