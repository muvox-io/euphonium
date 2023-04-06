class ResetRestorePlugin : Plugin 
  def init()
    self.name = "reset_restore"
    self.theme_color = "#1DB954"
    self.display_name = "Reset & Restore"
    self.type = "system"
    self.state = {}
    # self.fetch_config()
  end
  def make_form(ctx, state)
    ctx.create_group('reset', { 'label': 'Reset' })

    ctx.button_field('factoryResetButton', {
        'label': "Factory reset",
        'buttonText': "Reset",
        'group': 'reset',
    })
    if state.find("factoryResetButton") == true 
      state.setitem("factoryResetButton", false)
      state.setitem("factoryResetConfirm", nil)
      ctx.modal_confirm("factoryResetConfirm", {
        'label': "Factory reset",
        'hint': "Are you sure you want to reset euphonium to factory defaults?",
        'group': 'reset',
        'default': nil,
        'okValue': true,
        'cancelValue': false
      })
    end
    if state.find("factoryResetConfirm") == true 
      state.setitem("factoryResetConfirm", nil)
      self.perform_factory_reset()
    end

    # testing...
    ctx.modal_group("test_modal", {
      'title': "Test modal",
      'global': true,
    })
  end
  def perform_factory_reset()
    core.delete_config_files();
    wifi.clear_config();
    core.restart();
  end
end

var reset_restore = ResetRestorePlugin()

euphonium.register_plugin(reset_restore)
