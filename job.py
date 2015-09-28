import uuid
import concurrent.futures
import logging
from code_generator import CodeGenerator
import requests
import re
import tempfile
import os
import hashlib
import json
from unidecode import unidecode
from rockgarden import Patcher


class CompilationState:
    Queued = "queued"
    Downloading = "downloading"
    GeneratingCode = "generatingcode"
    Patching = "patching"
    Done = "done"
    Fail = "fail"


class CompilationJob:
    _compilation_worker_pool = concurrent.futures.ThreadPoolExecutor(max_workers=5)
    _jobs = {}
    _pbw_cache_dir = os.path.join(tempfile.gettempdir(), "sand-pbw-cache")
    _scratch_dir = os.path.join(tempfile.gettempdir(), "sand-scratch")

    @classmethod
    def create(cls, parameters):
        job_uuid = uuid.uuid4().hex
        job = CompilationJob(job_uuid, parameters)
        cls._jobs[job_uuid] = job
        future = cls._compilation_worker_pool.submit(job.run)
        def completion_callback(future):
            err = future.exception()
            if err:
                job._state = CompilationState.Fail
                job._error = err
                raise err
        future.add_done_callback(completion_callback)
        return job, future

    @classmethod
    def get(cls, job_uuid):
        if job_uuid not in cls._jobs:
            return None
        return cls._jobs[job_uuid]

    def __init__(self, uuid, parameters):
        assert parameters["app"]["id"], "No app UUID provided"
        self._uuid = uuid
        self._state = CompilationState.Queued
        self._parameters = parameters

    def run(self):
        self._state = CompilationState.Downloading
        # Get the app metadata
        appstore_uuid = self._parameters["app"]["id"] # Passed from JS-land
        appstore_uuid = re.sub(r"[^a-f0-9]", "", appstore_uuid)
        assert len(appstore_uuid) == 24, "Bad app UUID specified"
        self.app_metadata = app_metadata = requests.get("https://api2.getpebble.com/v2/apps/id/%s" % appstore_uuid).json()["data"][0]
        # Fetch the app itself, if we need to
        cached_pbw_name = "%s-%s.pbw" % (appstore_uuid, app_metadata["latest_release"]["id"])
        cached_pbw_path = os.path.join(self._pbw_cache_dir, cached_pbw_name)
        # (this has a race condition if you start a bunch of jobs at once)
        if not os.path.exists(self._pbw_cache_dir):
            os.makedirs(self._pbw_cache_dir)
        # Also a race condition here if two jobs were using the same PBW at once
        if not os.path.exists(cached_pbw_path):
            pbw_data = requests.get(app_metadata["latest_release"]["pbw_file"]).content
            with open(cached_pbw_path, "wb") as pbwhnd:
                pbwhnd.write(pbw_data)
        else:
            os.utime(cached_pbw_path, None) # So I can invalidate the cache by file metadata

        self._state = CompilationState.GeneratingCode
        # Generate the required C code
        modules_dir = os.path.join(os.path.dirname(__file__), "modules")
        c_code = CodeGenerator.generate_c_code(modules_dir, self._parameters["configuration"])

        # Set up our scratch space (same race condition as earlier...)
        scratch_dir = os.path.join(self._scratch_dir, self.uuid)
        if not os.path.exists(scratch_dir):
            os.makedirs(scratch_dir)

        # Write out config for later perusal
        json.dump(self._parameters, open(os.path.join(scratch_dir, "parameters.json"), "w"))

        c_code_path = os.path.join(scratch_dir, "mods.c")
        open(c_code_path, "w").write(c_code)

        # We swap out the UUID so the app store doesn't auto-update it away
        # We use a hard-to-guess prefix so people can't upload remixed apps in the PAS - only we should be doing that
        sha1 = hashlib.sha1()
        sha1.update(app_metadata["uuid"].encode("ascii"))
        sha1.update(os.environ.get("REMIXED_UUID_KEY", "").encode("ascii"))
        new_uuid = uuid.UUID("5a4d" + sha1.hexdigest()[:4] + app_metadata["uuid"][8:])
        self.output_pbw_uuid = new_uuid

        # Create the patched PBW
        self._state = CompilationState.Patching
        pbw_output_name = re.sub("[^a-zA-Z0-9]", "", unidecode(app_metadata["title"])) + "-remixed.pbw"
        pbw_output_path = os.path.join(scratch_dir, pbw_output_name)
        # Create a one-off logger to record the carnage
        patch_logger = logging.getLogger("rockgarden")
        patch_logger.setLevel(logging.DEBUG)
        fh = logging.FileHandler(os.path.join(scratch_dir, "patch.log"))
        patch_logger.addHandler(fh)
        Patcher(scratch_dir=os.path.join(scratch_dir, "patcher")).patch_pbw(cached_pbw_path, pbw_output_path, c_sources=[c_code_path], new_uuid=new_uuid, cflags=["-I%s" % modules_dir], ensure_platforms=["aplite", "basalt"])
        patch_logger.removeHandler(fh)
        fh.close()

        self._state = CompilationState.Done
        self._output_pbw = pbw_output_path

    @property
    def status(self):
        return {"state": self._state, "error": getattr(self, "_error", None)}

    @property
    def uuid(self):
        return self._uuid

    @property
    def output_pbw(self):
        return self._output_pbw

