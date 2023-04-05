import json
import global
import string

# /-- core bindings, usually in bindings.be, but required for the core to function
class Core
  def member(name)
      return get_native('core', name)
  end
end

core = Core()
# --/

# Load essential modules
core.load("system", "src/bindings.be")
core.load("system", "src/event_handler.be")
core.load("system", "src/http.be")
core.load("system", "src/hooks.be")
core.load("system", "src/form_ctx.be")
core.load("system", "src/plugin.be")
core.load("system", "src/util.be")

# Load core classes, http apis
core.load("system", "src/playback.be")
core.load("system", "src/euphonium.be")
core.load("system", "src/general_settings.be")
core.load("system", "src/reset_restore.be")
core.load("system", "src/api.be")
