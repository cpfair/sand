define(["react", "jsx!controls", "jsx!drag_and_drop", "jsx!icon_preview"], function(React, Controls, DragAndDropMixin, IconPreview){

    var config = React.createClass({
        render: function(){
            var visual_indicator_controls = this.props.getConfigValue("visual_indicator") ?
                        (<div className="group">
                            <Controls.colour label="Colour" name="colour_fg" target={this}/>
                            <Controls.checkbox label="Always visible" name="visual_indicator_always_visible" target={this} />
                        </div>)
                        : null;
            return  <div>
                        <Controls.checkbox label="Vibe on disconnect" name="vibe_indicator" target={this} />
                        <Controls.checkbox label="On-screen indicator" name="visual_indicator" target={this} />
                        { visual_indicator_controls }
                    </div>;
        }
    });

    var preview = React.createClass({
        mixins: [DragAndDropMixin],
        dnd_position_key: "pos",
        render_canvas: function(){
            if (!this.props.getConfigValue("visual_indicator")) return;
            return this.dnd_render_wrap(this.refs.icon_preview.render_canvas).apply(this.refs.icon_preview, arguments);
        },
        render: function(){
            if (!this.props.getConfigValue("visual_indicator")) return null;
            return this.dnd_wrap(<IconPreview ref="icon_preview" iconKey="BT_DISCONNECTED_SMALL" colour={this.props.getConfigValue("colour_fg")}/>);
        }
    });

    var module = {
        label: "BT Status",
        info: "Add an on-screen BT icon, and get a subtle alert when your phone goes out of range.",
        defaults: {
            visual_indicator: false,
            visual_indicator_always_visible: false,
            vibe_indicator: false,
            colour_fg: "GColorBlack",
            pos_x: 20,
            pos_y: 20
        },
        configuration_pane: config,
        preview: preview
    };

    return module;
})