from flask import Flask, render_template, jsonify, request, abort, url_for, send_file, Response, stream_with_context
from job import CompilationJob, CompilationState
import requests

app = Flask(__name__)

from webassets.loaders import PythonLoader
assets_env = PythonLoader("static.environment").load_environment()
assets_env.debug = True
css_urls = lambda: assets_env["css"].urls()

@app.route('/')
def index():
    return render_template('index.html', css_url=css_urls()[0])

def job_status(job):
    result = job.status
    if result["error"]:
        result["error"] = str(result["error"]) # Can't give them the Exception via JSON
    result.update({"uri": url_for("check_job", job_uuid=job.uuid)})
    if job.status["state"] == CompilationState.Done:
        result.update({"pbw_url": url_for("download_job", job_uuid=job.uuid, _external=True)})
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

@app.route('/job/<job_uuid>/download', methods=['GET'])
def download_job(job_uuid):
    job = CompilationJob.get(job_uuid)
    if job is None or getattr(job, "output_pbw", None) is None:
        abort(404)
    print("send", job.output_pbw)
    return send_file(job.output_pbw, as_attachment=True)

# We need to proxy the screenshots to have full use of canvas features without running into cross-domain issues
@app.route('/screenshot/<path:url>', methods=['GET'])
def proxy_screenshot(url):
    if not url.startswith("https://assets.getpebble.com/api/file/"):
        abort(400)
    req = requests.get(url, stream=True)
    return Response(stream_with_context(req.iter_content()), content_type=req.headers['content-type'])


if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0")
