
class BluetoothPlugin : Plugin

    # Generates a deterministic name based on the mac address of the ESP
    # Example: Free navy school 13fc
    def generate_device_name()
       
        var mac = wifi.get_mac()
        var macParts = string.split(mac, ':')
        var last2Bytes =  macParts.pop()
        last2Bytes = macParts.pop() + last2Bytes

        macParts = string.split(mac, ':') # reset the array after popping

        # dictionary of various words which are combined to create the name
        var opinions = ['Quick', 'Fast', 'Slow', 'Adorable', 'Cute', 'Ugly', 'Cold', 'Hot', 'Cursed', 'Sacred', 'Old', 'New', 'Small', 'Big', 'Long', 'Short', 'Round', 'Bad', 'Free', 'Dark', 'Light', 'Huge', 'Teeny', 'Loud', 'Quiet', 'Real', 'Moist', 'Dry', 'Wet', 'Proper']
        var colors = ['blue', 'red', 'white', 'black', 'pink', 'green', 'lime', 'cyan', 'aqua', 'navy', 'plum', 'purple', 'brown', 'teal', 'violet', 'grey', 'azure', 'beige']
        var things = ['tiger', 'onion', 'dog', 'cat', 'spoon', 'boat', 'car', 'lion', 'apple', 'pear', 'train', 'wrench', 'mouse', 'door', 'window', 'potato', 'lamp', 'chair', 'table', 'cable', 'hand', 'school', 'room', 'eye', 'ear', 'city', 'fox']

        var opinionsIdx = 2
        var colorsIdx = 13
        var thingsIdx = 3

        # generate pseudo random numbers based on the mac
        for part : macParts 
            opinionsIdx += string.byte(part[0]) * 3 + string.byte(part[1])
            if opinionsIdx % 2 == 0
                opinionsIdx -= 34
            end
            if opinionsIdx % 7 == 0
                opinionsIdx += 13
            end
            opinionsIdx ^= opinionsIdx >> 8
            opinionsIdx ^= opinionsIdx << 17
            colorsIdx ^= string.byte(part[1]) * 3 + string.byte(part[0])

            colorsIdx ^= colorsIdx << 13
            colorsIdx ^= colorsIdx >> 7

            thingsIdx ^= string.byte(part[0]) * 3 + string.byte(part[1])
            thingsIdx <<= string.byte(part[0])
            thingsIdx ^= thingsIdx << 5
            thingsIdx ^= colorsIdx >> 21
        end

        # wrap around the indices
        opinionsIdx %= opinions.size()
        colorsIdx %= colors.size()
        thingsIdx %= things.size()

        return opinions[opinionsIdx] + ' ' + colors[colorsIdx] + ' ' + things[thingsIdx] + ' ' + last2Bytes
    end
    def init()
        self.config_schema = {
            'name': {
                'tooltip': 'Device name',
                'type': 'string',
                'defaultValue': self.generate_device_name()
            },
        }

        self.apply_default_values()
        self.theme_color = "#287AA9"
        self.name = "bluetooth"
        self.display_name = "Bluetooth"
        self.type = "plugin"
    end
end

euphonium.register_plugin(BluetoothPlugin())
