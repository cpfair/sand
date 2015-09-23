define(["react", "text!modules/icons.h", "colours"], function(React, icon_src, colours) {
    var icon_paths_pattern = /GPATH_DEFINE\((.+?)_\d+,\s*\d+,\s*.+?\{\{((?:\d+,\d+.+)+)\}\}\)\)/g;
    var coords_pattern = /(\d+),(\d+)/g;
    var paths = {}
    while ((m = icon_paths_pattern.exec(icon_src)) !== null) {
        path_key = m[1];
        if (!paths[path_key]) paths[path_key] = [];
        var path = [];
        while ((pm = coords_pattern.exec(m[2])) !== null) {
            path.push([parseFloat(pm[1]), parseFloat(pm[2])]);
        }
        paths[path_key].push(path);
    }

    var IconPreview = React.createClass({
        render_canvas: function(canvas, ctx){
            ctx.fillStyle = ctx.strokeStyle = colours.findByName(this.props.colour);
            ctx.translate(0.5, 0.5);
            var icon_paths = paths[this.props.iconKey];
            for (var pidx = 0; pidx < icon_paths.length; pidx++) {
                var path = icon_paths[pidx];
                ctx.beginPath();
                for (var ptidx = 0; ptidx < path.length; ptidx++) {
                    var pt = path[ptidx];
                    if (!ptidx) {
                        ctx.moveTo(pt[0], pt[1]);
                    } else {
                        ctx.lineTo(pt[0], pt[1]);
                    }
                };
                ctx.fill();
                ctx.stroke();
            }
            ctx.translate(-0.5, -0.5);
        },
        render: function(){
            var icon_paths = paths[this.props.iconKey];
            var width = Math.max.apply(null, icon_paths.map(function(path) {return Math.max.apply(null, path.map(function(pt){return pt[0];}));}));
            var height = Math.max.apply(null, icon_paths.map(function(path) {return Math.max.apply(null, path.map(function(pt){return pt[1];}));}));
            return <div className="placeholder" ref="placeholder" style={{width: width + "px", height: height + "px"}}></div>;
        }
    });

    return IconPreview;
});
