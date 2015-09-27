import concurrent.futures
import os
import re
import requests
import time

# Process to create a new app:
# https://dev-portal.getpebble.com/applications/new?type=[watchapp|watchface] -> get CSRF token/"authenticity_token"
# POST 
#    utf8:✓
#    authenticity_token:9CkkDABMMBqEy+8F7f0iKdGm2Dmnm7K9zTaaqcEkDUE=
#    application[type]:watchapp
#    application[title]:Test app
#    application[link_to_website]:
#    application[link_to_source]:
#    application[support_email]:
#    application[category_id]:5261a8fb3b773043d500000c
#    application[list_image]:https://www.filepicker.io/api/file/OAzOFXbTStqHXgNjwxIo
#    application[icon_image]:https://www.filepicker.io/api/file/ES4KRCJT5ONcS8hlLu8k
#    commit:Create
# Then, "upload" the binary
# POST /applications/<id>/releases
#    utf8:✓
#    authenticity_token:9CkkDABMMBqEy+8F7f0iKdGm2Dmnm7K9zTaaqcEkDUE=
#    release[pbw_file]:https://www.filepicker.io/api/file/kls5BzkTeKOGnjYOFsMA
#    release[release_notes]:This is not a release
#    commit:Save
# Then, wait until the "enable timeline" button appears
# POST /applications/<id>/enable_timeline
#    _method:post
#     authenticity_token:9CkkDABMMBqEy+8F7f0iKdGm2Dmnm7K9zTaaqcEkDUE=
# We don't really need to publish a release - the timeline is now enabled and nobody else can upload an app with that UUID


class PASAutomation:
    _worker_pool = concurrent.futures.ThreadPoolExecutor(max_workers=1)
    _redis = None
    @classmethod
    def reserve_app(cls, app_metadata, pbw_url):
        if not cls._redis:
            import redis as redis_client
            cls._redis = redis_client.Redis(host=os.environ.get("REDIS_HOST", "localhost"))
        # Check if we've already created this app on the PAS
        if not cls._redis.exists("sand-app-%s" % app_metadata["uuid"]):
            cls._redis.set("sand-app-pending-%s" % app_metadata["uuid"], pbw_url)
            future = cls._worker_pool.submit(cls._run, app_metadata, pbw_url)
            def completion_callback(future):
                err = future.exception()
                if err:
                    raise err
                else:
                    cls._redis.delete("sand-app-pending-%s" % app_metadata["uuid"])
            future.add_done_callback(completion_callback)

    @classmethod
    def _run(cls, app_metadata, pbw_url):
        session = requests.Session()
        session.cookies["_PebbleDevPortal_session"] = os.environ.get("PAS_AUTOMATION_SESSION_COOKIE")

        def filepickerify(url):
            # https://assets.getpebble.com/api/file/JIyK8uphQLOWQRKOQn1r/convert?cache=true&fit=crop&w=144&h=168 -> https://www.filepicker.io/api/file/JIyK8uphQLOWQRKOQn1r
            return "https://www.filepicker.io/api/file/" + url.split('/')[5]

        # First, fetch the CSRF token
        creation_page = session.get("https://dev-portal.getpebble.com/applications/new?type=watchapp").text
        csrf_token = re.search(r'content="([^"]+)" name="csrf-token"', creation_page).group(1)

        creation_request = {
            "authenticity_token": csrf_token,
            "application[type]": app_metadata["type"],
            "application[title]": app_metadata["title"],
            "application[category_id]": app_metadata["category_id"],
            "application[list_image]": filepickerify(app_metadata["list_image"]["144x144"]),
            "application[icon_image]": filepickerify(app_metadata["icon_image"]["48x48"] if "icon_image" in app_metadata else app_metadata["list_image"]["144x144"]),
            "commit": "Create"
        }
        creation_result = session.post("https://dev-portal.getpebble.com/applications", data=creation_request, allow_redirects=False)
        assert creation_result.status_code == 302, "Could not create application, got %s %s" % (creation_result.status_code, creation_result.text)
        # Wheee
        pas_uuid = creation_result.headers["location"].split('/')[-1]

        # Next, upload the pbw
        release_creation_request = {
            "authenticity_token": csrf_token,
            "release[pbw_file]": pbw_url,
            "commit": "Save"
        }

        release_creation_result = session.post("https://dev-portal.getpebble.com/applications/%s/releases" % pas_uuid, data=release_creation_request, allow_redirects=False)
        assert release_creation_result.status_code == 302, "Could not create release, got %s %s" % (release_creation_result.status_code, release_creation_result.text)

        # At this point, the app is uploaded, we never want to try again even if timeline setup fails
        cls._redis.set("sand-app-%s" % app_metadata["uuid"], pas_uuid)
        cls._redis.set("sand-app-original-%s" % app_metadata["uuid"], app_metadata["id"])

        # Wait for it to process
        retries = 20
        while True:
            if not retries:
                raise Exception("Timeline enable timed out")
            # We have to wait for the button, first...
            summary_page = session.get("https://dev-portal.getpebble.com/applications/%s" % pas_uuid).text
            if "enable_timeline" in summary_page:
                timeline_enable_result = session.post("https://dev-portal.getpebble.com/applications/%s/enable_timeline" % pas_uuid, data={"authenticity_token": csrf_token, "_method":"post"}, allow_redirects=False)
                if timeline_enable_result.status_code == 302:
                    break
            time.sleep(1)
            retries -= 1
