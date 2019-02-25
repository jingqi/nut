
import platform

from os.path import realpath, dirname, join, splitext
from subprocess import Popen

from nova.app import App
from nova.util import file_utils
from nova.builtin import compile_c, file_op


CWD = dirname(realpath(__file__))

app = App.instance()
ns = app.add_namespace('test_nut')
app.import_config(join(CWD, 'novaconfig_nut.py'))

## Vars
src_root = join(CWD, '../../src/test_nut')
out_dir = platform.system().lower() + '-' + ('debug' if ns['DEBUG'] == '1' else 'release')
out_root = join(CWD, out_dir)
obj_root = join(out_root, 'obj', 'test_fig')
header_root = join(out_root, 'include', 'test_fig')

## Flags
ns.append_env_flags('CPPFLAGS', '-I' + realpath(join(src_root, '..')))
ns.append_env_flags('CFLAGS', '-std=c11')
ns.append_env_flags('CXXFLAGS', '-std=c++11')

if platform.system() == 'Darwin':
    ns.append_env_flags('CXXFLAGS', '-stdlib=libc++')
    ns.append_env_flags('LDFLAGS', '-lc++')
else:
    ns.append_env_flags('LDFLAGS', '-lstdc++')

if platform.system() == 'Linux':
    ns.append_env_flags('LDFLAGS', '-lpthread', '-latomic')

ns.append_env_flags('LDFLAGS', '-L' + out_root, '-lnut')

## Dependencies

# Generate headers
for src in file_utils.iterfiles(src_root, '.h'):
    h = src
    ih = file_utils.chproot(src, src_root, header_root)
    ns.set_recipe(ih, file_op.copyfile)
    ns.add_chained_deps('@headers', ih, h)

program = join(out_root, 'test_nut' + ns['PROGRAM_SUFFIX'])
ns.set_recipe('@read_deps', compile_c.read_deps)
for src in file_utils.iterfiles(src_root, '.c', '.cpp'):
    c = src
    o = splitext(file_utils.chproot(src, src_root, obj_root))[0] + '.o'
    d = o + '.d'
    ns.add_dep(d, '@headers')
    ns.add_chained_deps(o, '@read_deps', d, c)
    ns.add_chained_deps(program, o, c)

ns.set_default_target(program)
ns.set_recipe(program, compile_c.link_program)
ns.add_dep(program, join(out_root, 'libnut' + ns['SHARED_LIB_SUFFIX']))

# run
if platform.system() == 'Windows':
    libgcc = 'libgcc_s_dw2-1.dll'
    libgcc_path = join(out_root, libgcc)
    ns.add_chained_deps('@run', libgcc_path, join(dirname(ns['CC']), libgcc))
    ns.set_recipe(libgcc_path, file_op.copyfile)

    libstdcpp = 'libstdc++-6.dll'
    libstdcpp_path = join(out_root, libstdcpp)
    ns.add_chained_deps('@run', libstdcpp_path, join(dirname(ns['CC']), libstdcpp))
    ns.set_recipe(libstdcpp_path, file_op.copyfile)

def run(target):
    p = Popen([program], cwd=dirname(program))
    p.wait()
ns.set_recipe('@run', run)
ns.add_dep('@run', program)

# clean
def clean(target):
    file_utils.remove_many(program, obj_root, header_root)
ns.set_recipe('@clean', clean)
