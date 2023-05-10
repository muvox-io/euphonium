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
    var group = ctx.create_group('reset', { 'label': 'Reset' })

    var btn = group.button_field('factoryResetButton', {
        'label': "Factory reset",
        'buttonText': "Reset",
    })

    if btn.has_been("click")
      state.setitem("show_factory_reset_modal", true)
    end
    if state.find("show_factory_reset_modal") == true 
      var modal_group = group.modal_group("factoryResetConfirm", {
        'title': "Factory reset",
      })
      modal_group.paragraph("factoryResetConfirmText", {
        'text': "Are you sure you want to reset the device to factory defaults?",
      })
      var confirm_button = modal_group.button_field("factoryResetConfirmButton", {
        'label': "Confirm",
        'buttonText': "Confirm",
      })
      var cancel_button = modal_group.button_field("factoryResetCancelButton", {
        'label': "Cancel",
        'buttonText': "Cancel",
      })
      if cancel_button.has_been("click") || modal_group.has_been("dismiss")
        state.setitem("show_factory_reset_modal", nil)
        ctx.request_redraw() # we need to redraw the form to remove the modal, otherwise it will be stuck
      end
      if confirm_button.has_been("click")
        state.setitem("show_factory_reset_modal", nil)
        state.setitem("factory_reset_in_progress", true)
        self.perform_factory_reset()
        ctx.request_redraw() # we need to redraw the form to remove the modal, otherwise it will be stuck
      end
    end
    if state.find("factory_reset_in_progress") == true
      group.text_field("factoryResetInProgressText", {
        'label': "Factory reset in progress",
        'default': "Factory reset in progress",
      })
    end
  end
  def perform_factory_reset()
    core.delete_config_files();
    wifi.clear_config();
    core.restart();
  end
end

var reset_restore = ResetRestorePlugin()

euphonium.register_plugin(reset_restore)
