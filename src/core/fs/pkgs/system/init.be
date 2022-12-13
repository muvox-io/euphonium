class Core
  def member(name)
      return get_native('core', name)
  end
end

core = Core()

# Load essential modules
core.load("system", "internal/util.be")
core.load("system", "internal/http.be")
core.load("system", "internal/form_ctx.be")
core.load("system", "internal/plugin.be")
core.load("system", "internal/hooks.be")
core.load("system", "internal/euphonium.be")
core.load("system", "internal/api.be")