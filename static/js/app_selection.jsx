define(["react", "classnames", "reqwest"], function(React, classnames, reqwest){
    var algoliaParams = {"apiKey":"8dbb11cdde0f4f9d7bf787e83ac955ed","appID":"BUJATNZD81"};
    var algoliaURL = "https://bujatnzd81-dsn.algolia.io/1/indexes/pebble-appstore-production/query";

    var AppSelection = React.createClass({
        getInitialState: function(){
            return {};
        },
        searchFieldKeydown: function(e){
            // 38 up, 40 down
            if (e.which == 38 || e.which == 40) {
                if (this.state.results) {
                    var selectDelta = e.which == 40 ? 1 : -1;
                    var currentIndex = this.state.results.indexOf(this.state.selectedResult);
                    var newIndex = Math.max(0, Math.min(this.state.results.length - 1, currentIndex + selectDelta));
                    this.previewResult(this.state.results[newIndex]);
                }
                e.preventDefault();
            }
            if (e.which == 13) {
                this.selectResult(this.state.selectedResult);
            }

        },
        search: function(){
            var query = React.findDOMNode(this.refs.search_field).value;
            algoliaParams.params = "hitsPerPage=5&page=0&query=" + encodeURI(query);
            reqwest({url: algoliaURL, method: "POST", data: JSON.stringify(algoliaParams)}).then(function(res) {
                this.setState({selectedResult: null, results: res.hits})
            }.bind(this));
        },
        deferredSearch: function(){
            if (this._deferred_search_timeout) clearTimeout(this._deferred_search_timeout);
            this._deferred_search_timeout = setTimeout(this.search, 500);
        },
        _resultToApp: function(result){
            return {
                name: result.title,
                id: result.id,
                screenshots: result.screenshot_images
            };
        },
        selectResult: function(result) {
            this.props.selectApp(this._resultToApp(result));
        },
        previewResult: function(result) {
            this.setState({selectedResult: result});
            this.props.previewApp(this._resultToApp(result));
        },
        render: function() {

            var results;
            if (this.state.results) {
                results = this.state.results.map(function(result, idx){
                    return <div key={result.id} className={classnames({result: true, active: result == this.state.selectedResult})} onClick={this.selectResult.bind(this, result)} onMouseOver={this.previewResult.bind(this, result)}>{result.title}</div>;
                }.bind(this));
            }
            return  <div className="app-select">
                        <h1>Pick an app to remix</h1>
                        <div className="search-field-wrap"><input ref="search_field" type="text" className="search-field" placeholder="Search the App Store" onChange={this.deferredSearch} onKeyDown={this.searchFieldKeydown}/></div>
                        <div className="results">{results}</div>
                    </div>;
        }
    });

    return AppSelection;
});
