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

    var dow_selection = React.createClass({
        mixins: [controls_mixin],
        _dow_checked_change: function(dow, e) {
            var val = this.getConfigValue();
            if (e.target.checked) {
                val = val | (1 << dow);
            } else {
                val = val & ~(1 << dow);
            }
            console.log(dow, val);
            this.setConfigValue(val);
        },
        _dow_active: function(dow){
            return !!((this.getConfigValue() >> dow) & 1);
        },
        render: function() {
            var dow_controls = ["Su", "M", "T", "W", "Th", "F", "Sa"].map(function(dow_label, dow) {
                return <span key={dow} className="day"><input type="checkbox" id={this.props.name + dow} checked={this._dow_active(dow)} onChange={this._dow_checked_change.bind(this, dow)}/><label htmlFor={this.props.name + dow}>{dow_label}</label></span>
            }.bind(this));
            return  <div className="dow-picker">
                        <label>{this.props.label}</label>
                        <div>{dow_controls}</div>
                    </div>;
        }
    });

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

    var select = React.createClass({
        mixins: [controls_mixin],
        _selection_change: function(e) {
            this.setConfigValue(e.target.value);
        },
        render: function() {
            var options = this.props.options.map(function(option) {
                return <option key={option.value} value={option.value}>{option.label}</option>;
            }.bind(this));
            return  <select value={this.getConfigValue()} onChange={this._selection_change}>
                        {options}
                    </select>;
        }
    });


    return {
        checkbox: checkbox,
        days_of_week: dow_selection,
        colour: colour,
        select: select
    };
})