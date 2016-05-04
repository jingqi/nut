#!/usr/bin/python
# -*- coding: utf-8 -*-

import os,sys,errno

win32_qtsdk_root = 'D:/data/app/win/develop/QtSDK'
pro_path = os.path.abspath(os.path.join(os.path.split(sys.argv[0])[0], 'pro/nut.pro'))

platform_config = {
	'win32' : {
		'arch' : 'x86',
		'cmd_sep' : '&&',
		'qmake' : 'qmake',
		'qmake_flags' : '-r -spec win32-g++',
		'make' : 'mingw32-make',
	},
	'darwin' : {
		'arch' : 'x86_64',
		'cmd_sep' : ';',
		'qmake' : 'qmake',
		'qmake_flags' : '-r -spec macx-clang CONFIG+=x86_64',
		'make' : 'make',
	},
	'linux2' : {
		'arch' : 'x86_64',
		'cmd_sep' : ';',
		'qmake' : 'qmake',
		'qmake_flags' : '-r -spec linux-g++-64',
		'make' : 'make',
	},
}

def _setup_path():
	if sys.platform == 'win32':
		path = os.environ['path']
		path = path + ';' + os.path.abspath(os.path.join(win32_qtsdk_root, '5.3/mingw482_32/bin'))
		path = path + ';' + os.path.abspath(os.path.join(win32_qtsdk_root, 'Tools/mingw482_32/bin'))
		print(path)
		os.environ['path'] = path

def build(debug):
	# make dirs
	cfg = platform_config[sys.platform]
	build_path = os.path.abspath(os.path.join(os.path.split(sys.argv[0])[0], 'build/%s/%s/%s' % (sys.platform, cfg['arch'], 'debug' if debug else 'release')))
	try:
		os.makedirs(build_path)
	except OSError as exc:
		if exc.errno != errno.EEXIST:
			raise

	# qmake
	cd_cmd = 'cd "' + build_path + '"'
	cmd = '%s %s %s %s %s' % (cd_cmd, cfg['cmd_sep'], cfg['qmake'], pro_path, cfg['qmake_flags'])
	if debug:
		cmd += ' CONFIG+=debug'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('qmake failed')

	# make
	cmd = "%s %s %s" % (cd_cmd, cfg['cmd_sep'], cfg['make'])
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('make failed')

def clean(debug):
	cfg = platform_config[sys.platform]
	build_path = os.path.abspath(os.path.join(os.path.split(sys.argv[0])[0], 'build/%s/%s/%s' % (sys.platform, cfg['arch'], 'debug' if debug else 'release')))
	cd_cmd = 'cd "' + build_path + '"'
	cmd = '%s %s %s clean' % (cd_cmd, cfg['cmd_sep'], cfg['make'])
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('make clean failed')

def main():
	# 处理参数
	need_clean = False
	need_build = True
	debug = True
	for i in range(1, len(sys.argv)):
		if sys.argv[i] == 'help':
			print('build [debug|release] [help|build|clean|rebuild]')
			return
		elif sys.argv[i] == 'build':
			need_clean = False
			need_build = True
		elif sys.argv[i] == 'clean':
			need_clean = True
			need_build = False
		elif sys.argv[i] == 'rebuild':
			need_clean = True
			need_build = True
		elif sys.argv[i] == 'debug':
			debug = True
		elif sys.argv[i] == 'release':
			debug = False
		else:
			raise Exception('Unexpected argument: ' + sys.argv[i])

	_setup_path()

	if need_clean:
		clean()

	if need_build:
		build(debug)

if __name__ == '__main__':
	main()
