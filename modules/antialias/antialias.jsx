define(["react", "jsx!controls"], function(React, Controls, DragAndDropMixin, IconPreview){

    var config = React.createClass({
        render: function(){
            return  <div>
                        <Controls.checkbox label="Enable anti-aliasing for lines and shapes" name="enabled" target={this} />
                    </div>;
        }
    });

    // This one is hard to preview, beyond just applying a blur to everything (which looks like trash)

    var module = {
        label: "Anti-aliasing",
        info: "Make black & white apps less jagged on your Pebble Time.",
        defaults: {
            enabled: false
        },
        configuration_pane: config
    };

    return module;
});
