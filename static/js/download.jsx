define(["react", "classnames", "reqwest"], function(React, classnames, reqwest){
    var stateMap = {
        queued: "Queued",
        downloading: "Fetching app...",
        generatingcode: "Preparing mixins...",
        patching: "Remixing...",
        done: "Done",
        fail: "Failed :("
    };
    var DownloadControl = React.createClass({
        getInitialState: function(){
            return {};
        },
        _poll: function(){
            reqwest({url: this.state.status_uri}).then(function(result) {
                this.setState({jobStatus: result});
                if (result.state == "done" || result.state == "fail") {
                    clearInterval(this._poll_interval);
                    this.setState({running: false});
                }
            }.bind(this));
        },
        startJob: function(){
            this.setState({running: true, jobStatus: {}, capturedConfigurationGeneration: this.props.configurationGeneration});
            reqwest({url: "/job", method: "POST", contentType: "application/json", data: JSON.stringify({app: this.props.app, configuration: this.props.configuration})}).then(function(result){
                this.setState({status_uri: result.uri});
                this._poll_interval = setInterval(this._poll, 1000);
            }.bind(this));
        },
        render: function() {
            var controls, result, error;
            if (!this.state.running) {
                if (this.state.capturedConfigurationGeneration != this.props.configurationGeneration) {
                    controls = <div><button onClick={this.startJob}>Remix {this.props.app.name} {this.state.capturedConfigurationGeneration ? "Again" : ""}</button></div>;
                }
            } else {
                controls = <button className="stuck">{stateMap[this.state.jobStatus.state] || "Getting ready..."}</button>;
            }

            if (this.state.jobStatus) {
                if (this.state.jobStatus.error) {
                    error = <div className="error">{this.state.jobStatus.error}</div>;
                }

                if (this.state.jobStatus.state == "done") {
                    result = <a className="download-link" href={this.state.jobStatus.pbw_url}>
                                <img className="download-qr" src={"https://chart.googleapis.com/chart?chs=150x150&cht=qr&choe=ISO-8859-1&chl=" + encodeURI(this.state.jobStatus.pbw_url)}/>
                                <div className="download-instructions">(scan, or click to download PBW)</div>
                            </a>
                }
            }

            return  <div className="download-control">
                        {controls}
                        {result}
                        {error}
                    </div>

        }
    });

    return DownloadControl;
});
