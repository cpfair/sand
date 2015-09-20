from webassets import Bundle, Environment
import os

static_root = os.path.dirname(os.path.realpath(__file__))

css = Bundle('scss/*.scss', filters='pyscss', depends=('scss/*.scss',), output='css/app.css')

environment = Environment(static_root, "/static")
environment.register('css', css)
