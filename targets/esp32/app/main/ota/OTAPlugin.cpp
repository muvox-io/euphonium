
#include "OTAPlugin.h"
#include "esp_ota_ops.h"
#include "esp_system.h"

OTAPlugin::OTAPlugin() : bell::Task("ota", 512, 3, 0)
{
}

void OTAPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("esp32/ota", berry);
}

void OTAPlugin::rebootToRecovery() {
    esp_partition_iterator_t pi;

    pi = esp_partition_find(ESP_PARTITION_TYPE_APP,
                            ESP_PARTITION_SUBTYPE_APP_FACTORY, "factory");

    const esp_partition_t *factory = esp_partition_get(pi);
    esp_partition_iterator_release(pi);
    esp_ota_set_boot_partition(factory);
    esp_restart();
}

void OTAPlugin::setupBindings() {
    berry->export_this("ota_reboot_recovery", this, &OTAPlugin::rebootToRecovery);
}
