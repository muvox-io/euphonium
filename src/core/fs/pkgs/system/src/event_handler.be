# Native closure that will be called when an event is received from the native side.
# Aka, `EventBus` native-to-berry bridge
def handle_event(event_type, event_data)
  if event_type == "http_request"
    # Handle HTTP request
    http.handle_event(event_data)
  end
end