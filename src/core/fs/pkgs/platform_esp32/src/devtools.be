import json

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
    var group = ctx.create_group('statistics', { 'label': 'Statistics' })

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
    stats = memory_monitor.get_memory_stats()
  
    psram_percent = (real(stats["psram_used"]) / real(stats["psram_total"])) * 100
    internal_percent = (real(stats["internal_ram_used"]) / real(stats["internal_ram_total"])) * 100
    group.paragraph("memoryStatsPsram", {
      'text': "PSRAM: " + str(psram_percent) + "% used (" + str(stats["psram_used"]) + " bytes)"
    })
    
    group.graph("psramGraph", {
      'data': stats["psram_history"],
      'max_value': stats["psram_total"],
      'reverse_data': true,
      'label': 'PSRAM'
    })

    group.paragraph("memoryStatsInternalRam", {
      'text': "Internal RAM: " + str(internal_percent) + "% used (" + str(stats["internal_ram_used"]) + " bytes)"
    })

    group.graph("internalGraph", {
      'data': stats["internal_ram_history"],
      'max_value': stats["internal_ram_total"],
      'reverse_data': true,
      'label': 'Internal RAM'
    })

    if state.find("show_cpu_stats") == true 
      ctx.refresh_interval = 0
    else
      ctx.refresh_interval = 2000
    end
    
    var testing_group = ctx.create_group('testing', { 'label': 'Testing' })

    var trip_emergency_mode_btn = testing_group.button_field('tripEmergencyModeBtn', {
        'label': "Trip emergency mode",
        'buttonText': "Trip",
    })

    if trip_emergency_mode_btn.has_been("click")
      core.trip_emergency_mode("Tripped from devtools page")
    end

  end
end

var devtools_plugin = DevtoolsPlugin()
euphonium.register_plugin(devtools_plugin)
