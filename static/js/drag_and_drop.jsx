define(["react", "classnames"], function(React, classnames){
    var dnd_mixin = {
        getInitialState: function(){
            return {};
        },
        _dnd_get_pos: function(){
            return [this.props.getConfigValue(this.dnd_position_key + "_x"), this.props.getConfigValue(this.dnd_position_key + "_y")];
        },
        _dnd_set_pos: function(pos){
            this.props.setConfigValue(this.dnd_position_key + "_x", pos[0])
            this.props.setConfigValue(this.dnd_position_key + "_y", pos[1]);
        },
        _dnd_start_drag: function(e) {
            e.preventDefault();
            if (!this.props.active) return;
            this.setState({
                _dnd_dragging: true,
                _dnd_start_mouse_coords: [e.pageX, e.pageY],
                _dnd_start_object_pos: this._dnd_get_pos()
            });
            document.addEventListener("mousemove", this._dnd_do_drag);
            document.addEventListener("mouseup", this._dnd_end_drag);
        },
        _dnd_get_new_pos: function(e) {
            var container_el = React.findDOMNode(this.refs._dnd_container);
            var dx = e.pageX - this.state._dnd_start_mouse_coords[0];
            var dy = e.pageY - this.state._dnd_start_mouse_coords[1];
            var new_x = this.state._dnd_start_object_pos[0] + dx
            var new_y = this.state._dnd_start_object_pos[1] + dy;

            // Clamp
            new_x = Math.max(0, Math.min(container_el.parentElement.offsetWidth - container_el.offsetWidth, new_x));
            new_y = Math.max(0, Math.min(container_el.parentElement.offsetHeight - container_el.offsetHeight, new_y));
            return [new_x, new_y];
        },
        _dnd_do_drag: function(e) {
            // React state updates are too laggy for this
            var new_pos = this._dnd_get_new_pos(e);
            var container_el = React.findDOMNode(this.refs._dnd_container);
            container_el.style.left = new_pos[0] + "px";
            container_el.style.top = new_pos[1] + "px";
            this.props.render_preview_canvas();
        },
        _dnd_end_drag: function(e) {
            document.removeEventListener("mousemove", this._dnd_do_drag);
            document.removeEventListener("mouseup", this._dnd_end_drag);
            this._dnd_set_pos(this._dnd_get_new_pos(e));
            this.setState({_dnd_dragging: false});
        },
        dnd_render_wrap: function(renderer){
            var container_el = React.findDOMNode(this.refs._dnd_container);
            return function(canvas, ctx) {
                ctx.save();
                ctx.translate(container_el.offsetLeft, container_el.offsetTop);
                renderer.apply(null, arguments);
                ctx.restore();
            };
        },
        dnd_wrap: function(content){
            return <div ref="_dnd_container" onMouseDown={this._dnd_start_drag} className={classnames({"dnd-wrapper": true, active: this.props.active})} style={{left: this._dnd_get_pos()[0] + "px", top: this._dnd_get_pos()[1] + "px"}}>{content}</div>;
        }
    };
    return dnd_mixin;
});