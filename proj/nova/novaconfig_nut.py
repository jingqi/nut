
import platform

from os.path import realpath, dirname, join, splitext

from nova.util import file_utils
from nova.builtin import compile_c, file_op


CWD = dirname(realpath(__file__))

ns = globals()['namespace']
ns.set_name('nut')

## Vars
src_root = join(CWD, '../../src/nut')
out_dir = platform.system().lower() + '-' + ('debug' if ns['DEBUG'] == '1' else 'release')
out_root = join(CWD, out_dir)
obj_root = join(out_root, 'obj/nut')
header_root = join(out_root, 'include/nut')

## Flags
ns.append_env_flags('CPPFLAGS', '-DBUILDING_NUT', '-I' + realpath(join(out_root, 'include')))
ns.append_env_flags('CFLAGS', '-std=c11')
ns.append_env_flags('CXXFLAGS', '-std=c++11')

if platform.system() == 'Darwin':
    ns.append_env_flags('CXXFLAGS', '-stdlib=libc++')
    ns.append_env_flags('LDFLAGS', '-lc++')
else:
    ns.append_env_flags('LDFLAGS', '-lstdc++')

if platform.system() == 'Linux':
    ns.append_env_flags('LDFLAGS', '-lpthread', '-ldl', '-latomic')

if platform.system() != 'Windows':
    ns.append_env_flags('CXXFLAGS', '-fPIC')

## Dependencies
so = join(out_root, 'libnut' + ns['SHARED_LIB_SUFFIX'])
ns.set_default_target(so)

# Generate headers
for src in file_utils.iterfiles(src_root, '.h'):
    h = src
    ih = file_utils.chproot(src, src_root, header_root)
    ns.set_recipe(ih, file_op.copyfile)
    ns.add_chained_deps('@headers', ih, h)

ns.set_recipe('@read_deps', compile_c.read_deps)
for src in file_utils.iterfiles(src_root, '.c', '.cpp'):
    c = src
    o = splitext(file_utils.chproot(src, src_root, obj_root))[0] + '.o'
    d = o + '.d'
    ns.add_dep(d, '@headers')
    ns.add_chained_deps(o, '@read_deps', d, c)
    ns.add_chained_deps(so, o, c)

# clean
def clean(target):
    file_utils.remove_any(so, obj_root, header_root)
ns.set_recipe('@clean', clean)
