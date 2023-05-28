class DevtoolsPlugin : Plugin
  def init()
    self.name = "devtools"
    self.theme_color = "#1DB954"
    self.display_name = "Devtools"
    self.type = "system"
    self.state = {}
    # self.fetch_config()
  end

  def member(name)
    return get_native('devtools', name)
  end

  def fetch_task_stats_table()
    stats = self.get_task_stats(1000)
  
    var taskData = []
    for task : stats
      taskData.push([task['name'], str(task['runtime']), str(task['percentage']) + '%', str(task['core'])])
    end
    return {
      'columns': [
        {
          'title': 'Task',
          'align': 'left'
        },
        {
          'title': 'Time',
          'align': 'center'
        },
        {
          'title': 'Usage',
          'align': 'center'
        },
        {
          'title': 'Core ID',
          'align': 'center'
        }
      ],
      'data': taskData
    }
  end

  def make_form(ctx, state)
    var group = ctx.create_group('devtools', { 'label': 'Devtools' })

    var system_load_btn = group.button_field('cpuStatsBtn', {
        'label': "Get CPU statistics",
        'buttonText': "Measure",
    })

    if system_load_btn.has_been("click")
      state.setitem("show_cpu_stats", true)
    end

    if state.find("show_cpu_stats") == true 

      var modal_group = group.modal_group("cpuStats", {
        'title': "System usage statistics (last 1000ms)",
      })
      if modal_group.has_been("dismiss")
        state.setitem("show_cpu_stats", nil)
        ctx.request_redraw() # we need to redraw the form to remove the modal, otherwise it will be stuck
      else
        var data = self.fetch_task_stats_table()
        modal_group.table("system_load", data)
      end
    end
  end
end

var devtools_plugin = DevtoolsPlugin()
euphonium.register_plugin(devtools_plugin)
