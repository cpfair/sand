define(["react", "modules/modules"], function(React, modules){
    var Preview = React.createClass({
        getInitialState: function(){
            return {};
        },
        render_preview_canvas: function(){
            var canvas = React.findDOMNode(this.refs.canvas);
            var ctx = canvas.getContext('2d');
            ctx.setTransform(1, 0, 0, 1, 0, 0);
            ctx.clearRect(0, 0, canvas.width, canvas.height);

            // Pre-render steps
            modules.forEach(function(module) {
                var module_preview_ref = this.refs[module.name];
                if (module_preview_ref && module_preview_ref.pre_render_canvas) module_preview_ref.pre_render_canvas(canvas, ctx);
            }.bind(this));

            if (this.state && this.state.background_image) ctx.drawImage(this.state.background_image, 0, 0);

            modules.forEach(function(module) {
                var module_preview_ref = this.refs[module.name];
                if (module_preview_ref && module_preview_ref.render_canvas) module_preview_ref.render_canvas(canvas, ctx);
            }.bind(this));
        },
        update_background: function(src){
            var img = new Image;
            if (src.indexOf("/") != 0) {
                // Run the request through our proxy to satisfy canvas's security requirements
                // https://assets.getpebble.com/api/file/<token>/convert?cache=true&w=144&h=168&fit=cro
                src = "/screenshot/" + src.split('/')[5];
            }
            img.src = src;
            this.setState({loading_background_image: img});
            img.addEventListener("load", function(){
                this.setState({background_image: this.state.loading_background_image});
                this.render_preview_canvas();
            }.bind(this));
        },
        componentWillUpdate: function(){
            var shared_data = {};
            modules.forEach(function(module) {
                var module_preview_ref = this.refs[module.name];
                if (module_preview_ref && module_preview_ref.update_shared_preview_data) module_preview_ref.update_shared_preview_data(shared_data);
            }.bind(this));
            this.state.shared_data = shared_data; // Wheee
        },
        componentDidUpdate: function(){
            this.render_preview_canvas();
        },
        componentDidMount: function(){
            this.render_preview_canvas();
            this.update_background(this.props.backgroundImageSrc);
        },
        componentWillReceiveProps: function(props) {
            if (props.backgroundImageSrc != this.props.backgroundImageSrc) {
                this.update_background(props.backgroundImageSrc);
            }
        },
        render: function() {
            var module_content = modules.map(function(Module) {
                if (!Module.preview) return;
                return <Module.preview shared_preview_data={this.state.shared_data} render_preview_canvas={this.render_preview_canvas} ref={Module.name} key={Module.name} getConfigValue={this.props.getConfigValue.bind(null, Module)} setConfigValue={this.props.setConfigValue.bind(null, Module)} active={this.props.configuringModule == Module}/>
            }.bind(this));
            return <div className="preview-holder">
                <div className="preview-content">
                    <canvas ref="canvas" width="144px" height="168px"></canvas>
                    <div className="preview-overlay">{module_content}</div>
                </div>
            </div>
        }
    });

    return Preview;
});
