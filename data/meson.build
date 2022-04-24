subdir('appdata')
subdir('fonts')
subdir('gsettings')
subdir('icons')
subdir('language-specs')
subdir('style-schemes')

# Desktop launcher and description file.
desktop_file = i18n.merge_file(
  input: 'org.alusus.Alwarsha.desktop.in.in',
  output: 'org.alusus.Alwarsha.desktop',
  type: 'desktop',
  po_dir: '../po',
  install: true,
  install_dir: join_paths(get_option('datadir'), 'applications'),
)

desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils,
    args: [desktop_file]
  )
endif

# D-Bus service file.
dbusconf = configuration_data()
dbusconf.set('bindir', join_paths(get_option('prefix'), get_option('bindir')))
configure_file(
	input: 'org.alusus.Alwarsha.service.in',
	output: 'org.alusus.Alwarsha.service',
	configuration: dbusconf,
	install: true,
	install_dir: join_paths(get_option('datadir'), 'dbus-1', 'services'),
)
