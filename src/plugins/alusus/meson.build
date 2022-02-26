meson_templates_resources = gnome.compile_resources(
  'alusus_plugin',
  'alusus.gresource.xml',
  gresource_bundle: true,
           install: true,
       install_dir: plugindir,
)

install_data('alusus_plugin.py', install_dir: plugindir)

configure_file(
          input: 'alusus.plugin',
         output: 'alusus.plugin',
  configuration: config_h,
        install: true,
    install_dir: plugindir,
)
