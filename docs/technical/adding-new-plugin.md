# Extending Euphonium

## Euphonium Architecture

There are essentially 4 layers of the Euphonium application which are:

- The Web UI written in React / Javascript
- The "Application layer" written in [Berry Scripting language](https://github.com/berry-lang/berry).
- A platform agnostic "Feature layer", written in C/C++
- A platform Specific "Driver layer", written in C/C++

Plugins can integrate into each layer, and communicate between each layer.
Having a firm understanding of how these layers communicate will help you
understand what plugin's can do and what code you need to write when creating
a new plugin.

### Web Plugins in Brief

Plugins can expose functionality the users in the Web Application via the
`make_form()` method of the application layer plugin. Forms in the web app have
two distinct functions: (1) exposing the current plugin state to the user, and
(2) receiving inputs such as updated settings from users. The Web app is
extended by creating an application layer plugin and creating a form using it's
`make_form()` method.

Additionally the Web app is a React (Vite) app located in the `web/` directory
that you can modify to your heart's content.

### Application Layer Plugins in Brief

Application layer plugins are written in [Berry Scripting
language](https://github.com/berry-lang/berry) and inherit from the [Plugin
Class](https://github.com/feelfreelinux/euphonium/blob/master/euphonium/scripts/internal/plugin.be)

Plugin scripts located in the `euphonium/scripts/plugins` and are used to define a
new plugin class, instantiate the plugin and register it with Euphonium like so:

```berry
class MyPlugin : Plugin
    def init()
        # Define constants like the plugin name
    end

    def make_form(cts,state)
        # Create the form that allows users to set plugin settings in the Web UI
    end

    def on_event(event, data)
       # Handle events, such as plugin initialization, updates to the plugin
       # state (in response to user interaction with the Web app)
    end
end

# Instantiate your plugin
my_plugin = MyPlugin()

# Register your plugin with the euphonium application
euphonium.register_plugin(my_plugin)
```

As of this writing, these are the events handled by the plugin's `on_event()` method:

- EVENT_CONFIG_UPDATED
- EVENT_VOLUME_UPDATED
- EVENT_SYSTEM_INIT
- EVENT_SET_PAUSE
- EVENT_PLUGIN_INIT

### Feature Layer Plugins in Brief

Feature plugin interfaces are defined in the
`euphonium/include/plugins/[pluginName]` directories and the methods are defined
(out of line) in the `euphonium/src/plugins/[pluginName]` directories.

Feature plugins inherit from the
[Module](https://github.com/feelfreelinux/euphonium/blob/master/euphonium/include/Module.h)
and [bell::Task](https://github.com/feelfreelinux/bell/blob/master/include/Task.h#L17) classes.

For the purposes creating plugins,
[`bell::Task`](https://github.com/feelfreelinux/bell/blob/master/include/Task.h)
has the interface:

```c++
class Task {
public:
    std::string taskName;
protected:
    // Override this to start your plugin task
    virtual void runTask() = 0;
}
```

and the
[`Module`](https://github.com/feelfreelinux/euphonium/blob/master/euphonium/include/Module.h)
class has this interface:

```cpp
class Module {
public:
    Module() {} // constructor

    // Module name:
    std::string name;

    // Module Status:
    ModuleStatus status = ModuleStatus::SHUTDOWN;

    // A shared pointer to the berry runtime (vm) which can be used to expose
    // data and functions as to the berry runtime
    std::shared_ptr<berry::VmState> berry;

    // A shared pointer to the luaEventBus where events can be posted to the
    // application from the plugin
    std::shared_ptr<EventBus> luaEventBus;

    // The audioBuffer
    std::shared_ptr<MainAudioBuffer> audioBuffer;

    // configuration managed by the
    berry::map config;

    // PLUGIN LIFE CYCLE METHODS:

    // ??
    virtual void loadScript(std::shared_ptr<ScriptLoader> scriptLoader) = 0;

    // Called by the Core application when the Berry runtime is ready to have
    // data and functions exported
    virtual void setupBindings() = 0;

    // ??
    virtual void startAudioThread() = 0;

    // Called by the Core application at shut down
    virtual void shutdown() = 0;
};
```

When Feature plugins are loaded, they are provided with a reference to the
`berry` and `luaEventBus`, and then their `setupBindings()` method is called
with the following:

```cpp
// euphonium/src/Core.cpp
plugin->berry = this->berry;
plugin->luaEventBus = this->luaEventBus;
plugin->setupBindings();
```

The `berry` and `luaEventsBus` are used to communicate with the application
layer, as illustrated below.

### Driver Layer Plugins in Brief

Driver layer plugins (drivers, really) implement platform specific features and
are located in the platform specific `target/[platform]` directories. For
example, the I2C driver for the ESP32 is contained in the
`target/esp32/app/main/driver` directory. Drivers are used to expose platform
specific features (e.g. I2C) to the application layer.

# Communication between Euphonium Layers

## Communication from Web Forms to Application Layer Plugins

The web forms in the settings section of the app communicate to the application
layer via HTTP requests made to the `/plugins/:name` endpoint. When a `POST`
request is made to this endpoint, the request body is used to update the
plugin's state, and the plugin is notified that it's stat has been updated with
a call to it's `on_event()` method like so:

```be
plugin.on_event(EVENT_CONFIG_UPDATED, plugin.state)
```

Note that even though the new plugin stat is supplied as the second argument to
this method call, the plugin's state has already been replaced before the method
is called, so this invocation is merely an opportunity to respond to the
state change; the method does not need to update it's own state in order for the
state to be updated in this case.

## Application Layer Plugin HTTP APIs

Application layer plugins may expose API endpoints by registering a
callback with the [http plugin](https://feelfreelinux.github.io/euphonium/plugins/scripting-language/#http).

# Examples

## Application Layer Plugin Example

This plugin toggles a pin LOW or HIGH in response to http `POST` requests made
at a custom endpoint. Users can select the output pin on the in the web ap
under the "LED Driver" settings.

```be
class LED_Driver : Plugin
    var pin

    def init()
        self.apply_default_values()
        self.name = "led_driver"
        self.theme_color = "#d2c464"
        self.display_name = "LED Driver"
        self.type = "plugin"
    end

    def make_form(cts,state)
        # Create the form that allows users select the output pin for the LED
        ctx.create_group('led-driver', { 'label': 'General' })
        ctx.number_field('pin', {
            'label': "Output Pin",
            'default': "0",
            'group': 'led-driver',
        })
    end

    def on_event(event, data)
        if event == EVENT_CONFIG_UPDATED
            if data.find('pin') != nil && data['pin'] != '0'
                gpio.pin_mode(self.state['pin'], gpio.INPUT_PULLUP)
            end
        end
        if event == EVENT_PLUGIN_INIT
            if self.state.find('pin') != nil && self.state['pin'] != '0'
                gpio.pin_mode(self.state['pin'], gpio.INPUT_PULLUP)
            end
        end
    end

    def set_pin(pin_state)
        # custom method to be called by the HTTP callback
        if self.state.find('pin') == nil || self.state.pin == '0'
            # Output pin has not been set
            return
        end
        gpio.digital_write(self.pin,pin_state)
    end

end

# Instantiate the application layer plugin
led_driver = LED_Driver()

# Register the application layer plugin with the euphonium application
euphonium.register_plugin(CSpotPlugin())

# Register the http endpoint
http.handle("POST","/toggle-pin",def
    if request.json_body() == 'true'
        led_driver.set_pin(gpio.HIGH)
    else
        led_driver.set_pin(gpio.LOW)
    end
end)
```

## The Event Bus

Communication within the Feature and to the Application layer can be
achieved by posting messages to an Feature layer event bus. Messages
posted to the Feature layer event bus are propagated to both
Feature and application layer event subscribers.

When Feature plugins are registered, a reference to the `mainEventBus` is
bound to the plugin's `luaEventBus` property. Modules can therefor post events
to the event bus using

```cpp
this->luaEventBus->postEvent(std::move(event));
```

Feature layer plugins can subscribe to the event bus by registering a
listener which implements the EventSubscriber interface. Plugins which implement
an appropriate `handleEvent()` method can therefore register themselves as
subscribers using:

```cpp
auto subscriber = dynamic_cast<EventSubscriber *>(this);
luaEventBus->addListener(EventType::LUA_MAIN_EVENT, *subscriber);
```

Notably the Euphonium Core registers itself as a subscriber, and uses that
subscription to propagate events to the `handle_event` global in the application
layer which then propagate those events to registered event handlers and plugins.

Application layer plugins can receive events by registering a callback manually
with the euphonium core using:

```berry
euphonium.register_handler('wifiStateChanged', def (event)
    # Do something when the wifi state changes...
end)
```

**Note that** only one handler register may be registered to a given event.

## Asynchronous tasks

Application may need to do tasks periodically, such as update a display, call
out to a web server, etc. As of this writing, these types of tasks cannot be
accomplished in the application layer as berry language is synchronous, and code like this will block the application indefinitely :

```berry
# Do *not* to this:
while 1
    do_something()
    sleep_ms(1000)
end
```

Instead you'll need to create either a feature or driver plugin which inherits
from `bell::Task` and runs your asynchronous tasks in a FreeRTOS task, like so:

```cpp
MyAwesomePlugin::MyAwesomePlugin() : bell::Task("awesome", 6 * 1024, 0, 1) {
    name = "awesome";
    // create the FreeRTOS Task to run the `runTask()` method
    this->startTask();
}

void MyAwesomePlugin::runTask() {
    while (true) // or any FreeRTOS idiom like `for(auto item: someQueue)`
    {
        do_something()
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
```

## Exposing C++ Objects in the Berry language

While events can be propagated from the feature layer to the application
layer, (as of this writing) the same mechanism cannot be used to communicate
from the application layer to the feature layer. In order for the the
feature layer to receive events from the application layer,
feature plugins can bind functions, methods, and values into berry
runtime.

This is accomplished using the convenience methods of the `berry` reference
that is attached to each feature plugin, such as:

```cpp
// bind the MQTTPlugin::publish method to mqqt.plugin function in the berry runtime
berry->export_this("publish", this, &MQTTPlugin::publish, "mqtt");

// bind the gpioDigitalWrite function to gpio.digital_write function in the berry runtime
berry->export_function("digital_write", &gpioDigitalWrite, "gpio");

// bind the sleepMS function to sleep_ms global function in the berry runtime
berry->export_function("sleep_ms", &sleepMS);
```

# Installing Plugins

## Installing Application layer plugins

The Berry scripts that define the application layer plugins are stored in the
`euphonium/scripts/plugin` directory.

**Pro Tip:** Start by creating a blank file (`my-plugin.be`) for your plugin,
then compile, flash, and run the Euphonium application. This will create a new
empty file in the that you can edit using the Web IDE. The Web IDE is a web
application that you can run from your local machine, connect to your ESP32, edit
your new plugin and debug your code in real time. When the plugin is working
the way you like it, copy your code out of the web IDE and into your plugin's
`.be` file.

You can start the Web IDE by navigating to the `/web-ide` in the repo, with
these commands:

```bash
# install the dependencies (only required once)
yarn install

# Run the Web IDE application
yarn start
```

then Navigate to `http://localhost:3000` (if the window doesn't open on it's
own) to use the Web IDE

## Installing Feature layer (C/C++) plugins

### Installing the `.cpp` and `.h` files:

Your plugin will need `.h` and `.cpp` files, which you can create with:

```sh
pushd euphonium/src/plugins
mkdir newFeature
cd newFeature
touch newFeaturePlugin.cpp
popd
```

and:

```sh
pushd euphonium/include/plugins
mkdir newFeature
cd newFeature
touch newFeaturePlugin.h
popd
```

Next, you'll need to tell the build system about the source files for your new
plugin by adding to the list of glob patterns (e.g.
`"src/plugins/my-plugin/*.cpp"` or `"src/plugins/my-plugin/*.c"`)
[here](https://github.com/feelfreelinux/euphonium/blob/master/euphonium/CMakeLists.txt#L15)
in the same `CMakeLists.txt` file.

You also need to add your include directory near the bottom of
`euphonium/CMakeLists.txt` like so:

```cpp
include_directories("include/plugins/my-plugin")
```

Finally, you'll need to add include your plugin's header file to
`euphonium/include/Core.cpp`, and add your plugin to the [list of registered
plugins](https://github.com/feelfreelinux/euphonium/blob/master/euphonium/src/Core.cpp#L42-L46)
in `euphonium/src/Core.cpp`

## Using libraries with Feature Plugins

If your plugin is going to rely on existing external C/C++ libraries, then
you'll need to load them into the repo (preferably as sub-modules), and tell the
build system about the additional libraries. For example, if you want to library
from `github.com/some/great-library`, then you'll want to clone in into `euphonium/`
with:

```sh
git submodule add ../../some/great-library euphonium/great-library
```

and then update `euphonium/CMakeLists.txt` with your dependency. Probably
something like

```cmake
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bell ${CMAKE_CURRENT_BINARY_DIR}/bell)
```

as well as adding your library to this lists in `target_link_libraries()` and
`target_include_directories()`. A note of caution: Libraries that are aware of
or depend on the esp-idf features are likely to [flummox the build process](https://github.com/feelfreelinux/euphonium/issues/58)
if they are included in the euphonium core (`euphonium/`)

# Installing Platform Specific Drivers

Platform specific drivers, will depend on the platform. New feature drivers for
the ESP32, for example, can be installed with:

```
pushd targets/esp32/app/main/driver
mkdir feature
cd feature
touch featureDriver.cpp
touch featureDriver.h
popd
```

Driver functionality should be exposed to the Application layer in the usual
ways:

- Exporting C/C++ functions into the Berry language vm with the export helpers
  (`berry->export_something()`), which the ESP32 platform exposes to the drivers
  registers
  [here](https://github.com/feelfreelinux/euphonium/blob/master/targets/esp32/app/main/driver/ESP32Platform.cpp#L125-L141)

- create a plugin which inherits from `bell::Task` and [register it as a plugin
  in main.cpp](https://github.com/feelfreelinux/euphonium/blob/e562fea1a8eb659e9cba97820036f6208b2aec34/targets/esp32/app/main/main.cpp#L50), which is how the Bluetooth Plugin is registered in the ESP32 target.

## Using ESP-IDF Libraries with (Drivers)

When writing drivers for the ESP32, the ESP-IDF will automatically include
projects in the `targets/esp32/app/components` directory. For example, if you
want to add a display using the awesome `u8g2` library, you could add the
required libraries as git submodules like so:

```sh
git submodule add ../../olikraus/u8g2 targets/esp32/app/components/u8g2
git submodule add ../../mkfrey/u8g2-hal-esp-idf targets/esp32/app/components/u8g2-hal-esp-idf
```

and like that, your new components can be consumed in your driver files.
