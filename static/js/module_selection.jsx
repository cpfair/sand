define(["react", "modules/modules", "classnames"], function(React, modules, classnames){
    var ModuleSelection = React.createClass({
        render: function() {
            var module_options = modules.map(function(module) {
                var classes = classnames({item: true, selected: this.props.configuringModule == module});
                return <div className={classes} key={module.label} onClick={this.props.enabled ? this.props.configureModule.bind(null, module) : null}>{module.label}</div>
            }.bind(this));
            return <div className={classnames({"select-block": true, enabled: this.props.enabled})}>
                        {module_options}
                        <div className="item">Battery Status</div>
                        <div className="item">Date</div>
                        <div className="item">Time</div>
                        <div className="item">Weather</div>
                        <div className="item">Colour Swap</div>
                        <div className="item">No Vibe</div>
                        <div className="item">No Backlight</div>
                        <div className="item">Scheduled Launch</div>
                    </div>
        }
    });

    return ModuleSelection;
});
