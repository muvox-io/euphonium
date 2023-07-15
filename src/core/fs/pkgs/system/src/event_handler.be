# Event handler allows for communication between native-berry, and berry-berry
class EventHandler
  # Internal event handlers are closures that will be called when an event is received from the berry side.
  var internal_handlers

  # Native event handlers are closures that will be called when an event is received from the native side.
  var native_event_handlers

  def init()
    self.internal_handlers = {}
    self.native_event_handlers = {}
  end

  def register(event, handler)
    if self.internal_handlers.find(event) == nil
      self.internal_handlers[event] = []
    end

    self.internal_handlers[event].push(handler)
  end

  def handle(event, data)
    if self.internal_handlers.find(event) != nil
      self.internal_handlers[event](data)
    end
  end

  def register_native(event, handler)
    if self.native_event_handlers.find(event) == nil
      self.native_event_handlers[event] = []
    end

    self.native_event_handlers[event].push(handler)
  end

  def handle_native(event, data)
    if self.native_event_handlers.find(event) == nil
      print("No handler for event: " + event)
      print(data)
      return
    end

    for handler : self.native_event_handlers[event]
      handler(data)
    end
  end
end

events = EventHandler()

# Native closure that will be called when an event is received from the native side.
# Aka, `EventBus` native-to-berry bridge
def handle_event(event_type, event_data)
  events.handle_native(event_type, event_data)
end
