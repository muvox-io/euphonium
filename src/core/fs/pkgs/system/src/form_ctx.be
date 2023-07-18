

var _FieldContext # Forward declaration

# abstract class with utitlity functions for creating forms
class BaseFormContext
    def safe_copy_field(configuration, field, field_name)
        if (configuration.find(field_name) != nil)
            field[field_name] = configuration[field_name]
        end
    end

    def text_field(id, configuration)
        var field = {
            'id': id,
            'stateKey': id,
            'type': 'text_field',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'stateKey')
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')
     
        self.safe_copy_field(configuration, field, 'hidden')

       self.add(field)

       return _FieldContext(field, self.root_context)
    end

    def link_button(id, configuration)
        var field = {
            'id': id,
            'type': 'link_button',
            'label': configuration['label'],
            'link': configuration['link'],
            'placeholder': configuration['placeholder']
        }
        self.safe_copy_field(configuration, field, 'hint')
        
       
        self.safe_copy_field(configuration, field, 'hidden')

        self.add(field)

        return _FieldContext(field, self.root_context)
    end

    def modal_confirm(id, configuration)
        var field = {
            'id': id,
            'stateKey': id,
            'type': 'modal_confirm',
            'label': configuration['label'],
            'hint': configuration['hint'],
        }
        self.safe_copy_field(configuration, field, 'stateKey')
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')
       
        self.safe_copy_field(configuration, field, 'okValue')
        self.safe_copy_field(configuration, field, 'cancelValue')
        self.add(field)
        return _FieldContext(field, self.root_context)
    end


    def number_field(id, configuration)
        var field = {
            'id': id,
            'stateKey': id,
            'type': 'number_field',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'stateKey')
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')
       
        self.safe_copy_field(configuration, field, 'hidden')
        self.add(field)
        return _FieldContext(field, self.root_context)
    end

    def checkbox_field(id, configuration)
        var field = {
            'id': id,
            'stateKey': id,
            'type': 'checkbox_field',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'stateKey')
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')


        self.add(field)
        return _FieldContext(field, self.root_context)
    end

    def select_field(id, configuration)
        var field = {
            'id': id,
            'stateKey': id,
            'type': 'select_field',
            'label': configuration['label'],
            'values': configuration['values']
        }
        self.safe_copy_field(configuration, field, 'stateKey')
        self.safe_copy_field(configuration, field, 'hint')
        self.safe_copy_field(configuration, field, 'default')


        self.add(field)
        return _FieldContext(field, self.root_context)
    end

    def button_field(id, configuration)
        var field = {
            'id': id,
            'stateKey': id,
            'type': 'button_field',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'stateKey')
        self.safe_copy_field(configuration, field, 'buttonText')
        self.safe_copy_field(configuration, field, 'hint')


        self.add(field)
        return _FieldContext(field, self.root_context)
    end

    def file_upload_field(id, configuration)
        var field = {
            'id': id,
            'stateKey': id,
            'type': 'file_upload_field',
            'label': configuration['label'],
            'uploadEndpoint': configuration['uploadEndpoint'],
        }
        self.safe_copy_field(configuration, field, 'stateKey')
        self.safe_copy_field(configuration, field, 'buttonText')
        self.safe_copy_field(configuration, field, 'hint')


        self.add(field)
        return _FieldContext(field, self.root_context)
    end

    def paragraph(id, configuration)
        var field = {
            'id': id,
            'type': 'paragraph',
            'text': configuration['text']
        }
        self.add(field)
        return _FieldContext(field, self.root_context)
    end

    def table(id, configuration)
        var field = {
            'id': id,
            'type': 'table',
            'table_columns': configuration['columns'],
            'table_data': configuration['data']
        }
        self.add(field)
        return _FieldContext(field, self.root_context)
    end

    def graph(id, configuration)
        var field = {
            'id': id,
            'type': 'graph',
            'data': configuration['data'],
            'max_value': configuration['max_value'],
        }
        self.safe_copy_field(configuration, field, 'reverse_data')
        self.safe_copy_field(configuration, field, 'label')
        self.add(field)
        return _FieldContext(field, self.root_context)
    end

    def create_group(id, configuration)
        var field = {
            'id': id,
            'stateKey': id,
            'type': 'group',
            'label': configuration['label']
        }
        self.safe_copy_field(configuration, field, 'stateKey')
        self.safe_copy_field(configuration, field, 'hint')

        self.add(field)
        return _FieldContext(field, self.root_context)
    end

    # Creates a modal group, which is whose children are displayed in a modal
    # Configuration:
    #   title: The title of the modal
    #   dismissable: Whether the modal can be dismissed by clicking outside of it
    #   global: Whether the modal should be shown everywhere in the app, not just in the current plugin
    #   priority: The priority of the modal, higher priority modals are shown first
    def modal_group(id, configuration)
        var field = {
            'id': id,
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
        self.add(field)
        return _FieldContext(field, self.root_context)
    end


end

class FieldContext : BaseFormContext
    var data
    var root_context
    def init(data, root_context)
        self.data = data
        self.data['children'] = []
        self.root_context = root_context
    end
    def add(field)
        self.data['children'].push(field)
    end
    def has_been(event_name)
        for event : self.root_context.events
            if event['fieldId'] == self.data['id'] && event['name'] == event_name
                return true
            end
        end
        return false
    end
end

_FieldContext = FieldContext

class FormContext : BaseFormContext
    var children
    var root_context
    var events
    var redraw_requested
    var refresh_interval
    def init(events)
        self.children = []
        self.root_context = self
        self.events = events == nil ? [] : events
        self.redraw_requested = false
        self.refresh_interval = 0
    end

    def add(field)
        self.children.push(field)
    end

    def has_global_modal()
        for field : self.children
            if (field['type'] == 'modal_group' && field['global'] == true)
                return true
            end
        end
        return false
    end

    def request_redraw()
        self.redraw_requested = true
    end

    def walk(f)
        self._walk_arr(f, self.children)
    end

    def _walk_arr(f, arr)
        for field : arr
            f(field)
            if (field['children'] != nil)
                self._walk_arr(f, field['children'])
            end
        end
    end

end
