class FormContext
    var fields
    def init()
        self.fields = []
    end

    def safe_copy_field(configuration, field, field_name)
        if (configuration.find(field_name) != nil)
            field[field_name] = configuration[field_name]
        end
    end

    def text_field(key, configuration)
        var field = {
            'key': key,
            'type': 'text_field',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')
        self.safe_copy_field(configuration, field, 'group')

        self.fields.push(field)
    end

    def number_field(key, configuration)
        var field = {
            'key': key,
            'type': 'number_field',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')
        self.safe_copy_field(configuration, field, 'group')

        self.fields.push(field)
    end

    def checkbox_field(key, configuration)
        var field = {
            'key': key,
            'type': 'checkbox_field',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')
        self.safe_copy_field(configuration, field, 'group')

        self.fields.push(field)
    end

    def select_field(key, configuration)
        var field = {
            'key': key,
            'type': 'select_field',
            'label': configuration['label'],
            'values': configuration['values']
        }
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')
        self.safe_copy_field(configuration, field, 'group')

        self.fields.push(field)
    end

    def create_group(key, configuration)
        var field = {
            'key': key,
            'type': 'group',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'hint')

        self.fields.push(field)
    end

    def apply_state(state)
        for field : self.fields
            if (state.find(field['key']) != nil)
                field['value'] = state[field['key']]
            else
                if field.find('default') != nil
                    field['value'] = field['default']
                end
            end
        end
        return self.fields
    end
end