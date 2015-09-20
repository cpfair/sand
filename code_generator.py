import os
import re


class CodeGenerator:
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
        # I'm sure there's some obscure GCC directive that lets you dump arbitrary parts of the file system or something silly like that
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
        for module_name, text in module_code.items():
            # Add config defines to module code
            prepend = ["#define %s %s" % (k, cls._sanitize_parameter(v)) for k, v in module_configs[module_name].items() if v]
            postpend = ["#undef %s" % k for k in module_configs[module_name].keys()]
            text = "\n".join(prepend) + "\n" + text + "\n" + "\n".join(postpend)

            # Grab hooks
            # We insert an automatically generated function name, and alias it to a preprocessor directive which we will eventually call it via
            def hook_sub(match):
                nonlocal hook_call_idx
                hook_name = match.group(2)
                call_name = "hook_%d" % hook_call_idx
                hook_call_idx += 1
                hook_calls.setdefault(hook_name, []).append(call_name);
                return "#define _%s(...) %s(__VA_ARGS__);\n#define HOOK_%s\n" % (call_name, call_name, hook_name.upper()) + match.group(1) + call_name
            text = re.sub(r"^(.+?)%hook\(([^\)]+)\)", hook_sub, text, flags=re.MULTILINE)

            module_code[module_name] = text

        hook_preamble = []
        hook_postamble = []
        for hook_name, hook_calls in hook_calls.items():
            # We don't know which hooks actually got built, and we don't want to reverse-engineer their ifdefs to find out
            # so, instead, we let the preprocessor do the work for us
            for call in hook_calls:
                hook_postamble.append("#ifndef _%s\n#define _%s(...)\n#endif" % (call, call))

            hook_preamble.append("#define HOOK_%s_CALLS(...) %s" % (hook_name.upper(), "".join(["_%s(__VA_ARGS__)" % fcn for fcn in hook_calls])))

        master_code = master_code.replace("%module_code", "\n".join(list(module_code.values()) + hook_postamble)).replace("%hooks_preamble", "\n".join(hook_preamble))
        return master_code
