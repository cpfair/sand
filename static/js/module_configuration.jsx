define(["react"], function(React){
    var ModuleConfiguration = React.createClass({
        render: function() {
            if (!this.props.module){
                return <div className="configure-block">
                    <h1>Pick some mixins</h1>
                    <p>They're over on the left - don't worry, you can use as many as you want.</p>
                </div>
            } else {
                var Module = this.props.module; // capital M becase JSX
                var info = !Module.hide_info ? <p>{Module.info}</p> : null;
                return <div className="configure-block">
                            <h1>{Module.label}</h1>
                            {info}
                            <Module.configuration_pane setConfigValue={this.props.setModuleConfigValue} getConfigValue={this.props.getModuleConfigValue}/>
                        </div>;
            }
        }
    });

    return ModuleConfiguration;
});
