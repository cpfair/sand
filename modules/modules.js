// Here is where you define all the modules since we can't actually search the FS
var module_list = ["bt_monitor", "battery_monitor", "flip", "palette_swap", "power_save"];

define(module_list.map(function(module){return "jsx!modules/" + module + "/" + module}), function(){
    return [].splice.call(arguments, 0).map(function(module, idx){ module.name = module_list[idx]; return module; });
});
