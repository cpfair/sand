// Here is where you define all the modules since we can't actually search the FS
// Once upon a time this was a nice array..., but r.js wasn't picking up the dependencies properly so...

var modules = ["bt_monitor", "battery_monitor", "flip", "palette_swap", "antialias", "button_remap", "power_save", "wakeup", "ptr_simulator"];

define(["jsx!modules/bt_monitor/bt_monitor",
        "jsx!modules/battery_monitor/battery_monitor",
        "jsx!modules/flip/flip",
        "jsx!modules/palette_swap/palette_swap",
        "jsx!modules/antialias/antialias",
        "jsx!modules/button_remap/button_remap",
        "jsx!modules/power_save/power_save",
        "jsx!modules/wakeup/wakeup",
        "jsx!modules/ptr_simulator/ptr_simulator"], function(){
    return [].splice.call(arguments, 0).map(function(module, idx){ module.name = modules[idx]; return module; });
});
