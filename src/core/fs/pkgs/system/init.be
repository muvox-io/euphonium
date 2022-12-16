import json
import global

# /-- core bindings, usually in bindings.be, but required for the core to function
class Core
  def member(name)
      return get_native('core', name)
  end
end

core = Core()
# --/

# # Load essential modules
core.load("system", "src/bindings.be")
core.load("system", "src/http.be")

core.load("system", "src/event_handler.be")
# core.load("system", "internal/form_ctx.be")
# core.load("system", "internal/plugin.be")
# core.load("system", "internal/hooks.be")
# core.load("system", "internal/euphonium.be")
# core.load("system", "internal/api.be")
