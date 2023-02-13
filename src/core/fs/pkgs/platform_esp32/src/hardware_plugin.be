import math 

class HardwarePlugin : Plugin
    var registered_drivers
    var selected_driver

    def init()
        self.registered_drivers = []
        self.name = "hardware"
        self.display_name = "Hardware"
        self.type = "system"

        self.apply_default_values()
        self.fetch_config()

        self.selected_driver = nil
        self.is_audio_output = true

        http.handle(HTTP_GET, '/system/audio_hardware', def (request)
            request.write_json(self.get_hardware_status(), 200)
        end)
        
        http.handle(HTTP_POST, '/system/audio_hardware', def (request)
            var body = request.json_body()
            if body.find('max_volume') != nil
                var max_volume = body['max_volume']
                if max_volume >= 0 && max_volume <= 100
                    general_settings.set_max_volume_tuned(max_volume)
                    self.tune_max_volume(max_volume)
                end
            end

            request.write_json(self.get_hardware_status(), 200)
        end)
    end

    def get_hardware_status()
        if self.selected_driver != nil
            return {
                'driver': self.selected_driver.name,
                'driver_type': self.selected_driver.type,
                'hardware_volume': self.selected_driver.hardware_volume_control,
                'volume_table': self.selected_driver.get_volume_table(),
                'tuned': general_settings.is_max_volume_tuned()
            }
        end
    end

    def make_form(ctx, state)
        var drivers = []

        for driver : self.registered_drivers
            drivers.push(driver.name)
        end

        ctx.create_group('boardGroup', { 'label': 'Board' })

        board_names = []
        for board : ESP32_BOARDS
            board_names.push(board["name"])
        end

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
                for board : ESP32_BOARDS
                    if board["name"] == state.find("board")
                        # copy the state defined for the board
                        for key : board["state"].keys()
                            state.setitem(key, board["state"][key])
                        end
                        break
                    end
                end
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

    def tune_max_volume(max_vol)
        # only applies the tuning to amplifiers
        if self.selected_driver != nil && self.selected_driver.type == DAC_DRIVER_AMPLIFIER
            # Calculate index of upper limit of volume table
            var max_index = int((self.selected_driver.default_volume_table.size() * (real(max_vol) / 100)) + 0.5)

            # Store a new volume table with the new upper limit
            self.state['volume_table'] = json.dump(self.selected_driver.default_volume_table[0..max_index])
            self.selected_driver.state = self.state

            self.persist_config()
        end
    end

    def autodetect_hardware()
        for board : ESP32_BOARDS
            if board["name"] == "μVox"
                for key : board["state"].keys()
                    self.state.setitem(key, board["state"][key])
                end
                break
            end
        end
    end

    def on_event(event, data)
        if event == EVENT_CONFIG_UPDATED || event == EVENT_PLUGIN_INIT
            self.autodetect_hardware()
            if self.state.find('dac') != nil
                self.select_driver(self.state['dac'])
            end
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
