define(["react", "underscore", "modules/modules", "jsx!preview", "jsx!app_selection", "jsx!module_selection", "jsx!module_configuration", "jsx!download"],
        function(React, _, modules, Preview, AppSelection, ModuleSelection, ModuleConfiguration, DownloadControl){
    var AppView = React.createClass({
        getInitialState: function(){
            return {
                configuration: {},
                app: null
            }
        },
        setConfiguringModule: function(module) {
            this.setState({configuringModule: module});
        },
        _setConfigValue: function(module, key, value) {
            this.state.configuration[module.name] = this.state.configuration[module.name] || {};
            this.state.configuration[module.name][key] = value;
            this.setState({configurationGeneration: (this.state.configurationGeneration || 0) + 1});
            this.forceUpdate(); // I'm not good at react.js
        },
        _getConfigValue: function(module, key) {
            var val = (this.state.configuration[module.name] || {})[key];
            return val === undefined ? module.defaults[key] : val;
        },
        _configWithDefaults: function() {
            var captured_config = {};
            modules.forEach(function(module) {
                captured_config[module.name] = _.defaults(this.state.configuration[module.name] || {}, module.defaults);
            }.bind(this));
            return captured_config;
        },
        selectApp: function(app) {
            this.setState({app: app, previewApp: null});
        },
        previewApp: function(app) {
            this.setState({previewApp: app});
        },
        render: function(){

            var right_hand_panel = (this.state.app ?
                                        <div className="docked-container configure-module-container"><ModuleConfiguration module={this.state.configuringModule} enabled={!!this.state.app} setModuleConfigValue={this._setConfigValue.bind(this, this.state.configuringModule)} getModuleConfigValue={this._getConfigValue.bind(this, this.state.configuringModule)}/></div> :
                                        <div className="docked-container select-app-container"><AppSelection selectApp={this.selectApp} previewApp={this.previewApp}/></div>
                                    );
            var previewOrApp = this.state.previewApp || this.state.app;
            var screenshot = previewOrApp ? previewOrApp.screenshots[0] : "/static/img/placeholder.svg";
            var download = <DownloadControl configurationGeneration={this.state.configurationGeneration} app={this.state.app} configuration={this._configWithDefaults()}/>;
            return  <div>
                        <div className="docked-container select-module-container"><ModuleSelection configureModule={this.setConfiguringModule} enabled={!!this.state.app} configuringModule={this.state.configuringModule} /></div>
                        { right_hand_panel }
                        <Preview backgroundImageSrc={screenshot} getConfigValue={this._getConfigValue} setConfigValue={this._setConfigValue} configuringModule={this.state.configuringModule} />
                        { download }
                    </div>;
        }
    });

    function App() {};
    App.prototype.boot = function(){
        this.view = React.render(<AppView/>, document.getElementById('app'));
    };
    return App;
});

