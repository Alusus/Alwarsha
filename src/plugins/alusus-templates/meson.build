meson_templates_resources = gnome.compile_resources(
  'alusus_templates',
  'alusus-templates.gresource.xml',
  gresource_bundle: true,
           install: true,
       install_dir: plugindir,
)

install_data('alusus_templates.py', install_dir: plugindir)

configure_file(
          input: 'alusus-templates.plugin',
         output: 'alusus-templates.plugin',
  configuration: config_h,
        install: true,
    install_dir: plugindir,
)
