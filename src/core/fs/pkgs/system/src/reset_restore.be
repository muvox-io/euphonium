class ResetRestorePlugin : Plugin 
  def init()
    self.name = "reset_restore"
    self.theme_color = "#1DB954"
    self.display_name = "Reset & Restore"
    self.type = "system"
    self.state = {
      'clearWifi': true,
    }
    # self.fetch_config()
  end
  def make_form(ctx, state)
    var group = ctx.create_group('reset', { 'label': 'Reset' })

    var factory_reset_btn = group.button_field('factoryResetButton', {
        'label': "Factory reset",
        'buttonText': "Reset",
    })

    if factory_reset_btn.has_been("click")
      state.setitem("show_factory_reset_modal", true)
    end

    if state.find("show_factory_reset_modal") == true 
      var modal_group = group.modal_group("factoryResetConfirm", {
        'title': "Factory reset",
      })
      modal_group.paragraph("factoryResetConfirmText", {
        'text': "Are you sure you want to reset the device to factory defaults?",
      })
      modal_group.checkbox_field('clearWifi', {
        'label': "Clear WiFi credentials",
        'default': true,
        'group': 'mqtt'
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
        self.perform_factory_reset(state.find("clearWifi"))
        ctx.request_redraw() # we need to redraw the form to remove the modal, otherwise it will be stuck
      end
    end
    if state.find("factory_reset_in_progress") == true
      group.text_field("factoryResetInProgressText", {
        'label': "Factory reset in progress",
        'default': "Factory reset in progress",
      })
    end
    var backup_group = ctx.create_group('reset', { 'label': 'Backup' })


    backup_group.link_button('backup', {
      'label': "Backup configuration to .tar file",
      'link': "/config_backup",
      'placeholder': 'Backup'
    })
    var backup_restore = backup_group.file_upload_field('backupRestore', {
      'label': "Restore configuration from .tar file",
      'buttonText': "Restore",
      'uploadEndpoint': "/config_backup",
    })
    if backup_restore.has_been("upload")
      core.restart();
    end
  end
  def perform_factory_reset(clearWifi)
    core.delete_config_files();
    if clearWifi == true
      wifi.clear_config();
    end
   
    core.restart();
  end
end

var reset_restore = ResetRestorePlugin()

euphonium.register_plugin(reset_restore)
