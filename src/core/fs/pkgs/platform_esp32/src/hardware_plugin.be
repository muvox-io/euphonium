import math 

class HardwarePlugin : Plugin
    var registered_drivers
    var selected_driver

    def init()
        self.registered_drivers = []
        self.apply_default_values()

        self.name = "hardware"
        self.display_name = "Hardware"
        self.type = "system"
        self.selected_driver = nil
        self.is_audio_output = true
    end

    def make_form(ctx, state)
        var drivers = []

        for driver : self.registered_drivers
            drivers.push(driver.name)
        end

        ctx.create_group('boardGroup', { 'label': 'Board' })
        board_names = []

        ctx.select_field('board', {
            'label': "Select your board type",
            'default': "custom",
            'group': 'boardGroup',
            'values': board_names,
            'type': 'number'
        })

        # if the user has selected a board, show a dialog asking if he wants to 
        # change the gpio settings to this board
        if state.find("board") != self.state.find("board") 
            ctx.modal_confirm("boardChanged", {
                    'label': "Board changed",
                    'hint': "The board you selected has changed to " + state.find("board") + ". Do you want to apply the new settings?",
                    'group': 'boardGroup',
                    'default': nil,
                    'okValue': "confirmed_" + state.find("board"),
                    'cancelValue': "cancelled_" + state.find("board")
            })
            if state.find("boardChanged") == "confirmed_" + state.find("board")
                state["boardChanged"] = "cancelled_" + state.find("board")
            end
        end

        ctx.create_group('driver', { 'label': 'DAC Driver' })
       

        ctx.select_field('dac', {
            'label': "Select DAC chip driver",
            'default': "dummy",
            'group': 'driver',
            'values': drivers,
            'type': 'number'
        })
        for driver : self.registered_drivers
            if driver.name == state['dac']
                print("Found driver", driver)
                print("driver.make_config_form", driver.make_config_form)
                if driver.datasheet_link != nil && driver.datasheet_link != ""
                    ctx.link_button('datasheet', {
                        'label': "Datasheet",
                        'link': driver.datasheet_link,
                        'group': 'driver',
                        'placeholder': 'PDF'
                    })
                end
                driver.make_config_form(ctx, state)
                break
            end
        end
        

     
    end

    # register driver implementation
    def register_driver(driver)
        self.registered_drivers.push(driver)
    end

    def select_driver(driver_name)
        if (self.selected_driver != nil)
            i2s.set_readable(false)
            self.selected_driver.unload_i2s()
        end

        for driver : self.registered_drivers
            if driver.name == driver_name
                self.selected_driver = driver
                self.selected_driver.state = self.state
                self.selected_driver.init_i2s()
                i2s.set_readable(true)
            end
        end
    end

    def init_audio()
    end

    def has_hardware_volume()
        if self.selected_driver != nil
            return self.selected_driver.hardware_volume_control
        end
        return false
    end

    def on_event(event, data)
        if event == EVENT_CONFIG_UPDATED
            self.select_driver(self.state['dac'])
        end

        if event == EVENT_VOLUME_UPDATED
            if self.selected_driver != nil
                self.selected_driver.set_volume(data)
            else
                print("Attempt to set volume on unselected driver")
            end
           
        end
    end
end

var hardware = HardwarePlugin()

euphonium.register_plugin(hardware)
