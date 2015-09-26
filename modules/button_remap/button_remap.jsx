define(["react", "jsx!controls"], function(React, Controls){

    var config = React.createClass({
        render: function(){
            var button_opts = [
                {label: "Up", value: 1},
                {label: "Down", value: 3},
                {label: "Select", value: 2}
            ];
            // 'Back' isn't included as an option since it has a lot of default behaviour I haven't replicated
            // So I don't want people trying to use it and getting confused.
            return  <div className="button-remap">
                        <div><span className="row-label">Change <b>Up</b> → </span><Controls.select options={button_opts} name="remap_up" target={this}/></div>
                        <div><span className="row-label">Change <b>Down</b> → </span><Controls.select options={button_opts} name="remap_down" target={this}/></div>
                        <div><span className="row-label">Change <b>Select</b> → </span><Controls.select options={button_opts} name="remap_select" target={this}/></div>
                    </div>;
        }
    });


    var module = {
        label: "Remap Buttons",
        info: "When the app binds the left-hand button, the right-hand button will be used instead.",
        defaults: {
            // These are the defaults in the ButtonId enum
            remap_back: 0,
            remap_up: 1,
            remap_select: 2,
            remap_down: 3
        },
        configuration_pane: config
    };

    return module;
})