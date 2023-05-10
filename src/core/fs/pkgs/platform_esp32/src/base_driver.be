import math

var DAC_DRIVER_DEFAULT = "dac"
var DAC_DRIVER_AMPLIFIER = "amplifier"

class DACDriver
    var hardware_volume_control
    var state
    var name
    var signedness
    var datasheet_link
    var default_volume_table
    var type

    def init()
        self.type = DAC_DRIVER_DEFAULT
        self.default_volume_table = []
        self.hardware_volume_control = false
    end

    def get_gpio(pin)
        return int(self.state[pin])
    end

    def get_i2s_pins()
        var pins_config = I2SPinsConfig()
        pins_config.mck_io = 0
        pins_config.bck_io = self.get_gpio('bck')
        pins_config.ws_io = self.get_gpio('ws')
        pins_config.data_out_io = self.get_gpio('data')

        return pins_config
    end

    # initializes the DAC driver
    def init_i2s()
    end

    # disables I2S
    def unload_i2s()
    end

    # sets the volume of the DAC
    def set_volume(volume)
    end

    def make_config_form(ctx, state)
        var i2s_group = ctx.create_group('i2s', { 'label': 'I2S GPIO ddd' })
        var i2c_group = ctx.create_group('i2c', { 'label': 'I2C GPIO' })

        i2s_group.text_field('volume_table', {
            'label': "Volume table",
            'default': json.dump(self.default_volume_table)
        })
        
        i2s_group.number_field('bck', {
            'label': "BCK",
            'default': 0,

        })

        i2s_group.number_field('ws', {
            'label': "WS",
            'default': 0,
        })

        i2s_group.number_field('data', {
            'label': "DATA",
            'default': 0,
        })

        i2s_group.number_field('mclk', {
            'label': "MCLK",
            'default': 0,
        })

        i2c_group.number_field('sda', {
            'label': "SDA",
            'default': 0,
        })

        i2c_group.number_field('scl', {
            'label': "SCL",
            'default': 0,
        })
        var i2c_scan_button = i2c_group.button_field("i2c_scan_button", {
            'label': "",
            'buttonText': "Scan I2C bus",
        })
        
        if i2c_scan_button.has_been("click")
            
            if !i2c.is_installed()
               i2c.install(self.get_gpio('sda'), self.get_gpio('scl'))
            end
            var results = []
            for i : 0..127
                results.push(i2c.detect(i))
            end
            state.setitem("i2c_scan_result", results)
        end

        if state.find("i2c_scan_result") != nil
            var modal_group = i2c_group.modal_group("i2c_scan_modal", {
                'title': "I2C scan",
            })
            var i = 0
            var curr_row = ""
            for res : state['i2c_scan_result']
                var txt = ""
                if res 
                    txt = string.format("0x%02x ", i, txt)
                end
                curr_row += txt
                
                i += 1
            end
        
            modal_group.paragraph("i2c_scan_txt", {
                'text': curr_row
            })

            var done_button = modal_group.button_field("i2c_scan_done_button", {
                'label': "",
                'buttonText': "Done",
            })
            if done_button.has_been("click") || modal_group.has_been("dismiss")
                state.setitem("i2c_scan_result", nil)
                ctx.request_redraw() # we need to redraw the form to remove the modal, otherwise it will be stuck
            end
        end
        
    end

    def get_volume_table()
        var volume_table = self.default_volume_table
        if self.state.find('volume_table') != nil
            volume_table = json.load(self.state['volume_table'])
        end
        return volume_table
    end

    def bin(num)
        var res = 0
        for x : 1..size(num)
            res += math.pow(2, x-1) * int(num[size(num) - x])
        end
        return int(res)
    end    
end
