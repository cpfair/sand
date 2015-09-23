define(["react", "jsx!controls"], function(React, Controls, DragAndDropMixin, IconPreview){

    var config = React.createClass({
        render: function(){
            return  <div>
                        <Controls.checkbox label="Disable every-second updates" name="coarse_tick_timers" target={this} />
                        <Controls.checkbox label="Disable phone communication" name="disable_appmessage" target={this} />
                    </div>;
        }
    });

    var module = {
        label: "Power Save",
        info: "Make your Pebble last longer on a charge.",
        defaults: {
            coarse_tick_timers: false,
            disable_appmessage: false
        },
        configuration_pane: config
    };

    return module;
})