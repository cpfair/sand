define(["react", "modules/modules", "classnames"], function(React, modules, classnames){
    var ModuleSelection = React.createClass({
        render: function() {
            var module_options = modules.map(function(module) {
                var classes = classnames({item: true, selected: this.props.configuringModule == module});
                return <div className={classes} key={module.label} onClick={this.props.enabled ? this.props.configureModule.bind(null, module) : null}>{module.label}</div>
            }.bind(this));
            return <div className={classnames({"select-block": true, enabled: this.props.enabled})}>
                        {module_options}
                    </div>
        }
    });

    return ModuleSelection;
});
