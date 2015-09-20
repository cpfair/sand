define(["react", "jsx!controls", "colours"], function(React, Controls, colours){

    var config = React.createClass({
        render: function(){
            return  <div>
                        <Controls.colour pre_label={<span>Change <div className="swatch" style={{backgroundColor: "black"}}></div>→</span>} name="remap_black" target={this}/>
                        <Controls.colour pre_label={<span>Change <div className="swatch" style={{backgroundColor: "white"}}></div>→</span>} name="remap_white" target={this}/>
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

            var remap = function(from, to) {
                if (from == "GColorClear" || to == "GColorClear") return;
                remapPairs.push({from: pblToRGB(from), to: pblToRGB(to)});
            }

            remap("GColorBlack", this.props.getConfigValue("remap_black"));
            remap("GColorWhite", this.props.getConfigValue("remap_white"));
            if (remapPairs) {
                var imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
                var pixels = imageData.data;
                for (var i = 0; i < pixels.length; i += 4) {
                    for (var p = 0; p < remapPairs.length; p++) {
                        var pair = remapPairs[p];
                        if (pair.from.r == pixels[i] && pair.from.g == pixels[i + 1] && pair.from.b == pixels[i + 2]){
                            pixels[i] = pair.to.r;
                            pixels[i + 1] = pair.to.g;
                            pixels[i + 2] = pair.to.b;
                            break;
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
        label: "Colourize",
        info: "Add some colour to your black & white apps.",
        defaults: {
            remap_black: "GColorClear",
            remap_white: "GColorClear"
        },
        configuration_pane: config,
        preview: preview
    };

    return module;
});
