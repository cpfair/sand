define(["react", "jsx!controls", "colours"], function(React, Controls, colours){

    var config = React.createClass({
        render: function(){
            var colour_controls = this.props.getConfigValue("enabled") ? <div className="group"><Controls.colour label="Bezel colour" name="bezel_colour" target={this}/></div> : null;
            return  <div>
                        <Controls.checkbox label="Experience the circularity" name="enabled" target={this} />
                        {colour_controls}
                    </div>;
        }
    });

    var preview = React.createClass({
        render_canvas: function(canvas, ctx){
            var remapPairs = [];
            function pblToRGB(name) {
                var res = colours.findByName(name).match(/#([\da-f]{2})([\da-f]{2})([\da-f]{2})/i);
                return {
                    r: parseInt(res[1], 16),
                    g: parseInt(res[2], 16),
                    b: parseInt(res[3], 16)
                };
            }

            if (this.props.getConfigValue("enabled")) {
                var bezel_colour = pblToRGB(this.props.getConfigValue("bezel_colour"));
                var imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
                var pixels = imageData.data;
                for (var y = 0; y < canvas.height; y++) {
                    for (var x = 0; x < canvas.width; x++) {
                        var dx = x - 143/2;
                        var dy = y - 168/2;
                        var d = dx * dx + dy * dy;
                        if (d >= 5184) {
                            var pixelbase = (y * canvas.width + x) * 4;
                            pixels[pixelbase] = bezel_colour.r;
                            pixels[pixelbase + 1] = bezel_colour.g;
                            pixels[pixelbase + 2] = bezel_colour.b;
                        }
                    }
                }
                ctx.putImageData(imageData, 0, 0);
            }
        },
        render: function(){
            return null;
        }
    });

    var module = {
        label: "Time Round Simulator",
        info: "The future is now!",
        defaults: {
            enabled: false,
            bezel_colour: "GColorWhite",
        },
        configuration_pane: config,
        preview: preview
    };

    return module;
});
