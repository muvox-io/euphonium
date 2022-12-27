class Hooks
    var hooks
    var ON_INIT
    var POST_SYSTEM
    var POST_PLUGIN
    var AP_INIT
    var WIFI_INIT

    def init()
        self.ON_INIT = "ON_INIT"
        self.POST_SYSTEM = "POST_SYSTEM"
        self.POST_PLUGIN = "POST_PLUGIN"
        self.AP_INIT = "AP_INIT"
        self.WIFI_INIT = "WIFI_INIT"

        self.hooks = {}
        self.hooks[self.ON_INIT] = []
        self.hooks[self.POST_SYSTEM] = []
        self.hooks[self.POST_PLUGIN] = []
        self.hooks[self.AP_INIT] = []
        self.hooks[self.WIFI_INIT] = []
    end

    def add_handler(hook, func)
        self.hooks[hook].push(func)
    end

    def call(hook)
        for hook : self.hooks[hook]
            hook()
        end
    end
end

hooks = Hooks()
