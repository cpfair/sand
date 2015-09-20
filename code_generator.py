import os
import re

# This is a thrown-together pre-preprocessor that pulls in the individual module code, pastes in user configuration, and hooks everything up to the master file
# As multiple modules will want to do stuff like modify the framebuffer, we want some sort of compile-time hook infrastructure.
# I'm sure this could be achieved via some preprocessor tricks, too, but this works.
# Anyways
# Module code should use %hook(name) in place of the function name - this will "subscribe" that function to the named hook
# Then, the master code can use HOOK_<NAME>_CALLS(params, ...) to call all subscribed functions (in arbitrary order)
# It can also check the existence of the HOOK_<NAME> define to see if any functions did subscribe to a given hook


class CodeGenerator:
    # We take in whatever the user sends us with 0 validation up to this point - so we want to make sure they're not getting up to anything shady
    @classmethod
    def _sanitize_parameter(cls, parameter):
        # I'm sure there's some obscure GCC directive that lets you dump arbitrary parts of the file system or something silly like that
        if type(parameter) is str:
            return re.sub(r"[^a-zA-Z0-9\s]", "", parameter)
        elif type(parameter) in [float, int, bool]:
            return parameter
        else:
            raise TypeError("Bad config parameter type %s" % type(parameter))

    @classmethod
    def _sanitize_config_key(cls, key):
        if type(key) is str:
            return re.sub(r"[^a-zA-Z0-9]", "", key)
        else:
            raise TypeError("Bad config key type %s" % type(key))

    @classmethod
    def _sanitize_module_name(cls, name):
        # In case anyone wants to use the super-cool "../../../../../../../etc/passwd" module
        if type(name) is str:
            return re.sub(r"[^a-zA-Z0-9_]", "", name)
        else:
            raise TypeError("Bad module name type %s" % type(name))

    @classmethod
    def generate_c_code(cls, modules_dir, module_configs):
        module_configs = {cls._sanitize_module_name(k): v for k,v in module_configs.items()} # Stop shenanigans
        module_code = {k: open(os.path.join(modules_dir, k, k + ".c"), "r").read() for k in module_configs.keys() if module_configs[k]}
        master_code = open(os.path.join(modules_dir, "master.c"), "r").read()

        hook_call_idx = 0
        hook_calls = {}
        def hook_subscribe_name_sub(match):
            nonlocal hook_call_idx
            # We insert an automatically generated function name, and alias it to a preprocessor directive which we eventually call
            # HOOK_<NAME>_CALLS(...) -> _hook_#(...), ... -> hook_#(...)
            # And, if _hook_# never gets defined by a module (i.e. it was ifdef'd out), it gets stubbed out in the boilerplate before the master code
            # We also define HOOK_<NAME> in the same location, so the master code knows if a hook is used at all

            hook_name = match.group(2)
            call_name = "hook_%d" % hook_call_idx
            hook_call_idx += 1
            hook_calls.setdefault(hook_name, []).append(call_name)
            return "#define _%s(...) %s(__VA_ARGS__);\n#define HOOK_%s\n" % (call_name, call_name, hook_name.upper()) + match.group(1) + call_name

        for module_name, text in module_code.items():
            # Grab hooks the module declares and swap in auto-generated function names
            text = re.sub(r"^(.+?)%hook\(([^\)]+)\)", hook_subscribe_name_sub, text, flags=re.MULTILINE)

            # Add user's config to module code
            prepend = ["#define %s %s" % (k, cls._sanitize_parameter(v)) for k, v in module_configs[module_name].items() if v]
            # ...and take it out at the end (is there such thing as a "define scope" in gcc?)
            postpend = ["#undef %s" % k for k in module_configs[module_name].keys()]
            text = "\n".join(prepend) + "\n" + text + "\n" + "\n".join(postpend)

            module_code[module_name] = text

        hook_boilerplate = []
        for hook_name, hook_calls in hook_calls.items():
            # We don't know which hooks actually got built, and we don't want to reverse-engineer their ifdefs to find out
            # so, instead, we let the preprocessor do the work for us - if no module defined it, we stub it here
            for call in hook_calls:
                hook_boilerplate.append("#ifndef _%s\n#define _%s(...)\n#endif" % (call, call))

            # HOOK_<NAME>_CALLS is what the master code uses to initiate hooks - if none of a particular hook is defined, nothing comes out of the preprocessor at all
            # (the master code can use the existence of a define HOOK_<NAME> to check if a particular hook is used at all)
            hook_boilerplate.append("#define HOOK_%s_CALLS(...) %s" % (hook_name.upper(), "".join(["_%s(__VA_ARGS__)" % fcn for fcn in hook_calls])))

        # Paste in the module code itself, plus the hook boilerplate
        master_code = master_code.replace("%module_code", "\n".join(list(module_code.values()) + hook_boilerplate))
        return master_code
