from flask import Flask, render_template, jsonify, request, abort, url_for, send_file, Response, stream_with_context
from job import CompilationJob, CompilationState
import requests
import os

PRODUCTION = os.environ.get("PRODUCTION", False) == "1"

app = Flask(__name__)

# Otherwise errors disappear into the ether
if PRODUCTION:
    import logging
    stream_handler = logging.StreamHandler()
    stream_handler.setLevel(logging.ERROR)
    app.logger.addHandler(stream_handler)

if not PRODUCTION:
    from webassets.loaders import PythonLoader
    assets_env = PythonLoader("static.environment").load_environment()
    assets_env.debug = True
    css_url = lambda: assets_env["css"].urls()[0]
else:
    css_url = lambda: "/static/css/app.css"

js_url = lambda: "/static/js/app.min.js" if PRODUCTION else "/static/js/boot.js"

@app.route('/')
def index():
    return render_template('index.html', css_url=css_url(), js_url=js_url())

def job_status(job):
    result = job.status
    if result["error"]:
        result["error"] = str(result["error"]) # Can't give them the Exception via JSON
    result.update({"uri": url_for("check_job", job_uuid=job.uuid)})
    if job.status["state"] == CompilationState.Done:
        result.update({"pbw_url": url_for("download_job", job_uuid=job.uuid, pbw_name=os.path.basename(job.output_pbw), _external=True)})
    return jsonify(result)

@app.route('/job', methods=['POST'])
def create_job():
    parameters = request.get_json()
    job = CompilationJob.create(parameters)
    return job_status(job)

@app.route('/job/<job_uuid>', methods=['GET'])
def check_job(job_uuid):
    job = CompilationJob.get(job_uuid)
    if job is None:
        abort(404)
    return job_status(job)

if not PRODUCTION:
    @app.route('/job/<job_uuid>/download/<pbw_name>', methods=['GET'])
    def download_job(job_uuid, pbw_name):
        job = CompilationJob.get(job_uuid)
        if job is None or getattr(job, "output_pbw", None) is None:
            abort(404)
        print("send", job.output_pbw)
        return send_file(job.output_pbw, as_attachment=True)

    # We need to proxy the screenshots to have full use of canvas features without running into cross-domain issues
    @app.route('/screenshot/<token>', methods=['GET'])
    def proxy_screenshot(token):
        url = "https://assets.getpebble.com/api/file/" + token + "/convert?cache=true&w=144&h=168&fit=crop"
        req = requests.get(url, stream=True)
        return Response(stream_with_context(req.iter_content()), content_type=req.headers['content-type'])


if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0")
