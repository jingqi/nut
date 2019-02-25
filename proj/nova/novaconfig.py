
import platform

from os.path import dirname, join

from nova.app import App
from nova.util import file_utils


CWD = dirname(__file__)

app = App.instance()
ns = app.add_namespace('nut_wks')

app.import_config(join(CWD, 'novaconfig_nut.py'))
app.import_config(join(CWD, 'novaconfig_test_nut.py'))

out_dir = platform.system().lower() + '-' + ('debug' if ns['DEBUG'] == '1' else 'release')
ns.set_default_target(join(CWD, out_dir, 'test_nut' + ns['PROGRAM_SUFFIX']))

# run
ns.add_dep('@run', 'test_nut|@run')

# clean
def clean(target):
    file_utils.remove_any(join(CWD, out_dir))
ns.set_recipe('@clean', clean)
