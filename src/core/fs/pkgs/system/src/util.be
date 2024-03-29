import string

class Util
    # Generates a deterministic name based on the mac address of the ESP
    # Example: Free navy school 13fc
    def generate_device_name()
        var mac = core.get_mac()
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

    # converts a string to a hostname safe string
    def to_hostname(name)
        hostname = ""
        for i: 0..(size(name) - 1)
            var c = name[i]
            if c == ' ' || c == '-' && size(hostname) > 0 && hostname[size(hostname) - 1] != '-'
                hostname += '-'
            elif string.byte(c) >= 65 && string.byte(c) <= 90
                hostname += string.char(string.byte(c) + 32)
            else
                hostname += c
            end
        end
        if hostname[size(hostname) - 1] == '-'
            hostname = hostname[0..size(hostname) - 2]
        end
        if size(hostname) >  32
            hostname = hostname[0..32]
        end
        if size(hostname) <  1
            hostname = "EUPHONIUM"
        end
        
        return hostname
    end

    def url_encode(str)
        var encoded = ""
        for i: 0..(size(str) - 1)
            var c = str[i]
            if string.byte(c) >= 65 && string.byte(c) <= 90
                encoded += string.char(string.byte(c) + 32)
            elif string.byte(c) >= 97 && string.byte(c) <= 122
                encoded += c
            elif string.byte(c) >= 48 && string.byte(c) <= 57
                encoded += c
            elif c == '-' || c == '_' || c == '.' || c == '~' || c == '!' || c == '*' || c == '(' || c == ')' || c == "'"
                encoded += c
            else
                encoded += '%'
                encoded += string.format("%02x", string.byte(c))
            end
        end
        return encoded
    end
end
util = Util()

