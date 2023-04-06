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
        self.safe_copy_field(configuration, field, 'hidden')

        self.fields.push(field)
    end


    def link_button(key, configuration)
        var field = {
            'key': key,
            'type': 'link_button',
            'label': configuration['label'],
            'link': configuration['link'],
            'placeholder': configuration['placeholder']
        }
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')
        self.safe_copy_field(configuration, field, 'group')
        self.safe_copy_field(configuration, field, 'hidden')

        self.fields.push(field)
    end

    def modal_confirm(key, configuration)
        var field = {
            'key': key,
            'type': 'modal_confirm',
            'label': configuration['label'],
            'hint': configuration['hint'],
        }
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')
        self.safe_copy_field(configuration, field, 'group')
        self.safe_copy_field(configuration, field, 'okValue')
        self.safe_copy_field(configuration, field, 'cancelValue')
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
        self.safe_copy_field(configuration, field, 'hidden')

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

    def button_field(key, configuration)
        var field = {
            'key': key,
            'type': 'button_field',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'buttonText')
        self.safe_copy_field(configuration, field, 'hint')
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

    # Creates a modal group, which is a group of fields that are displayed in a modal
    # Configuration:
    #   title: The title of the modal
    #   dismissable: Whether the modal can be dismissed by clicking outside of it
    #   global: Whether the modal should be shown everywhere in the app, not just in the current plugin
    #   priority: The priority of the modal, higher priority modals are shown first
    def modal_group(key, configuration)
        var field = {
            'key': key,
            'type': 'modal_group',
            'title': configuration['title'],
            'dismissable': false,
            'global': false,
            'priority': 0
        }
        if (configuration.find('dismissable') == true)
            field['dismissable'] = true
        end
        if (configuration.find('global') == true)
            field['global'] = true
        end
        self.safe_copy_field(configuration, field, 'priority')
        self.fields.push(field)
    end

    def has_global_modal()
        for field : self.fields
            if (field['type'] == 'modal_group' && field['global'] == true)
                return true
            end
        end
        return false
    end

    # Applies a state object to the form schema, so that each field has the value property
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
