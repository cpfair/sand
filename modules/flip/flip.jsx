define(["react", "jsx!controls", "jsx!drag_and_drop", "jsx!icon_preview"], function(React, Controls, DragAndDropMixin, IconPreview){

    var config = React.createClass({
        render: function(){
            return  <div>
                        <Controls.checkbox label="Flip horizontally" name="flip_hz" target={this} />
                        <Controls.checkbox label="Flip vertically" name="flip_vt" target={this} />
                    </div>;
        }
    });

    var preview = React.createClass({
        update_shared_preview_data: function(data) {
            // This nonsense is later read by the drag and drop stuff because I'm lazy and don't want to switch to paper.js or similar for d&d
            data.flip_hz = this.props.getConfigValue("flip_hz");
            data.flip_vt = this.props.getConfigValue("flip_vt");
        },
        pre_render_canvas: function(canvas, ctx){
            if (this.props.getConfigValue("flip_hz")) {
                ctx.translate(0, canvas.height/2);
                ctx.scale(1, -1);
                ctx.translate(0, -canvas.height/2);
            }
            if (this.props.getConfigValue("flip_vt")) {
                ctx.translate(canvas.width/2, 0);
                ctx.scale(-1, 1);
                ctx.translate(-canvas.width/2, 0);
            }
        },
        render: function(){
            return null;
        }
    });

    var module = {
        label: "Flip",
        info: "Flip an app upside down or left-to-right.",
        defaults: {
            flip_hz: false,
            flip_vt: false
        },
        configuration_pane: config,
        preview: preview
    };

    return module;
})