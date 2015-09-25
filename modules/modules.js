// Here is where you define all the modules since we can't actually search the FS
// Once upon a time this was a nice array..., but r.js wasn't picking up the dependencies properly so...

var modules = ["bt_monitor", "flip", "palette_swap"];

define(["jsx!modules/bt_monitor/bt_monitor",
        "jsx!modules/flip/flip",
        "jsx!modules/palette_swap/palette_swap"], function(){
    return [].splice.call(arguments, 0).map(function(module, idx){ module.id = modules[idx]; return module; });
});
