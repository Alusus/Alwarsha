#!/usr/bin/env python3
# __init__.py
#
# Copyright 2022 Sarmad Abdullah <sarmad@alusus.org>
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

import gi
import os
import subprocess
from os import path

from gi.repository import (
    Ide,
    Gio,
    GLib,
    GObject,
    GtkSource,
    Template,
)

_ = Ide.gettext

class AlususBuildSystemDiscovery(Ide.SimpleBuildSystemDiscovery):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.props.glob = '+(.alwarsha|main.alusus|src/main.alusus|.الورشة|بداية.أسس|الشفرة/بداية.أسس)'
        self.props.hint = 'alusus_plugin'
        self.props.priority = 100

class AlususBuildSystem(Ide.Object, Ide.BuildSystem):
    project_file = GObject.Property(type=Gio.File)

    def do_get_id(self):
        return 'alusus'

    def do_get_display_name(self):
        return 'Alusus'

    def do_get_priority(self):
        return 0

class AlususPipelineAddin(Ide.Object, Ide.PipelineAddin):
    """
    The MakePipelineAddin registers stages to be executed when various
    phases of the build pipeline are requested.
    """

    def do_load(self, pipeline):
        context = pipeline.get_context()
        build_system = Ide.BuildSystem.from_context(context)

class AlususBuildTarget(Ide.Object, Ide.BuildTarget):
    def __init__(self, script, **kw):
        super().__init__(**kw)
        self._script = script

    def do_get_install_directory(self):
        return None

    def do_get_display_name(self):
        return self._script

    def do_get_name(self):
        return self._script

    def do_get_language(self):
        return 'alusus'

    def do_get_cwd(self):
        context = self.get_context()
        return context.ref_workdir().get_path()

    def do_get_argv(self):
        return ["alusus", self._script]

    def do_get_priority(self):
        return 0

class AlususBuildTargetProvider(Ide.Object, Ide.BuildTargetProvider):
    """
    The MakeBuildTargetProvider just wraps a "make run" command. If the
    Makefile doesn't have a run target, we'll just fail to execute and the user
    should get the warning in their application output (and can update their
    Makefile appropriately).
    """

    def do_get_targets_async(self, cancellable, callback, data):
        task = Gio.Task.new(self, cancellable, callback)
        task.set_priority(GLib.PRIORITY_LOW)

        context = self.get_context()
        build_system = Ide.BuildSystem.from_context(context)

        srcdir = context.ref_workdir().get_path()
        filePaths = [
            "/main.alusus",
            "/بداية.أسس",
            "/src/main.alusus",
            "/الشفرة/بداية.أسس"
        ]
        for path in filePaths:
            if os.path.isfile(srcdir+path):
                task.targets = [AlususBuildTarget(srcdir + path)]
                break

        task.return_boolean(True)

    def do_get_targets_finish(self, result):
        if result.propagate_boolean():
            return result.targets


class AlususTemplateProvider(GObject.Object, Ide.TemplateProvider):
    def do_get_project_templates(self):
        return [
            AlususEmptyProjectTemplate(_('Alusus Project - Arabic'), _('Create an Arabic based Alusus project'), "ar"),
            AlususEmptyProjectTemplate(_('Alusus Project - English'), _('Create an English based Alusus project'), "en")
        ]

class AlususTemplateLocator(Template.TemplateLocator):
    license = None

    def empty(self):
        return Gio.MemoryInputStream()

    def do_locate(self, path):
        if path.startswith('license.'):
            filename = GLib.basename(path)
            manager = GtkSource.LanguageManager.get_default()
            language = manager.guess_language(filename, None)

            if self.license is None or language is None:
                return self.empty()

            header = Ide.language_format_header(language, self.license)
            gbytes = GLib.Bytes(header.encode())

            return Gio.MemoryInputStream.new_from_bytes(gbytes)

        return super().do_locate(self, path)


class AlususTemplate(Ide.TemplateBase, Ide.ProjectTemplate):
    def __init__(self, id, name, icon_name, description, languages, priority):
        super().__init__()
        self.id = id
        self.name = name
        self.icon_name = icon_name
        self.description = description
        self.languages = languages
        self.priority = priority
        self.locator = AlususTemplateLocator()

        self.props.locator = self.locator

    def do_get_id(self):
        return self.id

    def do_get_name(self):
        return self.name

    def do_get_icon_name(self):
        return self.icon_name

    def do_get_description(self):
        return self.description

    def do_get_languages(self):
        return self.languages

    def do_get_priority(self):
        return self.priority

    def do_expand_async(self, params, cancellable, callback, data):
        self.reset()

        task = Ide.Task.new(self, cancellable, callback)

        if 'language' in params:
            self.language = params['language'].get_string().lower()
        else:
            self.language = 'alusus'

        if self.language != 'alusus':
            task.return_error(GLib.Error('Language %s not supported' % self.language))
            return

        if 'versioning' in params:
            self.versioning = params['versioning'].get_string()
        else:
            self.versioning = ''

        if 'author' in params:
            author_name = params['author'].get_string()
        else:
            author_name = GLib.get_real_name()

        scope = Template.Scope.new()
        scope.get('template').assign_string(self.id)

        name = params['name'].get_string().lower()
        name_ = ''.join([c if c.isalnum() else '_' for c in name])
        scope.get('name').assign_string(name)
        scope.get('name_').assign_string(name_)
        scope.get('NAME').assign_string(name_.upper())

        if 'app-id' in params:
            appid = params['app-id'].get_string()
        else:
            appid = 'org.example.App'
        appid_path = '/' + appid.replace('.', '/')
        scope.get('appid').assign_string(appid)
        scope.get('appid_path').assign_string(appid_path)

        prefix = name_ if not name_.endswith('_glib') else name_[:-5]
        PREFIX = prefix.upper()
        prefix_ = prefix.lower()
        PreFix = ''.join([word.capitalize() for word in prefix.lower().split('_')])

        scope.get('prefix').assign_string(prefix)
        scope.get('Prefix').assign_string(prefix.capitalize())
        scope.get('PreFix').assign_string(PreFix)
        scope.get('prefix_').assign_string(prefix_)
        scope.get('PREFIX').assign_string(PREFIX)


        scope.get('project_version').assign_string('0.1.0')
        scope.get('language').assign_string(self.language)
        scope.get('author').assign_string(author_name)

        exec_name = name
        scope.get('exec_name').assign_string(exec_name)

        modes = {
        }

        expands = {
            'prefix': prefix,
            'appid': appid,
            'name_': name_,
            'name': name,
            'exec_name': exec_name,
        }

        files = {
            # Build files

        }
        self.prepare_files(files)

        # No explicit license == proprietary
        spdx_license = 'LicenseRef-proprietary'

        # https://spdx.org/licenses/
        LICENSE_TO_SPDX = {
            'agpl_3': 'AGPL-3.0-or-later',
            'gpl_3': 'GPL-3.0-or-later',
            'lgpl_2_1': 'LGPL-2.1-or-later',
            'lgpl_3': 'LGPL-3.0-or-later',
            'mit_x11': 'MIT',
        }

        if 'license_full' in params:
            license_full_path = params['license_full'].get_string()
            files[license_full_path] = 'COPYING'

        if 'license_short' in params:
            license_short_path = params['license_short'].get_string()
            license_base = Gio.resources_lookup_data(license_short_path[11:], 0).get_data().decode()
            self.locator.license = license_base
            license_name = license_short_path.rsplit('/', 1)[1]
            spdx_license = LICENSE_TO_SPDX.get(license_name, '')

        scope.get('project_license').assign_string(spdx_license)

        if 'path' in params:
            dir_path = params['path'].get_string()
        else:
            dir_path = name
        directory = Gio.File.new_for_path(dir_path)
        scope.get('project_path').assign_string(directory.get_path())

        for src, dst in files.items():
            destination = directory.get_child(dst % expands)
            if src.startswith('resource://'):
                self.add_resource(src[11:], destination, scope, modes.get(src, 0))
            else:
                path = os.path.join('/plugins/alusus', src)
                self.add_resource(path, destination, scope, modes.get(src, 0))

        self.expand_all_async(cancellable, self.expand_all_cb, task)

    def do_expand_finish(self, result):
        return result.propagate_boolean()

    def expand_all_cb(self, obj, result, task):
        try:
            self.expand_all_finish(result)
            task.return_boolean(True)
        except Exception as exc:
            if isinstance(exc, GLib.Error):
                task.return_error(exc)
            else:
                task.return_error(GLib.Error(repr(exc)))




class AlususEmptyProjectTemplate(AlususTemplate):
    def __init__(self, title, description, lang):
        super().__init__(
            'empty-alusus',
            title,
            'pattern-empty',
            description,
            ['alusus'],
            200
        )
        self.lang = lang

    def prepare_files(self, files):
        if self.lang=='ar':
            files['resources/الشفرة/بداية.أسس'] = 'الشفرة/بداية.أسس'
        else:
            files['resources/src/main.alusus'] = 'src/main.alusus'