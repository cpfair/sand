define(["react", "classnames", "colours"], function(React, classnames, colours) {

    var controls_mixin = {
        // Kinda cheap
        getConfigValue: function(){
            return this.props.target.props.getConfigValue(this.props.name);
        },
        setConfigValue: function(val){
            this.props.target.props.setConfigValue(this.props.name, val);
        }
    };

    var checkbox = React.createClass({
        mixins: [controls_mixin],
        _checked_change: function(e) {
            this.setConfigValue(e.target.checked);
        },
        render: function() {
            return <div><input type="checkbox" id={this.props.name} checked={!!this.getConfigValue()} onChange={this._checked_change}/><label htmlFor={this.props.name}>{this.props.label}</label></div>;
        }
    })

    var colour = React.createClass({
        mixins: [controls_mixin],
        getInitialState: function(){
            return {};
        },
        _toggle_picker: function(){
            this.setState({expanded: !this.state.expanded});
        },
        _pick_colour: function(name){
            this.setConfigValue(name);
            this.setState({expanded: false});
        },
        render: function(){
            var colour_blocks = colours.list.map(function(colour, idx){
                return <div style={{backgroundColor: colour.colour}} key={colour.name} className={"swatch " + colour.name} onClick={this._pick_colour.bind(this, colour.name)}></div>
            }.bind(this));
            return <div>
                        <div className="colour-field" onClick={this._toggle_picker}>{this.props.pre_label}<div className={"swatch " + this.getConfigValue()} style={{backgroundColor: colours.findByName(this.getConfigValue())}}></div>{this.props.label}</div>
                        <div className={classnames({"colour-picker": true, visible: this.state.expanded})}>
                            <div className="swatches">
                                {colour_blocks}
                            </div>
                        </div>
                    </div>;
        }
    });


    return {
        checkbox: checkbox,
        colour: colour
    };
})