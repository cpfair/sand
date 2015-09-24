define(["react", "jsx!controls", "jsx!drag_and_drop", "jsx!icon_preview"], function(React, Controls, DragAndDropMixin, IconPreview){

    var config = React.createClass({
        render: function(){
            var hour_options = [];
            for (var i = 0; i < 24; i++) {
                hour_options.push({label: i, value: i});
            }
            var minute_options = [];
            for (var i = 0; i < 60; i++) {
                var label = i.toString();
                label = "0".substring(0, 2 - label.length) + label;
                minute_options.push({label: label, value: i});
            }
            return  <div>
                        Launch the app at <Controls.select options={hour_options} name="wakeup_hours" target={this}/>:<Controls.select options={minute_options} name="wakeup_minutes" target={this}/> every
                        <Controls.days_of_week name="days_of_week" target={this} />
                    </div>;
        }
    });

    var module = {
        label: "Scheduled Launch",
        info: "Start an app on a schedule.",
        hide_info: true,
        defaults: {
            days_of_week: 0,
            wakeup_hours: 12,
            wakeup_minutes: 15
        },
        configuration_pane: config
    };

    return module;
})